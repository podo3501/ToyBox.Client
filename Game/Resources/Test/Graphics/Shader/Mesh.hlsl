cbuffer ObjectCB : register(b0)
{
    float4x4 world;
};

cbuffer FrameCB : register(b1)
{
    float4x4 view;
    float4x4 proj;

    float3 lightDirection;
    float  lightIntensity;

    float3 lightColor;
    float  padding;
};

cbuffer MaterialCB : register(b2)
{
    float roughness;
    float metallic;

    float2 materialPadding;
};

struct MeshVertex
{
    float3 pos;
    float3 normal;
    float2 uv;
};

StructuredBuffer<MeshVertex> VertexBuffer : register(t0);
StructuredBuffer<uint>   IndexBuffer  : register(t1);

Texture2D gTexture : register(t2);
SamplerState gSampler : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 worldPos : POSITION0;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

PSInput VSMain(uint vID : SV_VertexID)
{
    PSInput output;

    uint vertexIndex = IndexBuffer[vID];
    MeshVertex input = VertexBuffer[vertexIndex];

    float4 worldPos = mul(float4(input.pos, 1.0f), world);
    float4 viewPos = mul(worldPos, view);
    float4 clipPos = mul(viewPos, proj);

    // normal transform
    float3 worldNormal = normalize(mul(float4(input.normal, 0.0f), world).xyz);

    output.pos = clipPos;
    output.worldPos = worldPos.xyz;
    output.normal = worldNormal;
    output.uv = input.uv;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 albedo = gTexture.Sample(gSampler, input.uv);

    // normal / light setup
    float3 N = normalize(input.normal);
    float3 L = normalize(-lightDirection);
    float3 V = normalize(-input.worldPos);
    float3 H = normalize(L + V);

    float NdotL = saturate(dot(N, L));
    float NdotH = saturate(dot(N, H));

    float shininess = lerp(128.0f, 2.0f, roughness);
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