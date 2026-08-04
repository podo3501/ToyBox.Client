SamplerState gSampler : register(s0);

cbuffer SkyboxCB : register(b0)
{
    float4x4 invViewProj; // translation 제거된 view * proj의 역행렬
    uint skyboxTextureIndex;
    uint3 padding;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 viewDir : TEXCOORD0;
};

// 풀스크린 트라이앵글 (버텍스/인덱스 버퍼 없이 SV_VertexID만으로 화면 전체를 덮는 삼각형 하나 생성)
PSInput VSMain(uint vID : SV_VertexID)
{
    PSInput output;

    // vID: 0, 1, 2 -> 화면을 다 덮는 큰 삼각형의 클립 공간 좌표
    float2 uv = float2((vID << 1) & 2, vID & 2);
    float2 clipXY = uv * 2.0f - 1.0f;

    // z = 1.0 (far plane) 고정 -> depth test LESS_EQUAL과 맞물려 항상 가장 뒤에 위치
    float4 clipPos = float4(clipXY.x, -clipXY.y, 1.0f, 1.0f);

    // 클립 공간 -> 월드 공간 역투영으로 이 픽셀이 가리키는 월드 방향 벡터 계산
    float4 worldPos = mul(clipPos, invViewProj);
    worldPos /= worldPos.w;

    output.pos = clipPos;
    output.viewDir = worldPos.xyz; // 카메라가 원점 고정이므로 이 벡터 자체가 샘플링 방향
    return output;
}

float3 ACESFilm(float3 x)
{
    float a = 2.51f, b = 0.03f, c = 2.43f, d = 0.59f, e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

//float4 PSMain(PSInput input) : SV_TARGET
//{
  //  TextureCube skyboxTex = ResourceDescriptorHeap[skyboxTextureIndex];
//    float3 dir = normalize(input.viewDir);
//    float3 color = skyboxTex.Sample(gSampler, dir).rgb;

    // 필요하면 노출 조절: color *= exposure;
//    color = ACESFilm(color);                 // 톤매핑: HDR -> 0~1
//    color = pow(color, 1.0f / 2.2f);          // 감마: linear -> sRGB

//    return float4(color, 1.0f);
//}

float4 PSMain(PSInput input) : SV_TARGET
{
    TextureCube skyboxTex = ResourceDescriptorHeap[skyboxTextureIndex];

    float3 dir = normalize(input.viewDir);
    float3 color = skyboxTex.Sample(gSampler, dir).rgb;

color = ACESFilm(color); 
    // PBR 셰이더와 동일한 감마 보정 (선형 공간 -> 모니터 공간)
    //color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));

    return float4(color, 1.0f);
}