#include "ModelClass.h"

using namespace std;

ModelClass::ModelClass() :

	m_vertexBuffer(0),
	m_indexBuffer(0),
	m_model(0),

	m_textureArray(0),
	m_texture(0)

{
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename1, WCHAR* textureFilename2, WCHAR* filename3)
{
	bool result;

	result = LoadModel(modelFilename);

	if(!result)
	{
		return false;
	}

	CalculateModelVectors();

	result = InitializeBuffers(device);

	if(!result)
	{
		return false;
	}

	result = LoadTextures(device, textureFilename1, textureFilename2, filename3);

	if(!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename1, WCHAR* textureFilename2)
{
	bool result;

	result = LoadModel(modelFilename);

	if(!result)
	{
		return false;
	}

	CalculateModelVectors();

	result = InitializeBuffers(device);

	if(!result)
	{
		return false;
	}

	result = LoadTextures(device, textureFilename1, textureFilename2);

	if(!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename)
{
	bool result;

	result = LoadModel(modelFilename);

	if(!result)
	{
		return false;
	}

	CalculateModelVectors();

	result = InitializeBuffers(device);

	if(!result)
	{
		return false;
	}

	result = LoadTexture(device, textureFilename);

	if(!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename)
{
	bool result;

	result = LoadModel(modelFilename);

	if(!result)
	{
		return false;
	}

	CalculateModelVectors();

	result = InitializeBuffers(device);

	if(!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	ReleaseTextures();
	ReleaseTexture();

	ShutdownBuffers();

	ReleaseModel();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView** ModelClass::GetTextureArray()
{
	return m_textureArray->GetTextureArray();
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	vertices = new VertexType[m_vertexCount];

	if(!vertices)
	{
		return false;
	}

	indices = new unsigned long[m_indexCount];

	if(!indices)
	{
		return false;
	}

	for(int i=0;i<m_vertexCount;i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		vertices[i].tangent = D3DXVECTOR3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
		vertices[i].binormal = D3DXVECTOR3(m_model[i].bx, m_model[i].by, m_model[i].bz);

		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	if(FAILED(result))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);

	if(FAILED(result))
	{
		return false;
	}

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTextures(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2, WCHAR* filename3)
{
	bool result;

	m_textureArray = new TextureArrayClass();

	if(!m_textureArray)
	{
		return false;
	}

	result = m_textureArray->Initialize(device, filename1, filename2, filename3);

	if(!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::LoadTextures(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2)
{
	bool result;

	m_textureArray = new TextureArrayClass();

	if(!m_textureArray)
	{
		return false;
	}

	result = m_textureArray->Initialize(device, filename1, filename2);

	if(!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTextures()
{
	if(m_textureArray)
	{
		m_textureArray->Shutdown();
		delete m_textureArray;
		m_textureArray = 0;
	}
}

bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	m_texture = new TextureClass();

	if(!m_texture)
	{
		return false;
	}

	result = m_texture->Initialize(device, filename);

	if(!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	if(m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
}

bool ModelClass::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;

	fin.open(filename);

	if(fin.fail())
	{
		return false;
	}

	fin.get(input);

	while(input != ':')
	{
		fin.get(input);
	}

	fin >> m_vertexCount;

	m_indexCount = m_vertexCount;

	m_model = new ModelType[m_vertexCount];

	if(!m_model)
	{
		return false;
	}

	fin.get(input);

	while(input != ':')
	{
		fin.get(input);
	}

	fin.get(input);
	fin.get(input);

	for(int i=0;i<m_vertexCount;i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	fin.close();

	return true;
}

void ModelClass::ReleaseModel()
{
	if(m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}

void ModelClass::CalculateModelVectors()
{
	int faceCount, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;

	faceCount = m_vertexCount / 3;

	index = 0;

	for(int i=0;i<faceCount;i++)
	{
		vertex1.x = m_model[index].x;
		vertex1.y = m_model[index].y;
		vertex1.z = m_model[index].z;
		
		vertex1.tu = m_model[index].tu;
		vertex1.tv = m_model[index].tv;

		vertex1.nx = m_model[index].nx;
		vertex1.ny = m_model[index].ny;
		vertex1.nz = m_model[index].nz;

		index++;

		
		vertex2.x = m_model[index].x;
		vertex2.y = m_model[index].y;
		vertex2.z = m_model[index].z;
		
		vertex2.tu = m_model[index].tu;
		vertex2.tv = m_model[index].tv;

		vertex2.nx = m_model[index].nx;
		vertex2.ny = m_model[index].ny;
		vertex2.nz = m_model[index].nz;

		index++;

		
		vertex3.x = m_model[index].x;
		vertex3.y = m_model[index].y;
		vertex3.z = m_model[index].z;
		
		vertex3.tu = m_model[index].tu;
		vertex3.tv = m_model[index].tv;

		vertex3.nx = m_model[index].nx;
		vertex3.ny = m_model[index].ny;
		vertex3.nz = m_model[index].nz;

		index++;

		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		CalculateNormal(tangent, binormal, normal);

		m_model[index-1].nx = normal.x;
		m_model[index-1].ny = normal.y;
		m_model[index-1].nz = normal.z;

		m_model[index-1].tx = tangent.x;
		m_model[index-1].ty = tangent.y;
		m_model[index-1].tz = tangent.z;

		m_model[index-1].bx = binormal.x;
		m_model[index-1].by = binormal.y;
		m_model[index-1].bz = binormal.z;


		m_model[index-2].nx = normal.x;
		m_model[index-2].ny = normal.y;
		m_model[index-2].nz = normal.z;

		m_model[index-2].tx = tangent.x;
		m_model[index-2].ty = tangent.y;
		m_model[index-2].tz = tangent.z;

		m_model[index-2].bx = binormal.x;
		m_model[index-2].by = binormal.y;
		m_model[index-2].bz = binormal.z;


		m_model[index-3].nx = normal.x;
		m_model[index-3].ny = normal.y;
		m_model[index-3].nz = normal.z;

		m_model[index-3].tx = tangent.x;
		m_model[index-3].ty = tangent.y;
		m_model[index-3].tz = tangent.z;

		m_model[index-3].bx = binormal.x;
		m_model[index-3].by = binormal.y;
		m_model[index-3].bz = binormal.z;
	}
}

void ModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;

	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}

void ModelClass::CalculateNormal(VectorType tangent, VectorType binormal, VectorType& normal)
{
	float length;

	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
}