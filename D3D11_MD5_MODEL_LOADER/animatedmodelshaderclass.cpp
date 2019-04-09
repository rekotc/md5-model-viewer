#include "animatedmodelshaderclass.h"

AnimatedModelShaderClass::AnimatedModelShaderClass()
{
	VS = 0;
	PS = 0;
	vertLayout = 0;

	CubesTexSamplerState = 0;
	cbPerObjectBuffer = 0;
	cbPerFrameBuffer = 0;



}


AnimatedModelShaderClass::AnimatedModelShaderClass(const AnimatedModelShaderClass& other)
{
}


AnimatedModelShaderClass::~AnimatedModelShaderClass()
{
}
bool AnimatedModelShaderClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd){


	return InitializeShader( device,  deviceContext,  hwnd);

	

}

bool AnimatedModelShaderClass::InitializeShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd) {

	HRESULT result;

	ID3D10Blob* errorMessage;
	ID3D10Blob* VS_Buffer;
	ID3D10Blob* PS_Buffer;

	errorMessage = 0;
	VS_Buffer = 0;
	PS_Buffer = 0;

	// Compile the vertex shader code.
	result = D3DCompileFromFile(L"shader.vs", NULL, NULL, "VS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&VS_Buffer, &errorMessage);

	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, L"shader.vs");
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, L"shader.vs", L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the vertex shader code.
	result = D3DCompileFromFile(L"shader.ps", NULL, NULL, "PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&PS_Buffer, &errorMessage);

	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, L"shader.ps");
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, L"shader.ps", L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//Create the Shader Objects
	result = device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	result = device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);


	//Set Vertex and Pixel Shaders
	deviceContext->VSSetShader(VS, 0, 0);
	deviceContext->PSSetShader(PS, 0, 0);

	light.pos = XMFLOAT3(0.0f, 0.0f, 15.0f);
	light.dir = XMFLOAT3(0.0f, 0.0f, -1.00f);
	light.range = 1000.0f;
	light.cone = 40.0f;
	light.att = XMFLOAT3(0.4f, 0.02f, 0.000f);
	light.ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	UINT numElements = ARRAYSIZE(layout);

	result = device->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(),
		VS_Buffer->GetBufferSize(), &vertLayout);

	//Set the Input Layout
	deviceContext->IASetInputLayout(vertLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Create the buffer to send to the cbuffer in effect file
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	result = device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

	//Create the buffer to send to the cbuffer per frame in effect file
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerFrame);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	result = device->CreateBuffer(&cbbd, NULL, &cbPerFrameBuffer);

	//// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	result = device->CreateSamplerState(&sampDesc, &CubesTexSamplerState);


	if (FAILED(result))
		return false;

	return true;

}

bool AnimatedModelShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext/*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView**/) {

	constbuffPerFrame.light = light;
	deviceContext->UpdateSubresource(cbPerFrameBuffer, 0, NULL, &constbuffPerFrame, 0, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer);

	//Spostato nel d3d initialize
	////Set our Render Target
	//m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//Set the default blend state (no blending) for opaque objects
	deviceContext->OMSetBlendState(0, 0, 0xffffffff);

	//Set Vertex and Pixel Shaders
	deviceContext->VSSetShader(VS, 0, 0);
	deviceContext->PSSetShader(PS, 0, 0);

	return true;
}

void XM_CALLCONV AnimatedModelShaderClass::PrepareToDraw(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* mesh) {
	   	
	WVP = worldMatrix * viewMatrix* projectionMatrix;	
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	cbPerObj.World = XMMatrixTranspose(worldMatrix);
	cbPerObj.hasTexture = true;		// We'll assume all md5 subsets have textures
	cbPerObj.hasNormMap = false;	// We'll also assume md5 models have no normal map (easy to change later though)
	deviceContext->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &cbPerObjectBuffer);
	deviceContext->PSSetShaderResources(0, 1, &mesh);
	deviceContext->PSSetSamplers(0, 1, &CubesTexSamplerState);
	//deviceContext->RSSetState(RSCullNone);
}

void AnimatedModelShaderClass::PrepareModel() {

	Scale = XMMatrixScaling(0.04f, 0.04f, 0.04f);			// The model is a bit too large for our scene, so make it smaller
	Translation = XMMatrixTranslation(0.0f, 3.0f, 0.0f);

	m_smilesWorld = Scale * Translation;
}

void AnimatedModelShaderClass::GetSmilesWorld(XMMATRIX& smilesWorld)
{
	smilesWorld = m_smilesWorld;
	return;
}

void AnimatedModelShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

ID3D11VertexShader* AnimatedModelShaderClass::getVS() {
	return VS;
}
ID3D11PixelShader* AnimatedModelShaderClass::getPS() {
	return PS;
}
AnimatedModelShaderClass::cbPerFrame* AnimatedModelShaderClass::getCbPerFrame() {
	return &constbuffPerFrame;
}
