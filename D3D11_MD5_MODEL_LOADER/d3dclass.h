////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_


/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//////////////
// INCLUDES //
//////////////
#include<d3d11.h>
#include<d3dcompiler.h>
#include<directxmath.h>
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();	
	void GetVideoCardInfo(char*, int&);

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

	struct cbPerFrame
	{
		Light  light;


	};

	void PrepareModel();

	void SetStateCullNone();

	XMMATRIX Rotate;
	XMMATRIX Scale;
	XMMATRIX Translation;
	XMMATRIX WVP;

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;

	//ADDED
	HRESULT hr;
	ID3D10Blob* VS_Buffer;
	ID3D10Blob* PS_Buffer;
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;

	ID3D11InputLayout* vertLayout;




	Light light;
	int Width = 800;
	int Height = 600;

	//Create effects constant buffer's structure//
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


	cbPerFrame constbuffPerFrame;
	ID3D11Buffer* cbPerFrameBuffer;

	XMMATRIX camProjection;
	XMMATRIX smilesWorld;

	ID3D11BlendState* d2dTransparency;
	ID3D11BlendState* Transparency;
	ID3D11SamplerState* CubesTexSamplerState;
	ID3D11RasterizerState* CCWcullMode;
	ID3D11RasterizerState* CWcullMode;
	//ID3D11RasterizerState* RSCullNone;
	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;

public:
	void GetWorldMatrix(XMMATRIX& worldMatrix);
	void GetProjectionMatrix(XMMATRIX&);

};

#endif
