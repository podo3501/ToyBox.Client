struct UITextProps
{
    nointerpolation float pxRange;     // SDF/MTSDF일 때만 유효. bake 시점 texel 단위 range
    nointerpolation uint params;
};

struct UIVertex
{
    float3 pos;
    float4 color;
    float2 uv;
    uint textureIndex; // Bindless SRV Heap Index
    uint mode; // 0: UI, 1: BitmapText, 2: MTSDF
    UITextProps textProps;
};

SamplerState samp : register(s0);

cbuffer UIIndicesCB : register(b0)
{
    uint g_vbIndex;
    uint g_ibIndex;
    uint g_textureIndex;
};

cbuffer UIDrawCB : register(b1)
{
    float4x4 world;
    float4x4 projection;
    float4 uvTransform; // x=u0, y=v0, z=u1, w=v1
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float2 localUV : LOCALUV; //글자 그라데이션을 위해서.
    nointerpolation uint mode : MODE;
    nointerpolation uint textureIndex : TEXINDEX;
    UITextProps textProps : TEXTPROPERTIES;
};

//글자 quad 하나는 항상 4개 vertex가 TL→TR→BR→BL 고정 순서로 연속 배치된다고 가정한다.
static const float2 kQuadCornerUV[4] =
{
    float2(0.0f, 0.0f), // TL
    float2(1.0f, 0.0f), // TR
    float2(1.0f, 1.0f), // BR
    float2(0.0f, 1.0f), // BL
};

PSInput VSMain(uint vID : SV_VertexID)
{
    PSInput output;

    StructuredBuffer<uint> ib = ResourceDescriptorHeap[g_ibIndex];
    uint vertexIndex = ib[vID];

    StructuredBuffer<UIVertex> vb = ResourceDescriptorHeap[g_vbIndex];
    UIVertex input = vb[vertexIndex];

    float4 localPos = float4(input.pos, 1.0f);
    float4 worldPos = mul(localPos, world);
    float4 clipPos = mul(worldPos, projection);

    output.pos = clipPos;
    output.color = input.color;
    output.localUV = kQuadCornerUV[vertexIndex % 4];
    output.uv.x = lerp(uvTransform.x, uvTransform.z, input.uv.x);
    output.uv.y = lerp(uvTransform.y, uvTransform.w, input.uv.y);
    output.mode = input.mode;
    output.textureIndex = input.textureIndex;
    output.textProps = input.textProps;

    return output;
}

uint UnpackNibble(uint packed, uint nibbleIndex)
{
    return (packed >> (nibbleIndex * 4)) & 0x0F;
}

static const float kOutlineWeightTable[13] =
{
    0.000f,
    0.015f,
    0.03f,
    0.05f,
    0.07f,
    0.10f,
    0.13f,
    0.17f,
    0.22f,
    0.27f,
    0.33f,
    0.38f,
    0.45f,
};

float UnpackOutlineWeight(uint params)
{
    uint weightIndex = UnpackNibble(params, 0);
    return kOutlineWeightTable[weightIndex];
}

static const float3 kOutlineColorTable[8] =
{
    float3(0.0f, 0.0f, 0.0f), // Black
    float3(0.25f, 0.25f, 0.25f), // DarkGray
    float3(0.75f, 0.75f, 0.75f), // LightGray
    float3(1.00f, 1.00f, 1.00f), // White
    float3(0.00f, 0.00f, 0.35f), // Navy
    float3(0.21f, 0.27f, 0.31f), // Charcoal
    float3(0.25f, 0.13f, 0.08f), // DarkBrown
    float3(0.83f, 0.69f, 0.22f), // Gold
};

float3 UnpackOutlineColor(uint params)
{
    uint colorIndex = UnpackNibble(params, 1);
    return kOutlineColorTable[colorIndex];
}

static const float kShadowTable[12] =
{
    0.00f,
    0.03f,
    0.06f,
    0.10f,
    0.15f,
    0.21f,
    0.28f,
    0.36f,
    0.45f,
    0.54f,
    0.64f,
    0.75f,
};

float UnpackShadowOffset(uint params)
{
    uint idx = UnpackNibble(params, 2);
    return kShadowTable[idx];
}

float UnpackShadowSoftness(uint params)
{
    uint idx = UnpackNibble(params, 3);
    return kShadowTable[idx];
}

float3 UnpackShadowColor(uint params)
{
    uint idx = UnpackNibble(params, 4);
    return kOutlineColorTable[idx];
}

uint UnpackGradientStartIndex(uint params)
{
    return UnpackNibble(params, 5);
}

uint UnpackGradientEndIndex(uint params)
{
    return UnpackNibble(params, 6);
}

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

// bake 시점 texel 단위 pxRange를 받아, 현재 이 픽셀이 화면상 몇 px 범위를
// 나타내는지 fwidth(uv)로 자동 역산. 렌더 스케일/확대축소를 CPU 계산 없이 대응.
float ScreenPxRange(float2 uv, float pxRange, float2 texSize)
{
    float2 unitRange = float2(pxRange, pxRange) / texSize;
    float2 screenTexSize = 1.0f / fwidth(uv);
    return max(0.5f * dot(unitRange, screenTexSize), 1.0f); // 최소 1px 보장 (pxRange=0인 mode 2/3 이외 안전장치)
}

float4 PSMain(PSInput input) : SV_TARGET
{
    Texture2D uiTex = ResourceDescriptorHeap[g_textureIndex];
    float4 texColor = uiTex.Sample(samp, input.uv);
    float4 finalColor = float4(0, 0, 0, 0);

    if (input.mode == 0) // Mode 0: 일반 UI (RGBA 컬러 텍스처)
    {
        finalColor = texColor * input.color; // 색 곱해서 tint 가능
    }
    else if (input.mode == 1) // Mode 1: BitmapText (R 채널 알파 마스크)
    {
        float alpha = texColor.r;
        finalColor = float4(input.color.rgb, input.color.a * alpha);
    }
    else if (input.mode == 2) // Mode 2: MTSDF Font
    {
        float texW, texH;
        uiTex.GetDimensions(texW, texH);

        float screenPxRange = ScreenPxRange(input.uv, input.textProps.pxRange, float2(texW, texH));
        float distance = median(texColor.r, texColor.g, texColor.b);
        float sd = (distance - 0.5f) * screenPxRange;

        // --- 그라데이션 fill 색상 계산 ---
        // start == end 인덱스면 그라데이션 없음으로 간주하고 기존 vertex color(tint)를 그대로 사용.
	uint gradStartIdx = UnpackGradientStartIndex(input.textProps.params);
        uint gradEndIdx = UnpackGradientEndIndex(input.textProps.params);

	float3 fillColor = input.color.rgb;
        if (gradStartIdx != gradEndIdx)
        {
            float3 gradStart = kOutlineColorTable[gradStartIdx];
            float3 gradEnd = kOutlineColorTable[gradEndIdx];
            fillColor = lerp(gradStart, gradEnd, saturate(input.localUV.y));
        }

	// --- 아웃라인 ---
        float outlineWidthFrac = UnpackOutlineWeight(input.textProps.params);
        float outlineWidthPx = outlineWidthFrac * screenPxRange;
        float3 outlineColor = UnpackOutlineColor(input.textProps.params);

        float fillAlpha = saturate(sd + 0.5f);
        float3 rgb;
        float alpha;
        if (outlineWidthPx > 0.0f)
        {
            float outlineAlpha = saturate(sd + outlineWidthPx + 0.5f);
            rgb = lerp(outlineColor.rgb, fillColor, fillAlpha);
            alpha = max(fillAlpha, outlineAlpha) * input.color.a;
        }
        else
        {
            rgb = fillColor;
            alpha = input.color.a * fillAlpha;
        }

	// --- 드롭 섀도우 ---
        float shadowOffsetFrac = UnpackShadowOffset(input.textProps.params);
        float shadowOffsetPx = shadowOffsetFrac * screenPxRange;
        if (shadowOffsetPx > 0.0f)
        {
            float shadowSoftnessFrac = UnpackShadowSoftness(input.textProps.params);
            float shadowSoftness = max(shadowSoftnessFrac * screenPxRange, 0.001f); // 0 나눗셈 방지
            float3 shadowColor = UnpackShadowColor(input.textProps.params);
    
            // 픽셀 단위 오프셋 -> UV 델타. 방향은 우하단 고정(1,1) 예시, 필요시 nibble로 각도화
            float2 dir = normalize(float2(1.0f, 1.0f));
            float2 uvOffset = dir * shadowOffsetPx * fwidth(input.uv);

            float4 shadowTexColor = uiTex.Sample(samp, input.uv - uvOffset);
            // 참고: fwidth(uv)는 부호가 없어서 -uvOffset로 빼는 방향을 아래쪽처럼 보이게 조정
            // (텍스처 uv.y가 아래로 증가하는 컨벤션이면 +uvOffset이 맞을 수 있으니 실제 확인 필요)

            float shadowDistance = median(shadowTexColor.r, shadowTexColor.g, shadowTexColor.b);
            float shadowSd = (shadowDistance - 0.5f) * screenPxRange;
            float shadowAlpha = saturate(shadowSd / shadowSoftness + 0.5f) * input.color.a;

            // shadow 위에 fill/outline을 alpha compositing (over 연산)
            float3 outRgb = rgb * alpha + shadowColor * shadowAlpha * (1.0f - alpha);
            float outA = alpha + shadowAlpha * (1.0f - alpha);

            finalColor = float4(outA > 0.0001f ? outRgb / outA : 0.0f, outA);
        }
        else
        {
            finalColor = float4(rgb, alpha);
        }



    }
    else // 예외 예비 처리 (기존 UI 로직 적용)
    {
        finalColor = texColor * input.color;
    }

    return finalColor;
}