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
 
    // IBL(Environment) 관련 - 환경이 없으면 reflectionTextureIndex == 0xFFFFFFFF
    uint reflectionTextureIndex;
    uint reflectionMipCount;
    float2 envPadding;

    float4 irradianceSH[9]; // xyz만 사용, w는 패딩
};

cbuffer ObjectCB : register(b2)
{
    float4x4 world;
};

cbuffer PbrMaterialCB : register(b3)
{
    uint albedoTextureIndex;
    uint normalTextureIndex;
    uint armTextureIndex;
    float normalIntensity;
    float roughnessIntensity;
    float ambientOcclusionIntensity;
    float metallic;
    float MaterialPadding;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 worldPos : POSITION0;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 tangent  : TANGENT;
    float4 shadowPos : TEXCOORD1;
};

/*
PSInput VSMain(uint vID : SV_VertexID)
{
    PSInput output;

    // 인덱스/버텍스 버퍼 정보를 완전히 무시하고, 
    // SV_VertexID(0, 1, 2, 3...)에 따라 화면 구석 4개의 좌표를 직접 하드코딩합니다.
    // 0번: 좌상단, 1번: 우상단, 2번: 좌하단, 3번: 우하단 (삼각형 스트립 기준)
    float2 texCoords[4] = {
        float2(0.0f, 0.0f), // 좌상단
        float2(1.0f, 0.0f), // 우상단
        float2(0.0f, 1.0f), // 좌하단
        float2(1.0f, 1.0f)  // 우하단
    };

    // DirectX 클립 공간 좌표 (-1.0 ~ 1.0)
    float4 clipPositions[4] = {
        float4(-1.0f,  1.0f, 0.0f, 1.0f),
        float4( 1.0f,  1.0f, 0.0f, 1.0f),
        float4(-1.0f, -1.0f, 0.0f, 1.0f),
        float4( 1.0f, -1.0f, 0.0f, 1.0f)
    };

    // 만약 현재 드로우 콜이 인덱스 버퍼를 쓰는 일반 삼각형 리스트(Triangle List) 방식이라면
    // vID가 0, 1, 2, 3, 4, 5 순으로 들어옵니다. 사각형을 이루는 6개 정점을 안전하게 매핑합니다.
    uint id[6] = { 0, 1, 2, 2, 1, 3 };
    uint index = (vID < 6) ? id[vID] : 0;

    // 화면 가득 채우기 (정점 좌표 강제 주입)
    output.pos = clipPositions[index];
    output.uv = texCoords[index]; // 픽셀 셰이더에서 바로 쓸 수 있는 UV도 넣어줍니다.

    // 나머지 사용하지 않는 픽셀 셰이더 입력값들은 0으로 초기화
    output.worldPos = float3(0.0f, 0.0f, 0.0f);
    output.normal   = float3(0.0f, 0.0f, 1.0f);
    output.tangent  = float3(1.0f, 0.0f, 0.0f);
    output.shadowPos = float4(0.0f, 0.0f, 0.0f, 1.0f);

    return output;
}
*/

PSInput VSMain(uint vID : SV_VertexID)
{
    PSInput output;

    StructuredBuffer<uint> ib = ResourceDescriptorHeap[g_ibIndex];
    uint vertexIndex = ib[vID];

    StructuredBuffer<MeshVertex> vb = ResourceDescriptorHeap[g_vbIndex];
    MeshVertex input = vb[vertexIndex];

    float4 worldPos = mul(float4(input.pos, 1.0f), world);
    float4 viewPos = mul(worldPos, view);
    float4 shadowPos = mul(worldPos, lightViewProj);
    float4 clipPos = mul(viewPos, proj);

    // normal transform
    float3 worldNormal = normalize(mul(float4(input.normal, 0.0f), world).xyz);
    float3 worldTangent = normalize(mul(float4(input.tangent, 0.0f), world).xyz);

    output.pos = clipPos;
    output.worldPos = worldPos.xyz;
    output.shadowPos = shadowPos;
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

// 러프니스를 고려한 프레넬 (IBL용 - Schlick-Roughness)
// 일반 FresnelSchlick과 달리 roughness가 높을수록 프레넬 엣지가 완만해짐
float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    float3 F90 = max(float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness), F0);
    return F0 + (F90 - F0) * pow(saturate(1.0f - cosTheta), 5.0f);
}

// EnvBRDFApprox: Karis 2014, "Physically Based Shading on Mobile"
// 별도의 DFG LUT 텍스처 없이 specular IBL의 (scale, bias) 항을 근사
float2 EnvBRDFApprox(float roughness, float NdotV)
{
    const float4 c0 = float4(-1.0f, -0.0275f, -0.572f, 0.022f);
    const float4 c1 = float4(1.0f, 0.0425f, 1.04f, -0.04f);
    float4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28f * NdotV)) * r.x + r.y;
    return float2(-1.04f, 1.04f) * a004 + r.zw;
}

// Ramamoorthi & Hanrahan SH9 irradiance 근사식
float3 EvalIrradianceSH9(float3 n)
{
    return irradianceSH[0].rgb
         + irradianceSH[1].rgb * n.y + irradianceSH[2].rgb * n.z + irradianceSH[3].rgb * n.x
         + irradianceSH[4].rgb * (n.x * n.y) + irradianceSH[5].rgb * (n.y * n.z)
         + irradianceSH[6].rgb * (3.0f * n.z * n.z - 1.0f) + irradianceSH[7].rgb * (n.x * n.z)
         + irradianceSH[8].rgb * (n.x * n.x - n.y * n.y);
}

// ACES Filmic Tone Mapping 근사 (Narkowicz 2015)
float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

//Shadow 계산 함수
float CalculateShadow(float4 shadowPos)
{
    float3 projCoords = shadowPos.xyz / shadowPos.w;

    projCoords.x = projCoords.x * 0.5f + 0.5f;
    projCoords.y = -projCoords.y * 0.5f + 0.5f;

    if (projCoords.x < 0.0f || projCoords.x > 1.0f ||
        projCoords.y < 0.0f || projCoords.y > 1.0f)
        return 1.0f;

    Texture2D shadowMap = ResourceDescriptorHeap[shadowTextureIndex];

    float shadowDepth = shadowMap.SampleLevel(
        gSampler,
        projCoords.xy,
        0).r;

    float currentDepth = projCoords.z;

    float bias = 0.001f;

    return (currentDepth - bias <= shadowDepth)
        ? 1.0f
        : 0.3f;
}

/*
float4 PSMain_TextureTest(PSInput input) : SV_TARGET
{
    // 섀도우 맵 가져오기
    Texture2D tex = ResourceDescriptorHeap[shadowTextureIndex];
    uint width, height;
    tex.GetDimensions(width, height); 

    // 화면 해상도 비율에 맞게 UV 계산 (0.0 ~ 1.0)
    // ※ 만약 화면이 비정상적으로 나오면 float2(1920.0f, 1080.0f) 처럼 
    //    본인의 실제 창(Window) 해상도 수치를 직접 나누어 대입해 보셔도 됩니다.
    float2 screenUV = input.pos.xy / float2(width, height); 

    // 섀도우 맵 깊이 값 샘플링
    float4 texSample = tex.SampleLevel(gSampler, screenUV, 0);

    // 화면 전체에 흑백 깊이 값 출력!
    return float4(texSample.rgb, 1.0f);
}
*/

float4 PSMain(PSInput input) : SV_TARGET
{
//float3 projCoords = input.shadowPos.xyz / input.shadowPos.w;
//float2 shadowUV;
//shadowUV.x = projCoords.x * 0.5f + 0.5f;
//shadowUV.y = -projCoords.y * 0.5f + 0.5f; // 만약 Y축 반전이 있다면

//return float4(shadowUV, 0.0f, 1.0f); // X는 Red, Y는 Green으로 출력

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

    // 3. ARM(Ambient Occlusion, Roughness, Metallic)
    Texture2D armTex = ResourceDescriptorHeap[armTextureIndex];
    float4 armSample = armTex.Sample(gSampler, input.uv);

    float sampledAO = armSample.r; // R 채널 = Ambient Occlusion
    float sampledRoughness = armSample.g; // G 채널 = Roughness
    float sampledMetallic  = armSample.b; // B 채널 = Metallic

    // 4. PBR에 필요한 핵심 3대 방향 벡터 계산 (L, V, H)
    float3 L = normalize(-lightDirection);
    float3 V = normalize(cameraPosition - input.worldPos);     
    float3 H = normalize(L + V);

    // 5. 내적값 계산 및 saturate
    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));

    // 6. PBR 재질 데이터 가공
    float PBR_Roughness = clamp(sampledRoughness * roughnessIntensity, 0.05f, 1.0f);
    float PBR_Metallic  = saturate(sampledMetallic * metallic);

    // 7. Cook-Torrance PBR BRDF 연산 시작
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

    // 8. 에너지 보존 법칙 적용 (kD, kS 계산)
    float3 kS = F;              
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS; 
    kD *= (1.0f - PBR_Metallic); 

    // 9. 디퓨즈 컬러 (빛 번짐) 연산
    float3 diffuseColor = kD * albedo.rgb / PI;
    
    // 10. 최종 셰이딩 라이트 결합 (디퓨즈 + 스펙큘러)
    //float3 finalLight = (diffuseColor + specularBRDF) * lightColor * (NdotL * lightIntensity);
    float3 finalSpecular = (NdotL > 0.0f) ? specularBRDF : float3(0.0f, 0.0f, 0.0f);
    //float3 finalLight = (diffuseColor + finalSpecular) * lightColor * (NdotL * lightIntensity);
    float shadow = CalculateShadow(input.shadowPos);
    float3 finalLight = (diffuseColor + finalSpecular) * lightColor * (NdotL * lightIntensity) * shadow;

    // 11. 주변광(Ambient) 처리
    float highContrastAO = pow(saturate(sampledAO), 2.0f);
    float PBR_AO = lerp(1.0f, highContrastAO, ambientOcclusionIntensity);

    float3 iblDiffuse;
    float3 iblSpecular;

    if (reflectionTextureIndex != 0xFFFFFFFF)
    {
        // Diffuse IBL - SH로부터 irradiance 평가
        float3 irradiance = EvalIrradianceSH9(N);
        // kD를 재사용하지 않고 IBL 전용 kS/kD를 다시 구함
        // (roughness를 고려한 프레넬이라 직접광의 kS와 값이 다름)
        float3 iblKS = FresnelSchlickRoughness(NdotV, F0, PBR_Roughness);
        float3 iblKD = (float3(1.0f, 1.0f, 1.0f) - iblKS) * (1.0f - PBR_Metallic);
        iblDiffuse = iblKD * albedo.rgb * irradiance;

        // Specular IBL - roughness -> mip 매핑으로 프리필터링된 큐브맵 샘플링
        float3 R = reflect(-V, N);
        TextureCube reflectionTex = ResourceDescriptorHeap[reflectionTextureIndex];
        float mipLevel = PBR_Roughness * float(reflectionMipCount - 1);
        float3 prefilteredColor = reflectionTex.SampleLevel(gSampler, R, mipLevel).rgb;

        float2 envBRDF = EnvBRDFApprox(PBR_Roughness, NdotV);
        iblSpecular = prefilteredColor * (F0 * envBRDF.x + envBRDF.y);
    }
    else
    {
        // 환경이 없는 경우 - 기존의 단순 flat ambient로 폴백
        iblDiffuse = albedo.rgb * 0.02f;
        iblSpecular = float3(0.0f, 0.0f, 0.0f);
    }
 
    //float3 ambient = albedo.rgb * 0.02f * PBR_AO;
    float3 ambient = (iblDiffuse + iblSpecular) * PBR_AO;
    float3 finalColor = ambient + finalLight;
 
    // 12. 톤매핑 (HDR -> LDR) - 감마 보정 전에 반드시 먼저 적용
    finalColor *= 1.0f;
    finalColor = ACESFilm(finalColor);
    // 13. 감마 보정 (선형 공간 연산을 모니터 공간으로 출력)
    finalColor = pow(finalColor, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));

    // 최종 결과 출력!!
    return float4(finalColor, albedo.a);
}