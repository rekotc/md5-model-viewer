struct Light
{
	float3 pos;
	float  range;
	float3 dir;
	float cone;
	float3 att;
	float4 ambient;
	float4 diffuse;
};

cbuffer cbPerFrame
{
	Light light;
};

cbuffer cbPerObject
{
	float4x4 WVP;
    float4x4 World;

	float4 difColor;
	bool hasTexture;
	bool hasNormMap;
};

Texture2D ObjTexture;
Texture2D ObjNormMap;
SamplerState ObjSamplerState;
TextureCube SkyMap;

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};


VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT)
{
    VS_OUTPUT output;
	
    output.Pos = mul(inPos, WVP);
	output.worldPos = mul(inPos, World);

	output.normal = mul(normal, World);

	output.tangent = mul(tangent, World);

    output.TexCoord = inTexCoord;

    return output;
}