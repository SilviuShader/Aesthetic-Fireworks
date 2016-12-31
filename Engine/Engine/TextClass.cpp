#include "TextClass.h"

TextClass::TextClass() :

	m_font(0),
	m_fontShader(0),

	m_sentence1(0),
	m_sentence2(0),
	m_sentence3(0)

{
}

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hWnd, int screenWidth, int screenHeight, D3DXMATRIX baseViewMatrix)
{
	bool result;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_baseViewMatrix = baseViewMatrix;

	m_font = new FontClass;

	if(!m_font)
	{
		return false;
	}

	result = m_font->Initialize(device, "../Engine/data/fontdata.txt", L"../Engine/data/font.dds");

	if(!result)
	{
		MessageBox(hWnd, L"Could not initialize the cont object.", L"Error", MB_OK);
		return false;
	}

	m_fontShader = new FontShaderClass();

	if(!m_fontShader)
	{
		return false;
	}

	result = m_fontShader->Initialize(device, hWnd);

	if(!result)
	{
		MessageBox(hWnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	result = InitializeSentence(&m_sentence1, 16, device);

	if(!result)
	{
		return false;
	}

	result = UpdateSentence(m_sentence1, "Hello", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext);

	if(!result)
	{
		return false;
	}

	result = InitializeSentence(&m_sentence2, 16, device);

	if(!result)
	{
		return false;
	}

	result = UpdateSentence(m_sentence2, "Goodbye", 100, 200, 1.0f, 1.0f, 0.0f, deviceContext);

	if(!result)
	{
		return false;
	}

	result = InitializeSentence(&m_sentence3, 16, device);

	if(!result)
	{
		return false;
	}

	result = UpdateSentence(m_sentence3, "Latency", 100, 300, 1.0f, 1.0f, 0.0f, deviceContext);

	if(!result)
	{
		return false;
	}

	return true;
}

void TextClass::Shutdown()
{
	ReleaseSentence(&m_sentence1);

	ReleaseSentence(&m_sentence2);

	ReleaseSentence(&m_sentence3);

	if(m_fontShader)
	{
		m_fontShader->Shutdown();
		delete m_fontShader;
		m_fontShader = 0;
	}

	if(m_font)
	{
		m_font->Shutdown();
		delete m_font;
		m_font = 0;
	}

	return;
}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	bool result;

	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);

	if(!result)
	{
		return false;
	}

	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);

	if(!result)
	{
		return false;
	}

	result = RenderSentence(deviceContext, m_sentence3, worldMatrix, orthoMatrix);

	if(!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char fpsString[16];
	float red, green, blue;
	bool result;

	if(fps > 9999)
	{
		fps = 9999;
	}

	_itoa_s(fps, tempString, 10);

	strcpy_s(fpsString, "FPS: ");
	strcat_s(fpsString, tempString);

	if(fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	if(fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	if(fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	result = UpdateSentence(m_sentence1, fpsString, 20, 20, red, green, blue, deviceContext);

	if(!result)
	{
		return false;
	}
}

bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char cpuString[16];
	bool result;

	_itoa_s(cpu, tempString, 10);

	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	result = UpdateSentence(m_sentence2, cpuString, 20, 40, 0.0f, 1.0f, 0.0f, deviceContext);

	if(!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetLatency(int latency, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char cpuString[16];
	bool result;

	_itoa_s(latency, tempString, 10);

	strcpy_s(cpuString, "Latency: ");
	strcat_s(cpuString, tempString);

	result = UpdateSentence(m_sentence3, cpuString, 20, 60, 0.0f, 1.0f, 0.0f, deviceContext);

	if(!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetRenderCount(int count, ID3D11DeviceContext* deviceContext)
{
	char tempString[32];
	char countString[32];
	bool result;

	_itoa_s(count, tempString, 10);

	strcpy_s(countString, "Render count: ");
	strcat_s(countString, tempString);

	result = UpdateSentence(m_sentence1, countString, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext);

	if(!result)
	{
		return false;
	}

	result = UpdateSentence(m_sentence2, "", 20, 20, 1.0f, 1.0f, 1.0f, deviceContext);

	if(!result)
	{
		return false;
	}

	return true;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;

	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;

	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;

	HRESULT result;

	*sentence = new SentenceType();

	if(!*sentence)
	{
		return false;
	}

	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	(*sentence)->maxLength = maxLength;

	(*sentence)->vertexCount = 6 * maxLength;

	(*sentence)->indexCount = (*sentence)->vertexCount;

	vertices = new VertexType[(*sentence)->vertexCount];

	if(!vertices)
	{
		return false;
	}

	indices = new unsigned long[(*sentence)->indexCount];

	if(!indices)
	{
		return false;
	}

	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	for(int i=0; i< (*sentence)->indexCount;i++)
	{
		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);

	if(FAILED(result))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);

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

bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	
	float drawX;
	float drawY;

	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	numLetters = (int)strlen(text);

	if(numLetters > sentence->maxLength)
	{
		return false;
	}

	vertices = new VertexType[sentence->vertexCount];

	if(!vertices)
	{
		return false;
	}

	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	m_font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if(FAILED(result))
	{
		return false;
	}

	verticesPtr = (VertexType*)mappedResource.pData;

	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	deviceContext->Unmap(sentence->vertexBuffer, 0);

	delete[] vertices;
	vertices = 0;

	return true;
}

void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if(*sentence)
	{
		if((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		if((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		delete *sentence;
		*sentence = 0;
	}

	return;
}

bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	D3DXVECTOR4 pixelColor;
	bool result;

	stride = sizeof(VertexType);

	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pixelColor = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);

	result = m_fontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_font->GetTexture(), pixelColor);

	if(!result)
	{
		return false;
	}

	return true;
}