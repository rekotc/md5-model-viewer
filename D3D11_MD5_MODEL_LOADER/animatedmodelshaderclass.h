////////////////////////////////////////////////////////////////////////////////
// Filename: animatedmodelshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _ANIMATEDMODELSHADERCLASS_H_
#define _ANIMATEDMODELSHADERCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;
//
/////////////////////////
//// MY CLASS INCLUDES //
/////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Class name: AnimatedModelShaderClass
////////////////////////////////////////////////////////////////////////////////
class AnimatedModelShaderClass
{
public:
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND hwnd);
	AnimatedModelShaderClass();
	AnimatedModelShaderClass(const AnimatedModelShaderClass& other);	
	~AnimatedModelShaderClass();
	bool SetShaderParameters(ID3D11DeviceContext*);
	void PrepareModel();
	void XM_CALLCONV PrepareToDraw(ID3D11DeviceContext*, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* mesh);


	void GetSmilesWorld(XMMATRIX&);
	XMMATRIX WVP;
private:
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename);
	bool InitializeShader(ID3D11Device*, ID3D11DeviceContext*, HWND hwnd);

	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;

	ID3D11InputLayout* vertLayout;

	XMMATRIX Scale;
	XMMATRIX Translation;
	XMMATRIX m_smilesWorld;


	ID3D11SamplerState* CubesTexSamplerState;

	struct cbPerObject
	{
		XMMATRIX  WVP;
		XMMATRIX World;

		//These will be used for the pixel shader
		XMFLOAT4 difColor;
		BOOL hasTexture;
		//Because of HLSL structure packing, we will use windows BOOL
		//instead of bool because HLSL packs things into 4 bytes, and
		//bool is only one byte, where BOOL is 4 bytes
		BOOL hasNormMap;
	};

	cbPerObject cbPerObj;
	ID3D11Buffer* cbPerObjectBuffer;

	struct Light
	{
		Light()
		{
			ZeroMemory(this, sizeof(Light));
		}
		XMFLOAT3 pos;
		float range;
		XMFLOAT3 dir;
		float cone;
		XMFLOAT3 att;
		float pad2;
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
	};

	Light light;
	struct cbPerFrame
	{
		Light  light;
	};
	cbPerFrame constbuffPerFrame;

	ID3D11Buffer* cbPerFrameBuffer;

public:
	ID3D11VertexShader* getVS();
	ID3D11PixelShader* getPS();
	cbPerFrame* getCbPerFrame();
};


#endif