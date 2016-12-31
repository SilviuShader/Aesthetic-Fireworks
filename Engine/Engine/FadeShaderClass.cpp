#pragma once

#include "FadeShaderClass.h"

using namespace std;

FadeShaderClass::FadeShaderClass() :

	m_vertexShader(0),
	m_pixelShader(0),
	m_layout(0),
	m_matrixBuffer(0),
	m_sampleState(0),

	m_fadeBuffer(0)

{
}

FadeShaderClass::FadeShaderClass(const FadeShaderClass& other)
{
}

FadeShaderClass::~FadeShaderClass()
{
}

bool FadeShaderClass::Initialize(ID3D11Device* device, HWND hWnd)
{
	bool result;

	result = InitializeShader(device, hWnd, L"../Engine/Fade.vs", L"../Engine/Fade.ps");

	if(!result)
	{
		return false;
	}

	return true;
}

void FadeShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}

bool FadeShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float fadeAmount)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, fadeAmount);

	if(!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool FadeShaderClass::InitializeShader(ID3D11Device* device, HWND hWnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;

	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	unsigned int numElements;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC fadeBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "FadeVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);

	if(FAILED(result))
	{
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hWnd, vsFilename);
		}
		else
		{
			MessageBox(hWnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "FadePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);

	if(FAILED(result))
	{
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hWnd, psFilename);
		}
		else
		{
			MessageBox(hWnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);

	if(FAILED(result))
	{
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);

	if(FAILED(result))
	{
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);

	if(FAILED(result))
	{
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);

	if(FAILED(result))
	{
		return false;
	}

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;

	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);

	if(FAILED(result))
	{
		return false;
	}

	fadeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	fadeBufferDesc.ByteWidth = sizeof(FadeBufferType);
	fadeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fadeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fadeBufferDesc.MiscFlags = 0;
	fadeBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&fadeBufferDesc, NULL, &m_fadeBuffer);

	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void FadeShaderClass::ShutdownShader()
{
	if(m_fadeBuffer)
	{
		m_fadeBuffer->Release();
		m_fadeBuffer = 0;
	}

	if(m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	if(m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if(m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void FadeShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hWnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize;
	ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for(int i=0; i<bufferSize;i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hWnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool FadeShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float fadeAmount)
{
	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	FadeBufferType* dataPtr2;

	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0,&mappedResource);

	if(FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	deviceContext->PSSetShaderResources(0, 1,&texture);

	result = deviceContext->Map(m_fadeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if(FAILED(result))
	{
		return false;
	}

	dataPtr2 = (FadeBufferType*)mappedResource.pData;

	dataPtr2->fadeAmount = fadeAmount;
	dataPtr2->padding = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	deviceContext->Unmap(m_fadeBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_fadeBuffer);

	return true;
}

void FadeShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}