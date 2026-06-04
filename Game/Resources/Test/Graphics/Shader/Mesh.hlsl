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

cbuffer ObjectCB : register(b1)
{
    float4x4 world;
};

cbuffer MeshFrameCB : register(b2)
{
    float4x4 view;
    float4x4 proj;

    float3 cameraPosition;
    float cameraPadding;

    float3 lightDirection;
    float  lightIntensity;

    float3 lightColor;
    float  lightPadding;
};

cbuffer MaterialCB : register(b3)
{
    uint albedoTextureIndex;
    uint normalTextureIndex;
    uint roughnessTextureIndex;
    uint ambientOcclusionTextureIndex;
    float normalIntensity;
    float roughnessIntensity;
    float ambientOcclusionIntensity;
    float metallic;
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


static const float PI = 3.14159265359f;

// 정규화된 디즈니/GGX 러프니스 항 (D 항)
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = saturate(dot(N, H));
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / max(denom, 0.000001f); // 0 나누기 방지
}

// 기하 차폐 항 (G 항 - Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float nom   = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

// 기하 차폐 통합 항 (Smith)
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = saturate(dot(N, V));
    float NdotL = saturate(dot(N, L));
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// 프레넬 반사율 항 (F 항 - Schlick)
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(saturate(1.0f - cosTheta), 5.0f);
}


float4 PSMain(PSInput input) : SV_TARGET
{
//Texture2D testTex = ResourceDescriptorHeap[ambientOcclusionTextureIndex];
//float4 testSample = testTex.Sample(gSampler, input.uv);
//return float4(testSample.rgb, testSample.a);

    // 1. 알베도 샘플링
    Texture2D albedoTex = ResourceDescriptorHeap[albedoTextureIndex];
    float4 albedo = albedoTex.Sample(gSampler, input.uv);

    // 2. 노멀 및 TBN 변환
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N); 

    Texture2D normalTex = ResourceDescriptorHeap[normalTextureIndex];
    float3 localNormal = normalTex.Sample(gSampler, input.uv).xyz;
    localNormal = localNormal * 2.0f - 1.0f;
    localNormal = normalize(localNormal);

    // 탄젠트 공간 -> 월드 공간 노멀 최종 변환
    N = normalize(mul(localNormal, TBN));

    // 3. PBR에 필요한 핵심 3대 방향 벡터 계산 (L, V, H)
    float3 L = normalize(lightDirection);
    //float3 L = normalize(float3(1.0f, 0.0f, 0.0f));
    float3 V = normalize(cameraPosition - input.worldPos);     
    float3 H = normalize(L + V);

    // 4. 내적값 계산 및 saturate
    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));

    // 5. PBR 재질 데이터 가공 (여기는 일단 테스트용 고정값을 쓰고, 나중에 C++ 연동해줘!)
    //float PBR_Roughness = 0.2f; // 너무 낮으면 바늘구멍이 되니 0.35가 딱 좋아
    Texture2D roughnessTex = ResourceDescriptorHeap[roughnessTextureIndex];
    float sampledRoughness = roughnessTex.Sample(gSampler, input.uv).r;
    float PBR_Roughness = clamp(sampledRoughness * roughnessIntensity, 0.05f, 1.0f);
    float PBR_Metallic  = saturate(metallic); 

    // 6. Cook-Torrance PBR BRDF 연산 시작
    float3 F0 = float3(0.04f, 0.04f, 0.04f); 
    F0 = lerp(F0, albedo.rgb, PBR_Metallic);

    // D, G, F 항 연산
    float  D = DistributionGGX(N, H, PBR_Roughness);
    float  G = GeometrySmith(N, V, L, PBR_Roughness);
    float3 F = FresnelSchlick(saturate(dot(H, V)), F0);

    // Specular BRDF 분모 분자 결합
    float3 numerator    = D * G * F;
    float  denominator  = 4.0f * NdotV * NdotL;
    float3 specularBRDF = numerator / max(denominator, 0.0001f); 

    // 7. 에너지 보존 법칙 적용 (kD, kS 계산)
    float3 kS = F;              
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS; 
    kD *= (1.0f - PBR_Metallic); 

    // 8. 디퓨즈 컬러 (빛 번짐) 연산
    float3 diffuseColor = kD * albedo.rgb / PI;
    
    // 9. 최종 셰이딩 라이트 결합 (디퓨즈 + 스펙큘러)
    //float3 finalLight = (diffuseColor + specularBRDF) * lightColor * (NdotL * lightIntensity);
    float3 finalSpecular = (NdotL > 0.0f) ? specularBRDF : float3(0.0f, 0.0f, 0.0f);
    float3 finalLight = (diffuseColor + finalSpecular) * lightColor * (NdotL * lightIntensity);

    // 10. 주변광(Ambient) 처리
    Texture2D aoTex = ResourceDescriptorHeap[ambientOcclusionTextureIndex];
    float sampledAO = aoTex.Sample(gSampler, input.uv).r;
    float highContrastAO = pow(saturate(sampledAO), 2.0f);
    float PBR_AO = lerp(1.0f, highContrastAO, ambientOcclusionIntensity);
 
    float3 ambient = albedo.rgb * 0.02f * PBR_AO;
    float3 finalColor = ambient + finalLight;
 
    // 11. 감마 보정 (선형 공간 연산을 모니터 공간으로 출력)
    finalColor = pow(finalColor, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));

    // 최종 결과 출력!!
    return float4(finalColor, albedo.a);
}