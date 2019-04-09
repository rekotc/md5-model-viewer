////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#include <iostream>
using namespace std;

GraphicsClass::GraphicsClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_targaData = 0;
	m_texture = 0;
	m_AnimatedModelShader = 0;
	m_Input = 0;
	m_Position = 0;

}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(HINSTANCE hinstance, int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Create the input object.
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the Direct3D object.
	m_Direct3D = new D3DClass;
	if(!m_Direct3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the position object.
	m_Position = new PositionClass;
	if (!m_Position)
	{
		return false;
	}

	// Set the initial position and rotation.
	m_Position->SetPosition(0.0f, 0.3f, -0.27f);

	// Create the m_Camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	m_Camera->Render();
	m_Camera->RenderBaseViewMatrix();


	/*if (!LoadMD5Model(L"boy.md5mesh", NewMD5Model, meshSRV, textureNameArray))*/
	if (!LoadMD5Model(L"bob_lamp_update_export.md5mesh", NewMD5Model, meshSRV, textureNameArray))
	return false;

	// Create the Animated Model Shader object.
	m_AnimatedModelShader = new AnimatedModelShaderClass;
	if (!m_AnimatedModelShader)
	{
		return false;
	}

	//Initialize the Animated Model Shader
	result = m_AnimatedModelShader->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize AnimatedModelShader.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	
	// Release the D3D object.
	if(m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	   	  
	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	//AGGIORNA TIMER

	// run game code    
	frameCount++;
	if (GetTime() > 1.0f)
	{
		fps = frameCount;
		frameCount = 0;
		StartTimer();
	}

	frameTime = GetFrameTime();

	// Do the input frame processing.
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->IsEscapePressed() == true)
	{
		return false;
	}
	HandleMovementInput(m_Input, frameTime);


	UpdateScene(frameTime);

	// Render the graphics scene.
	
	result = DrawScene();
	if(!result)
	{
		return false;
	}

	//result = Render(); 
	//if(!result)
	//{
	//	return false;
	//}

	return true;
}


bool GraphicsClass::Render()
{
		
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);	

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}

bool GraphicsClass::UpdateScene(double time)
{

	
		return true;
}

bool GraphicsClass::DrawScene()
{
	bool result = true;

	ID3D11DeviceContext* d3d11DevCon = m_Direct3D->GetDeviceContext();

	m_Direct3D->PrepareModel(); //è sostituita da:
	//m_AnimatedModelShader->PrepareModel();

	m_Direct3D->BeginScene(1.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix;

	m_Camera->Render();
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);

	char s[256];
	sprintf(s, "Camera current position : %f : %f\n", m_Camera->GetPosition().z, m_Camera->GetPosition().y);
	OutputDebugStringA(s);

	// Get the position of the camera.
	XMFLOAT3 cameraPosition = m_Camera->GetPosition();

	m_AnimatedModelShader->SetShaderParameters(d3d11DevCon);


	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	///////////////**************new**************////////////////////
	///***Draw MD5 Model***///
	for (int i = 0; i < NewMD5Model.numSubsets; i++)
	{
		//Set the grounds index buffer
		d3d11DevCon->IASetIndexBuffer(NewMD5Model.subsets[i].indexBuff, DXGI_FORMAT_R32_UINT, 0);
		//Set the grounds vertex buffer
		d3d11DevCon->IASetVertexBuffers(0, 1, &NewMD5Model.subsets[i].vertBuff, &stride, &offset);		
		m_AnimatedModelShader->PrepareToDraw(d3d11DevCon, worldMatrix, viewMatrix, projectionMatrix, meshSRV[NewMD5Model.subsets[i].texArrayIndex]);
		m_Direct3D->SetStateCullNone();
		d3d11DevCon->DrawIndexed(NewMD5Model.subsets[i].indices.size(), 0, 0);
	}
	
	//Present the backbuffer to the screen
	//SwapChain->Present(0, 0);
	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();
	return true;
}

double GraphicsClass::GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - CounterStart) / countsPerSecond;
}

double GraphicsClass::GetFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if (tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount) / countsPerSecond;
}

void GraphicsClass::StartTimer()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	CounterStart = frequencyCount.QuadPart;
}

bool GraphicsClass::LoadMD5Model(std::wstring filename,
	Model3D& MD5Model,
	std::vector<ID3D11ShaderResourceView*>& shaderResourceViewArray,
	std::vector<std::wstring> texFileNameArray)
{

	ID3D11Device* d3d11Device = m_Direct3D->GetDevice();
	ID3D11DeviceContext* d3d11DevCon = m_Direct3D->GetDeviceContext();

	HRESULT hr;

	std::wifstream fileIn(filename.c_str());		// Open file

	std::wstring checkString;						// Stores the next string from our file

	if (fileIn)										// Check if the file was opened
	{
		while (fileIn)								// Loop until the end of the file is reached
		{
			fileIn >> checkString;					// Get next string from file

			if (checkString == L"MD5Version")		// Get MD5 version (this function supports version 10)
			{
				/*fileIn >> checkString;
				MessageBox(0, checkString.c_str(),	//display message
				L"MD5Version", MB_OK);*/
			}
			else if (checkString == L"commandline")
			{
				std::getline(fileIn, checkString);	// Ignore the rest of this line
			}
			else if (checkString == L"numJoints")
			{
				fileIn >> MD5Model.numJoints;		// Store number of joints
			}
			else if (checkString == L"numMeshes")
			{
				fileIn >> MD5Model.numSubsets;		// Store number of meshes or subsets which we will call them
			}
			else if (checkString == L"joints")
			{
				Joint tempJoint;

				fileIn >> checkString;				// Skip the "{"

				for (int i = 0; i < MD5Model.numJoints; i++)
				{
					fileIn >> tempJoint.name;		// Store joints name
					// Sometimes the names might contain spaces. If that is the case, we need to continue
					// to read the name until we get to the closing " (quotation marks)
					if (tempJoint.name[tempJoint.name.size() - 1] != '"')
					{
						wchar_t checkChar;
						bool jointNameFound = false;
						while (!jointNameFound)
						{
							checkChar = fileIn.get();

							if (checkChar == '"')
								jointNameFound = true;

							tempJoint.name += checkChar;
						}
					}

					fileIn >> tempJoint.parentID;	// Store Parent joint's ID

					fileIn >> checkString;			// Skip the "("

					// Store position of this joint (swap y and z axis if model was made in RH Coord Sys)
					fileIn >> tempJoint.pos.x >> tempJoint.pos.z >> tempJoint.pos.y;

					fileIn >> checkString >> checkString;	// Skip the ")" and "("

					// Store orientation of this joint
					fileIn >> tempJoint.orientation.x >> tempJoint.orientation.z >> tempJoint.orientation.y;

					// Remove the quotation marks from joints name
					tempJoint.name.erase(0, 1);
					tempJoint.name.erase(tempJoint.name.size() - 1, 1);

					// Compute the w axis of the quaternion (The MD5 model uses a 3D vector to describe the
					// direction the bone is facing. However, we need to turn this into a quaternion, and the way
					// quaternions work, is the xyz values describe the axis of rotation, while the w is a value
					// between 0 and 1 which describes the angle of rotation)
					float t = 1.0f - (tempJoint.orientation.x * tempJoint.orientation.x)
						- (tempJoint.orientation.y * tempJoint.orientation.y)
						- (tempJoint.orientation.z * tempJoint.orientation.z);
					if (t < 0.0f)
					{
						tempJoint.orientation.w = 0.0f;
					}
					else
					{
						tempJoint.orientation.w = -sqrtf(t);
					}

					std::getline(fileIn, checkString);		// Skip rest of this line

					MD5Model.joints.push_back(tempJoint);	// Store the joint into this models joint vector
				}

				fileIn >> checkString;					// Skip the "}"
			}
			else if (checkString == L"mesh")
			{
				ModelSubset subset;
				int numVerts, numTris, numWeights;

				fileIn >> checkString;					// Skip the "{"

				fileIn >> checkString;
				while (checkString != L"}")			// Read until '}'
				{
					// In this lesson, for the sake of simplicity, we will assume a textures filename is givin here.
					// Usually though, the name of a material (stored in a material library. Think back to the lesson on
					// loading .obj files, where the material library was contained in the file .mtl) is givin. Let this
					// be an exercise to load the material from a material library such as obj's .mtl file, instead of
					// just the texture like we will do here.
					if (checkString == L"shader")		// Load the texture or material
					{
						std::wstring fileNamePath;
						fileIn >> fileNamePath;			// Get texture's filename

						// Take spaces into account if filename or material name has a space in it
						if (fileNamePath[fileNamePath.size() - 1] != '"')
						{
							wchar_t checkChar;
							bool fileNameFound = false;
							while (!fileNameFound)
							{
								checkChar = fileIn.get();

								if (checkChar == '"')
									fileNameFound = true;

								fileNamePath += checkChar;
							}
						}

						// Remove the quotation marks from texture path
						fileNamePath.erase(0, 1);
						fileNamePath.erase(fileNamePath.size() - 1, 1);

						//check if this texture has already been loaded
						bool alreadyLoaded = false;
						for (int i = 0; i < texFileNameArray.size(); ++i)
						{
							if (fileNamePath == texFileNameArray[i])
							{
								alreadyLoaded = true;
								subset.texArrayIndex = i;
							}
						}

						//if the texture is not already loaded, load it now
						if (!alreadyLoaded)
						{
							ID3D11ShaderResourceView* tempMeshSRV;

							LoadAlternativo(d3d11Device, d3d11DevCon, fileNamePath.c_str());
							hr = d3d11Device->CreateShaderResourceView(m_texture, &srvDesc, &tempMeshSRV);

							//hr = D3DX11CreateShaderResourceViewFromFile( d3d11Device, fileNamePath.c_str(),
							//	NULL, NULL, &tempMeshSRV, NULL );



							if (SUCCEEDED(hr))
							{
								texFileNameArray.push_back(fileNamePath.c_str());
								subset.texArrayIndex = shaderResourceViewArray.size();
								shaderResourceViewArray.push_back(tempMeshSRV);
							}
							else
							{
								MessageBox(0, fileNamePath.c_str(),		//display message
									L"Could Not Open:", MB_OK);
								return false;
							}
						}

						std::getline(fileIn, checkString);				// Skip rest of this line
					}
					else if (checkString == L"numverts")
					{
						fileIn >> numVerts;								// Store number of vertices

						std::getline(fileIn, checkString);				// Skip rest of this line

						for (int i = 0; i < numVerts; i++)
						{
							Vertex tempVert;

							fileIn >> checkString						// Skip "vert # ("
								>> checkString
								>> checkString;

							fileIn >> tempVert.texCoord.x				// Store tex coords
								>> tempVert.texCoord.y;

							fileIn >> checkString;						// Skip ")"

							fileIn >> tempVert.StartWeight;				// Index of first weight this vert will be weighted to

							fileIn >> tempVert.WeightCount;				// Number of weights for this vertex

							std::getline(fileIn, checkString);			// Skip rest of this line

							subset.vertices.push_back(tempVert);		// Push back this vertex into subsets vertex vector
						}
					}
					else if (checkString == L"numtris")
					{
						fileIn >> numTris;
						subset.numTriangles = numTris;

						std::getline(fileIn, checkString);				// Skip rest of this line

						for (int i = 0; i < numTris; i++)				// Loop through each triangle
						{
							DWORD tempIndex;
							fileIn >> checkString;						// Skip "tri"
							fileIn >> checkString;						// Skip tri counter

							for (int k = 0; k < 3; k++)					// Store the 3 indices
							{
								fileIn >> tempIndex;
								subset.indices.push_back(tempIndex);
							}

							std::getline(fileIn, checkString);			// Skip rest of this line
						}
					}
					else if (checkString == L"numweights")
					{
						fileIn >> numWeights;

						std::getline(fileIn, checkString);				// Skip rest of this line

						for (int i = 0; i < numWeights; i++)
						{
							Weight tempWeight;
							fileIn >> checkString >> checkString;		// Skip "weight #"

							fileIn >> tempWeight.jointID;				// Store weight's joint ID

							fileIn >> tempWeight.bias;					// Store weight's influence over a vertex

							fileIn >> checkString;						// Skip "("

							fileIn >> tempWeight.pos.x					// Store weight's pos in joint's local space
								>> tempWeight.pos.z
								>> tempWeight.pos.y;

							std::getline(fileIn, checkString);			// Skip rest of this line

							subset.weights.push_back(tempWeight);		// Push back tempWeight into subsets Weight array
						}

					}
					else
						std::getline(fileIn, checkString);				// Skip anything else

					fileIn >> checkString;								// Skip "}"
				}

				//*** find each vertex's position using the joints and weights ***//
				for (int i = 0; i < subset.vertices.size(); ++i)
				{
					Vertex tempVert = subset.vertices[i];
					tempVert.pos = XMFLOAT3(0, 0, 0);	// Make sure the vertex's pos is cleared first

					// Sum up the joints and weights information to get vertex's position
					for (int j = 0; j < tempVert.WeightCount; ++j)
					{
						Weight tempWeight = subset.weights[tempVert.StartWeight + j];
						Joint tempJoint = MD5Model.joints[tempWeight.jointID];

						// Convert joint orientation and weight pos to vectors for easier computation
						// When converting a 3d vector to a quaternion, you should put 0 for "w", and
						// When converting a quaternion to a 3d vector, you can just ignore the "w"
						XMVECTOR tempJointOrientation = XMVectorSet(tempJoint.orientation.x, tempJoint.orientation.y, tempJoint.orientation.z, tempJoint.orientation.w);
						XMVECTOR tempWeightPos = XMVectorSet(tempWeight.pos.x, tempWeight.pos.y, tempWeight.pos.z, 0.0f);

						// We will need to use the conjugate of the joint orientation quaternion
						// To get the conjugate of a quaternion, all you have to do is inverse the x, y, and z
						XMVECTOR tempJointOrientationConjugate = XMVectorSet(-tempJoint.orientation.x, -tempJoint.orientation.y, -tempJoint.orientation.z, tempJoint.orientation.w);

						// Calculate vertex position (in joint space, eg. rotate the point around (0,0,0)) for this weight using the joint orientation quaternion and its conjugate
						// We can rotate a point using a quaternion with the equation "rotatedPoint = quaternion * point * quaternionConjugate"
						XMFLOAT3 rotatedPoint;
						XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightPos), tempJointOrientationConjugate));

						// Now move the verices position from joint space (0,0,0) to the joints position in world space, taking the weights bias into account
						// The weight bias is used because multiple weights might have an effect on the vertices final position. Each weight is attached to one joint.
						tempVert.pos.x += (tempJoint.pos.x + rotatedPoint.x) * tempWeight.bias;
						tempVert.pos.y += (tempJoint.pos.y + rotatedPoint.y) * tempWeight.bias;
						tempVert.pos.z += (tempJoint.pos.z + rotatedPoint.z) * tempWeight.bias;

						// Basically what has happened above, is we have taken the weights position relative to the joints position
						// we then rotate the weights position (so that the weight is actually being rotated around (0, 0, 0) in world space) using
						// the quaternion describing the joints rotation. We have stored this rotated point in rotatedPoint, which we then add to
						// the joints position (because we rotated the weight's position around (0,0,0) in world space, and now need to translate it
						// so that it appears to have been rotated around the joints position). Finally we multiply the answer with the weights bias,
						// or how much control the weight has over the final vertices position. All weight's bias effecting a single vertex's position
						// must add up to 1.
					}

					subset.positions.push_back(tempVert.pos);			// Store the vertices position in the position vector instead of straight into the vertex vector
					// since we can use the positions vector for certain things like collision detection or picking
					// without having to work with the entire vertex structure.
				}

				// Put the positions into the vertices for this subset
				for (int i = 0; i < subset.vertices.size(); i++)
				{
					subset.vertices[i].pos = subset.positions[i];
				}

				//*** Calculate vertex normals using normal averaging ***///
				std::vector<XMFLOAT3> tempNormal;

				//normalized and unnormalized normals
				XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

				//Used to get vectors (sides) from the position of the verts
				float vecX, vecY, vecZ;

				//Two edges of our triangle
				XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
				XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

				//Compute face normals
				for (int i = 0; i < subset.numTriangles; ++i)
				{
					//Get the vector describing one edge of our triangle (edge 0,2)
					vecX = subset.vertices[subset.indices[(i * 3)]].pos.x - subset.vertices[subset.indices[(i * 3) + 2]].pos.x;
					vecY = subset.vertices[subset.indices[(i * 3)]].pos.y - subset.vertices[subset.indices[(i * 3) + 2]].pos.y;
					vecZ = subset.vertices[subset.indices[(i * 3)]].pos.z - subset.vertices[subset.indices[(i * 3) + 2]].pos.z;
					edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our first edge

					//Get the vector describing another edge of our triangle (edge 2,1)
					vecX = subset.vertices[subset.indices[(i * 3) + 2]].pos.x - subset.vertices[subset.indices[(i * 3) + 1]].pos.x;
					vecY = subset.vertices[subset.indices[(i * 3) + 2]].pos.y - subset.vertices[subset.indices[(i * 3) + 1]].pos.y;
					vecZ = subset.vertices[subset.indices[(i * 3) + 2]].pos.z - subset.vertices[subset.indices[(i * 3) + 1]].pos.z;
					edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our second edge

					//Cross multiply the two edge vectors to get the un-normalized face normal
					XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));

					tempNormal.push_back(unnormalized);
				}

				//Compute vertex normals (normal Averaging)
				XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
				int facesUsing = 0;
				float tX, tY, tZ;	//temp axis variables

				//Go through each vertex
				for (int i = 0; i < subset.vertices.size(); ++i)
				{
					//Check which triangles use this vertex
					for (int j = 0; j < subset.numTriangles; ++j)
					{
						if (subset.indices[j * 3] == i ||
							subset.indices[(j * 3) + 1] == i ||
							subset.indices[(j * 3) + 2] == i)
						{
							tX = XMVectorGetX(normalSum) + tempNormal[j].x;
							tY = XMVectorGetY(normalSum) + tempNormal[j].y;
							tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

							normalSum = XMVectorSet(tX, tY, tZ, 0.0f);	//If a face is using the vertex, add the unormalized face normal to the normalSum

							facesUsing++;
						}
					}

					//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
					normalSum = normalSum / facesUsing;

					//Normalize the normalSum vector
					normalSum = XMVector3Normalize(normalSum);

					//Store the normal and tangent in our current vertex
					subset.vertices[i].normal.x = -XMVectorGetX(normalSum);
					subset.vertices[i].normal.y = -XMVectorGetY(normalSum);
					subset.vertices[i].normal.z = -XMVectorGetZ(normalSum);

					//Clear normalSum, facesUsing for next vertex
					normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
					facesUsing = 0;
				}

				// Create index buffer
				D3D11_BUFFER_DESC indexBufferDesc;
				ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

				indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
				indexBufferDesc.ByteWidth = sizeof(DWORD) * subset.numTriangles * 3;
				indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				indexBufferDesc.CPUAccessFlags = 0;
				indexBufferDesc.MiscFlags = 0;

				D3D11_SUBRESOURCE_DATA iinitData;

				iinitData.pSysMem = &subset.indices[0];
				d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &subset.indexBuff);

				//Create Vertex Buffer
				D3D11_BUFFER_DESC vertexBufferDesc;
				ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

				vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;							// We will be updating this buffer, so we must set as dynamic
				vertexBufferDesc.ByteWidth = sizeof(Vertex) * subset.vertices.size();
				vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;				// Give CPU power to write to buffer
				vertexBufferDesc.MiscFlags = 0;

				D3D11_SUBRESOURCE_DATA vertexBufferData;

				ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
				vertexBufferData.pSysMem = &subset.vertices[0];
				hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &subset.vertBuff);

				// Push back the temp subset into the models subset vector
				MD5Model.subsets.push_back(subset);
			}
		}
	}
	else
	{
		//SwapChain->SetFullscreenState(false, NULL);	// Make sure we are out of fullscreen
		m_Direct3D->Shutdown();

		// create message
		std::wstring message = L"Could not open: ";
		message += filename;

		MessageBox(0, message.c_str(),	// display message
			L"Error", MB_OK);

		return false;
	}

	return true;
}
///////////////**************new**************////////////////////

bool GraphicsClass::LoadAlternativo(ID3D11Device* d3d11Device,
	ID3D11DeviceContext* d3d11DevCon, std::wstring filename) {

	HRESULT hResult;
	bool result;

	// Load the targa image data into memory.
	result = LoadTarga(filename, height, width);
	if (!result)
	{
		return false;
	}


	// Setup the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //32 bit textures
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.

	hResult = d3d11Device->CreateTexture2D(&textureDesc, NULL, &m_texture);

	if (FAILED(hResult))
	{
		return false;
	}

	// Set the row pitch of the targa image data.
	rowPitch = (width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	d3d11DevCon->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	return true;
}

bool GraphicsClass::LoadTarga(std::wstring filename, int& height, int& width)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;

	std::size_t found = filename.find(L".tga");

	if (found == std::string::npos) {
		filename = filename + L".tga";
	}

	// Open the targa file for reading in binary.
	error = _wfopen_s(&filePtr, filename.c_str(), L"rb");

	if (error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Get the important information from the header.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Allocate memory for the targa destination data.
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return false;
	}

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (width * height * 4) - (width * 4);
	//k = 0;

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		//k += (width * 4);
		k -= (width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = 0;

	return true;
}

void GraphicsClass::HandleMovementInput(InputClass* Input, float frameTime)
{
	bool keyDown;
	float posX, posY, posZ, rotX, rotY, rotZ;


	// Set the frame time for calculating the updated position.
	m_Position->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = Input->IsLeftPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = Input->IsRightPressed();
	m_Position->TurnRight(keyDown);

	keyDown = Input->IsUpPressed();
	m_Position->MoveForward(keyDown);

	keyDown = Input->IsDownPressed();
	m_Position->MoveBackward(keyDown);

	keyDown = Input->IsAPressed();
	m_Position->MoveUpward(keyDown);

	keyDown = Input->IsZPressed();
	m_Position->MoveDownward(keyDown);

	keyDown = Input->IsPgUpPressed();
	m_Position->LookUpward(keyDown);

	keyDown = Input->IsPgDownPressed();
	m_Position->LookDownward(keyDown);

	// Get the view point position/rotation.
	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	// Set the position of the camera.
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	return;
}