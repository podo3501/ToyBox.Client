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

    float3 lightDirection;
    float  lightIntensity;

    float3 lightColor;
    float  lightPadding;
};

cbuffer MaterialCB : register(b3)
{
    uint  albedoTextureIndex;
    uint  normalTextureIndex;
    float normalIntensity;
    float roughnessIntensity;
    float metallic;
    float3 matPadding;
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
    Texture2D albedoTex = ResourceDescriptorHeap[albedoTextureIndex];
    float4 albedo = albedoTex.Sample(gSampler, input.uv);

return float4(albedo.rgb, albedo.a); //테스트용 알베도만 출력

    // normal / light setup
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N); // 탄젠트 공간 -> 월드 공간 변환 행렬 완성

    Texture2D normalTex = ResourceDescriptorHeap[normalTextureIndex];
    float3 localNormal = normalTex.Sample(gSampler, input.uv).xyz;
    localNormal = localNormal * 2.0f - 1.0f;

    // 굴곡 부스팅 (X, Y 축의 변화량을 강제로 10배 키우고 재정렬)
    localNormal.xy *= normalIntensity; 
    localNormal = normalize(localNormal);

    // 탄젠트 공간에 있던 노멀 값을 실제 월드 공간 노멀로 최종 변환!
    N = normalize(mul(localNormal, TBN));

    float3 L = normalize(-lightDirection);
    float3 V = normalize(-input.worldPos);
    float3 H = normalize(L + V);

    float NdotL = saturate(dot(N, L));
    float NdotH = saturate(dot(N, H));

    float shininess = lerp(128.0f, 2.0f, roughnessIntensity);
    float specularPower = pow(NdotH, shininess);
    float specularStrength = lerp(0.04f, 1.0f, metallic); // metallic가 높을수록 specular 강하게

    float3 specular = 
	lightColor *
        specularPower *
        specularStrength *
        lightIntensity;

    float3 diffuse =
        albedo.rgb *
        lightColor *
        (NdotL * lightIntensity);

    float3 ambient = albedo.rgb * 0.1f; //0.1 주변광. 일단 주변광을 임시 처리

    float3 finalColor = ambient + diffuse + specular;

    // =========================
    // DEBUG MODE (Surface 확인)
    // =========================

#if defined(SURFACE_DEBUG)

    // 1. Surface 값 자체 확인 (가장 중요)
    return float4(roughness, metallic, 0.0f, 1.0f);

#elif defined(SPECULAR_DEBUG)

    // 2. specular만 확인
    return float4(specular.xxx, 1.0f);

#elif defined(DIFFUSE_DEBUG)

    // 3. diffuse만 확인
    return float4(diffuse, 1.0f);

#else

    // 4. 정상 렌더링
    return float4(finalColor, albedo.a);

#endif
}