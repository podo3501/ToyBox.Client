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
    nointerpolation uint mode : MODE;
    nointerpolation uint textureIndex : TEXINDEX;
    UITextProps textProps : TEXTPROPERTIES;
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

static const float kOutlineWeightTable[10] =
{
    0.0f,
    0.5f,
    0.8f,
    1.1f,
    1.4f,
    1.7f,
    2.0f,
    2.3f,
    2.6f,
    2.9f,
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

        float outlineWidthPx = UnpackOutlineWeight(input.textProps.params);
        float3 outlineColor = UnpackOutlineColor(input.textProps.params);

        float fillAlpha = saturate(sd + 0.5f);
        if (outlineWidthPx > 0.0f)
        {
            float outlineAlpha = saturate(sd + outlineWidthPx + 0.5f);
            float3 rgb = lerp(outlineColor.rgb, input.color.rgb, fillAlpha);
            float a = max(fillAlpha, outlineAlpha) * input.color.a;
            finalColor = float4(rgb, a);
        }
        else
        {
            finalColor = float4(input.color.rgb, input.color.a * fillAlpha);
        }
    }
    else // 예외 예비 처리 (기존 UI 로직 적용)
    {
        finalColor = texColor * input.color;
    }

    return finalColor;
}