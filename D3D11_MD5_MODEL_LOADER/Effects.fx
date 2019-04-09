
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

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	input.normal = normalize(input.normal);	

	//Set diffuse color of material
	float4 diffuse = difColor;

	//If material has a diffuse texture map, set it now
	if(hasTexture == true)
		diffuse = ObjTexture.Sample( ObjSamplerState, input.TexCoord );

	//If material has a normal map, we can set it now
	if(hasNormMap == true)
	{
		//Load normal from normal map
		float4 normalMap = ObjNormMap.Sample( ObjSamplerState, input.TexCoord );

		//Change normal map range from [0, 1] to [-1, 1]
		normalMap = (2.0f*normalMap) - 1.0f;

		//Make sure tangent is completely orthogonal to normal
		input.tangent = normalize(input.tangent - dot(input.tangent, input.normal)*input.normal);

		//Create the biTangent
		float3 biTangent = cross(input.normal, input.tangent);

		//Create the "Texture Space"
		float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);

		//Convert normal from normal map to texture space and store in input.normal
		input.normal = normalize(mul(normalMap, texSpace));
	}

	float3 finalColor;

	finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(light.dir, input.normal) * light.diffuse * diffuse);
	
	return float4(finalColor, diffuse.a);
}

