////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include <vector>
#include <string>
#include<fstream>
#include<istream>
using namespace std;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "animatedmodelshaderclass.h"
#include "cameraclass.h"
#include "inputclass.h"
#include "positionclass.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(HINSTANCE hinstance, int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();
	double GetTime();
	double GetFrameTime();
	void StartTimer();
	bool UpdateScene(double time);
	bool DrawScene();
	void HandleMovementInput(InputClass*, float);
	
private:
	InputClass* m_Input;
	PositionClass* m_Position;
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	double countsPerSecond = 0.0;
	__int64 CounterStart = 0;
	__int64 frameTimeOld = 0;
	double frameTime;
	int frameCount = 0;
	int fps = 0;

	AnimatedModelShaderClass* m_AnimatedModelShader;

	struct Vertex	//Overloaded Vertex Structure
	{
		Vertex() {}
		Vertex(float x, float y, float z,
			float u, float v,
			float nx, float ny, float nz,
			float tx, float ty, float tz)
			: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz),
			tangent(tx, ty, tz) {}

		XMFLOAT3 pos;
		XMFLOAT2 texCoord;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 biTangent;

		///////////////**************new**************////////////////////
			// Will not be sent to shader
		int StartWeight;
		int WeightCount;
		///////////////**************new**************////////////////////
	};
	///////////////**************new**************////////////////////
	struct Joint
	{
		std::wstring name;
		int parentID;

		XMFLOAT3 pos;
		XMFLOAT4 orientation;
	};

	struct Weight
	{
		int jointID;
		float bias;
		XMFLOAT3 pos;
	};

	struct ModelSubset
	{
		int texArrayIndex;
		int numTriangles;

		std::vector<Vertex> vertices;
		std::vector<DWORD> indices;
		std::vector<Weight> weights;

		std::vector<XMFLOAT3> positions;

		ID3D11Buffer* vertBuff;
		ID3D11Buffer* indexBuff;
	};

	struct Model3D
	{
		int numSubsets;
		int numJoints;

		std::vector<Joint> joints;
		std::vector<ModelSubset> subsets;
	};

	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

	bool LoadMD5Model(std::wstring filename,
		Model3D& MD5Model,
		std::vector<ID3D11ShaderResourceView*>& shaderResourceViewArray,
		std::vector<std::wstring> texFileNameArray);
	bool LoadAlternativo(ID3D11Device*,
		ID3D11DeviceContext*, std::wstring);
	bool GraphicsClass::LoadTarga(std::wstring filename, int& height, int& width);


	//CUSTOMONE
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	unsigned char* m_targaData;
	ID3D11Texture2D* m_texture;
	//Textures and material variables, used for all mesh's loaded
	std::vector<ID3D11ShaderResourceView*> meshSRV;
	std::vector<std::wstring> textureNameArray;


	Model3D NewMD5Model;

	
};

#endif