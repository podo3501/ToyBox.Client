struct UIVertex
{
    float3 pos;
    float4 color;
    float2 uv;
    uint mode; // 0: UI, 1: BitmapText, 2: SDF, 3: MSDF
    uint textureIndex; // Bindless SRV Heap Index
    float pxRange;     // SDF/MTSDF일 때만 유효. bake 시점 texel 단위 range
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
    nointerpolation float pxRange : PXRANGE;
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
    output.pxRange = input.pxRange;

    return output;
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
    else if (input.mode == 2) // Mode 2: SDF Font
    {
        float distance = texColor.r;
        // 텍스처 UV 변화율을 기반으로 안티앨리어싱 폭(Smoothing Edge Width)을 계산
        // 화면 해상도나 확대/축소 비율에 맞춰 경계를 자동으로 부드럽게 만듦.
        float edgeSmoothing = fwidth(distance);
        float alpha = smoothstep(0.5f - edgeSmoothing, 0.5f + edgeSmoothing, distance); // 0.5 경계값을 기준으로 smoothstep을 적용해 부드러운 알파 알파 마스크 생성
        finalColor = float4(input.color.rgb, input.color.a * alpha);
    }
    else if (input.mode == 3) // Mode 3: MTSDF Font
    {
//        float distance = median(texColor.r, texColor.g, texColor.b);
//        float edgeSmoothing = fwidth(distance);
//        float alpha = smoothstep(0.5f - edgeSmoothing, 0.5f + edgeSmoothing, distance);
//        finalColor = float4(input.color.rgb, input.color.a * alpha);

        float texW, texH;
        uiTex.GetDimensions(texW, texH);

        float screenPxRange = ScreenPxRange(input.uv, input.pxRange, float2(texW, texH));
        float distance = median(texColor.r, texColor.g, texColor.b);
        //float sd = (distance - 0.5f) * screenPxRange;
float sd = (distance - 0.5f) * 4.0f;

        float alpha = saturate(sd + 0.5f);
        finalColor = float4(input.color.rgb, input.color.a * alpha);
    }
    else // 예외 예비 처리 (기존 UI 로직 적용)
    {
        finalColor = texColor * input.color;
    }

    return finalColor;
}