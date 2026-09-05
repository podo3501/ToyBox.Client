struct UITextProps
{
    nointerpolation float pxRange;     // SDF/MTSDF일 때만 유효. bake 시점 texel 단위 range
    nointerpolation float4 clipRect;
    nointerpolation uint params1;
    nointerpolation uint params2;
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
};

cbuffer UIDrawCB : register(b1)
{
    float4x4 projection;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float2 localUV : LOCALUV; //글자 그라데이션을 위해서.
    float2 localPos : LOCALPOS; //클리핑을 위해서.
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

    float4 worldPos = float4(input.pos, 1.0f);

    output.pos = mul(worldPos, projection);
    output.color = input.color;
    output.localUV = kQuadCornerUV[vertexIndex % 4];
    output.localPos = input.pos.xy;
    output.uv = input.uv; // uvTransform도 CPU에서 이미 적용.
    output.mode = input.mode;
    output.textureIndex = input.textureIndex;
    output.textProps = input.textProps;

    return output;
}

uint UnpackNibble(uint packed, uint nibbleIndex)
{
    return (packed >> (nibbleIndex * 4)) & 0x0F;
}

static const float kOutlineWeightTable[16] =
{
    0.000f,
    0.012f,
    0.024f,
    0.038f,
    0.053f,
    0.070f,
    0.090f,
    0.112f,
    0.137f,
    0.165f,
    0.197f,
    0.234f,
    0.277f,
    0.327f,
    0.392f,
    0.465f,
};

float UnpackOutlineWeight(uint params)
{
    uint weightIndex = UnpackNibble(params, 0);
    return kOutlineWeightTable[weightIndex];
}

static const float3 kColorTable[16] =
{
    float3(0.00f, 0.00f, 0.00f), // Black
    float3(0.25f, 0.25f, 0.25f), // DarkGray
    float3(0.50f, 0.50f, 0.50f), // Gray
    float3(0.75f, 0.75f, 0.75f), // LightGray
    float3(1.00f, 1.00f, 1.00f), // White

    float3(0.80f, 0.15f, 0.15f), // Red
    float3(0.15f, 0.75f, 0.20f), // Green
    float3(0.20f, 0.45f, 0.95f), // Blue
    float3(1.00f, 0.90f, 0.20f), // Yellow

    float3(1.00f, 0.55f, 0.15f), // Orange
    float3(0.70f, 0.30f, 0.90f), // Purple
    float3(0.15f, 0.85f, 0.85f), // Cyan

    float3(0.00f, 0.00f, 0.35f), // Navy
    float3(0.21f, 0.27f, 0.31f), // Charcoal
    float3(0.25f, 0.13f, 0.08f), // DarkBrown
    float3(0.83f, 0.69f, 0.22f), // Gold
};

float3 UnpackOutlineColor(uint params)
{
    uint colorIndex = UnpackNibble(params, 1);
    return kColorTable[colorIndex];
}

static const float kShadowTable[16] =
{
    0.000f,
    0.020f,
    0.045f,
    0.075f,
    0.110f,
    0.155f,
    0.210f,
    0.275f,
    0.350f,
    0.440f,
    0.540f,
    0.650f,
    0.760f,
    0.860f,
    0.940f,
    1.000f,
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
    return kColorTable[idx];
}

uint UnpackGradientStartIndex(uint params)
{
    return UnpackNibble(params, 5);
}

uint UnpackGradientEndIndex(uint params)
{
    return UnpackNibble(params, 6);
}

// --- 아우터 글로우 (params2) ---
// 아웃라인(최대 0.45 * screenPxRange)보다 훨씬 넓게 퍼져야 해서 별도 테이블 사용.
static const float kGlowRangeTable[16] =
{
    0.000f,
    0.030f,
    0.062f,
    0.096f,
    0.132f,
    0.171f,
    0.213f,
    0.258f,
    0.306f,
    0.357f,
    0.411f,
    0.468f,
    0.528f,
    0.591f,
    0.657f,
    0.700f,
};

float UnpackGlowRange(uint params2)
{
    uint idx = UnpackNibble(params2, 0);
    return kGlowRangeTable[idx];
}

float UnpackGlowIntensity(uint params2)
{
    // 4bit 그대로 0~1 선형 매핑 (별도 테이블 불필요, 16단계면 충분히 촘촘함)
    uint idx = UnpackNibble(params2, 1);
    return idx / 15.0f;
}

float3 UnpackGlowColor(uint params2)
{
    uint idx = UnpackNibble(params2, 2);
    return kColorTable[idx];
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

void CompositeOver(inout float3 outRgb, inout float outA, float3 topRgbStraight, float topA)
{
    float3 topPremult = topRgbStraight * topA;
    outRgb = topPremult + outRgb * (1.0f - topA);
    outA = topA + outA * (1.0f - topA);
}

float4 PSMain(PSInput input) : SV_TARGET
{
    bool clipEnabled = (input.textProps.clipRect.z > 0.0f && input.textProps.clipRect.w > 0.0f);

    if (clipEnabled)
    {
        float2 clipMin = input.textProps.clipRect.xy;
        float2 clipMax = input.textProps.clipRect.xy + input.textProps.clipRect.zw;

        if (input.localPos.x < clipMin.x || input.localPos.x > clipMax.x ||
    	    input.localPos.y < clipMin.y || input.localPos.y > clipMax.y)
	    discard;
    }

    Texture2D uiTex = ResourceDescriptorHeap[input.textureIndex];
    float4 texColor = uiTex.Sample(samp, input.uv);
    float4 finalColor = float4(0, 0, 0, 0);

    if (input.mode == 0) // Mode 0: 일반 UI (RGBA 컬러 텍스처)
    {
        // texColor.rgb는 이미 premultiplied. tint(input.color)는 straight이므로
        // rgb는 tint.rgb * tint.a로, alpha는 tint.a로만 추가 스케일링한다.
        finalColor.rgb = texColor.rgb * input.color.rgb * input.color.a;
        finalColor.a   = texColor.a   * input.color.a;
    }
    else if (input.mode == 1) // Mode 1: BitmapText (R 채널 알파 마스크, PMA 무관)
    {
        float alpha = texColor.r;
        float finalAlpha = input.color.a * alpha;
        finalColor = float4(input.color.rgb * finalAlpha, finalAlpha);
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
	uint gradStartIdx = UnpackGradientStartIndex(input.textProps.params1);
        uint gradEndIdx = UnpackGradientEndIndex(input.textProps.params1);

	float3 fillColor = input.color.rgb;
        if (gradStartIdx != gradEndIdx)
        {
            float3 gradStart = kColorTable[gradStartIdx];
            float3 gradEnd = kColorTable[gradEndIdx];
            fillColor = lerp(gradStart, gradEnd, saturate(input.localUV.y));
        }

	// --- 아웃라인 ---
        float outlineWidthFrac = UnpackOutlineWeight(input.textProps.params1);
        float outlineWidthPx = outlineWidthFrac * screenPxRange;
        float3 outlineColor = UnpackOutlineColor(input.textProps.params1);

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

	float3 outRgb = float3(0, 0, 0);
        float outA = 0.0f;
	// --- 아우터 글로우 (가장 아래 레이어) ---
        float glowRange = UnpackGlowRange(input.textProps.params2) * screenPxRange;
        if (glowRange > 0.0f)
        {
            float glowIntensity = UnpackGlowIntensity(input.textProps.params2);
            float3 glowColor = UnpackGlowColor(input.textProps.params2);

            // sd=0(외곽선)에서 1, sd=-glowRange(바깥으로 glowRange만큼)에서 0으로 부드럽게 falloff.
            // sd>0(글자 내부)에서는 smoothstep이 1로 saturate되지만 위 레이어(fill/outline)가 덮으므로 무해.
            float glowAlpha = smoothstep(-glowRange, 0.0f, sd) * glowIntensity;
            CompositeOver(outRgb, outA, glowColor, glowAlpha);
        }

	// --- 드롭 섀도우 ---
        float shadowOffsetFrac = UnpackShadowOffset(input.textProps.params1);
        float shadowOffsetPx = shadowOffsetFrac * screenPxRange;
        if (shadowOffsetPx > 0.0f)
        {
            float shadowSoftnessFrac = UnpackShadowSoftness(input.textProps.params1);
            float shadowSoftness = max(shadowSoftnessFrac * screenPxRange, 0.001f); // 0 나눗셈 방지
            float3 shadowColor = UnpackShadowColor(input.textProps.params1);
    
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
            CompositeOver(outRgb, outA, shadowColor, shadowAlpha);		
        }

	CompositeOver(outRgb, outA, rgb, alpha);
	finalColor = float4(outRgb, outA);
    }
    else // 예외 예비 처리 (기존 UI 로직 적용)
    {
        finalColor.rgb = texColor.rgb * input.color.rgb * input.color.a;
        finalColor.a   = texColor.a   * input.color.a;
    }

    return finalColor;
}