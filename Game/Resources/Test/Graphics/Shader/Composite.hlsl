cbuffer CompositeDataCB : register(b0)
{
    uint g_colorSRVIndex; // 뷰 컬러 타겟의 bindless SRV 힙 인덱스
};

SamplerState samp : register(s0);

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

VSOutput VSMain(uint vID : SV_VertexID)
{
    VSOutput output;

    // 정점/인덱스 버퍼 없이 SV_VertexID만으로 풀스크린 삼각형을 생성
    // vID: 0 -> (0,0), 1 -> (2,0), 2 -> (0,2) 형태의 UV를 만들어 화면 전체를 덮는 삼각형을 구성
    float2 uv = float2((vID << 1) & 2, vID & 2);
    output.uv = uv;

    // UV(0~1 범위, 삼각형 밖은 화면 밖으로 잘림) -> 클립 스페이스(-1~1, Y축 반전) 변환
    output.pos = float4(uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    Texture2D<float4> viewColor = ResourceDescriptorHeap[g_colorSRVIndex];
    return viewColor.Sample(samp, input.uv);
}