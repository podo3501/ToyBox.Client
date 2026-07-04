struct MeshVertex
{
    float3 pos;
    float3 normal;
    float2 uv;
    float3 tangent;
};

SamplerState gSampler : register(s0);

cbuffer MeshIndicesCB : register(b0)
{
    uint g_vbIndex;
    uint g_ibIndex;
};

cbuffer MeshFrameCB : register(b1)
{
    float4x4 view;
    float4x4 proj;
    float4x4 lightViewProj;

    float3 cameraPosition;
    float cameraPadding;

    float3 lightDirection;
    float lightIntensity;

    float3 lightColor;
    uint shadowTextureIndex;
};

cbuffer ObjectCB : register(b2)
{
    float4x4 world;
};

cbuffer PhongMaterialCB : register(b3)
{
    uint albedoTextureIndex;
    uint normalTextureIndex;
    uint dummyTextureIndex; // Phong에서는 안 쓰므로 더미
    float normalIntensity; // 노멀 맵 강도 조절 (0.0 ~ 1.0)
    float shininess; // PBR의 roughnessIntensity 자리 재활용 (하이라이트 지수: 보통 4.0 ~ 256.0)
    float specularIntensity;   
    float ambientIntensity;    
    float MaterialPadding;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 worldPos : POSITION0;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 tangent  : TANGENT;
};

PSInput VSMain(uint vID : SV_VertexID)
{
    PSInput output;

    StructuredBuffer<uint> ib = ResourceDescriptorHeap[g_ibIndex];
    uint vertexIndex = ib[vID];

    StructuredBuffer<MeshVertex> vb = ResourceDescriptorHeap[g_vbIndex];
    MeshVertex input = vb[vertexIndex];

    float4 worldPos = mul(float4(input.pos, 1.0f), world);
    float4 viewPos = mul(worldPos, view);
    float4 clipPos = mul(viewPos, proj);

    // normal transform
    float3 worldNormal = normalize(mul(float4(input.normal, 0.0f), world).xyz);
    float3 worldTangent = normalize(mul(float4(input.tangent, 0.0f), world).xyz);

    output.pos = clipPos;
    output.worldPos = worldPos.xyz;
    output.normal = worldNormal;
    output.tangent  = worldTangent;
    output.uv = input.uv;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    // 1. 알베도 텍스처 샘플링
    Texture2D albedoTex = ResourceDescriptorHeap[albedoTextureIndex];
    float4 albedo = albedoTex.Sample(gSampler, input.uv);

    // 2. 고품질 노멀 맵 변환 및 TBN 연산
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N); 

    Texture2D normalTex = ResourceDescriptorHeap[normalTextureIndex];
    float3 localNormal = normalTex.Sample(gSampler, input.uv).xyz;
    localNormal = localNormal * 2.0f - 1.0f;
    
    // C++에서 받아온 노멀 강도(normalIntensity)를 xy축에 보정 후 변환
    localNormal.xy *= normalIntensity;
    localNormal = normalize(localNormal);
    N = normalize(mul(localNormal, TBN));

    // 3. 라이팅 처리를 위한 방향 벡터들 계산 (L, V, H)
    float3 L = normalize(-lightDirection);
    float3 V = normalize(cameraPosition - input.worldPos);     
    float3 H = normalize(L + V); // Blinn-Phong 기법의 핵심인 하프웨이 벡터

    // 4. 조명 계산용 내적(Dot Product) 값 saturate 처리
    float NdotL = saturate(dot(N, L));
    float NdotH = saturate(dot(N, H));

    // 5. Blinn-Phong 항 연산 (Diffuse + Specular)
    // [A] Diffuse (난반사)
    float3 diffuse = albedo.rgb * NdotL;

    // [B] Specular (정반사 하이라이트)
    float specPower = pow(NdotH, shininess);
    float3 specular = specPower * specularIntensity;

    // 라이트 컬러 및 라이트 자체의 Intensity(강도) 결합
    float3 finalLight = (diffuse + specular) * lightColor * lightIntensity;

    // 6. 환경광(Ambient) 처리
    float3 ambient = albedo.rgb * ambientIntensity;
    
    // 최종 색상 합성
    float3 finalColor = ambient + finalLight;

    // 7. 감마 보정 (PBR 결과물과 화면의 톤 밸런스를 맞추기 위해 선형->모니터 공간 변환)
    finalColor = pow(finalColor, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));


    // =================================================================
    // 💥 [GPU STRESS TEST ZONE] 
    // 컴파일러가 지우지 못하도록 최종 색상(finalColor)을 루프 연산에 강제로 엮습니다.
    // =================================================================
    
    // shininess 값을 루프 횟수로 재활용하거나, 고정값(예: 2000회)으로 부하를 조절하세요.
    // 값이 클수록 프레임드랍이 극심해집니다.
    uint loopCount = 2000; 

    float3 stressWeight = float3(0.0f, 0.0f, 0.0f);
    float3 seed = input.worldPos * input.uv.xyx; // 픽셀마다 다른 동적 시드값

    [loop] // 컴파일러에게 루프를 풀지(Unroll) 말고 진짜 GPU 루프 명령어로 실행하라고 강제함
    for (uint i = 0; i < loopCount; ++i)
    {
        // 1. GPU ALU 가 가장 싫어하는 삼각함수 복합 연산 무한 가동
        seed = sin(seed * 12.9898f + float3(i, i * 2, i * 3)) * 43758.5453f;
        seed = frac(seed);

        // 2. 연산 결과를 계속 누적
        stressWeight += cos(seed) * pow(saturate(dot(seed, N)), 2.0f);
    }

    // 3. 루프를 통해 나온 stressWeight를 아주 미세하게(0.000001) 최종 컬러에 더해줍니다.
    // 컴파일러 입장에서는 이 연산이 결과에 영향을 주므로 절대 루프를 최적화로 지울 수 없습니다.
    finalColor += stressWeight * 0.000001f;

    // =================================================================



    // 최종 렌더 타겟 출력
    return float4(finalColor, albedo.a);
}