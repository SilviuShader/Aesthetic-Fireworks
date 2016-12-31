#pragma once

#include "LightShaderClass.h"

using namespace std;

LightShaderClass::LightShaderClass() :

	m_vertexShader(0),
	m_pixelShader(0),
	m_layout(0),
	m_matrixBuffer(0),
	m_sampleState(0),
	m_lightBuffer(0),
	m_cameraBuffer(0),

	m_lightColorBuffer(0),
	m_lightPositionBuffer(0)

{
}

LightShaderClass::LightShaderClass(const LightShaderClass& other)
{
}

LightShaderClass::~LightShaderClass()
{
}

bool LightShaderClass::Initialize(ID3D11Device* device, HWND hWnd)
{
	bool result;

	result = InitializeShader(device, hWnd, L"../Engine/Light.vs", L"../Engine/Light.ps");

	if(!result)
	{
		return false;
	}

	return true;
}

void LightShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor ,diffuseColor, cameraPosition, specularColor, specularPower);

	if(!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}


bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower, D3DXVECTOR4 pointLightColors[], D3DXVECTOR4 lightPositions[])
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor ,diffuseColor, cameraPosition, specularColor, specularPower, pointLightColors, lightPositions);

	if(!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hWnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;

	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];

	unsigned int numElements;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC lightColorBufferDesc;
	D3D11_BUFFER_DESC lightPositionBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);

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

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);

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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

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

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);

	if(FAILED(result))
	{
		return true;
	}

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);

	if(FAILED(result))
	{
		return false;
	}

	lightColorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightColorBufferDesc.ByteWidth = sizeof(LightColorBufferType);
	lightColorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightColorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightColorBufferDesc.MiscFlags = 0;
	lightColorBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightColorBufferDesc, NULL, &m_lightColorBuffer);

	if(FAILED(result))
	{
		return false;
	}

	lightPositionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightPositionBufferDesc.ByteWidth = sizeof(LightPositionBufferType);
	lightPositionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightPositionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightPositionBufferDesc.MiscFlags = 0;
	lightPositionBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightPositionBufferDesc, NULL, &m_lightPositionBuffer);

	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void LightShaderClass::ShutdownShader()
{
	if(m_lightColorBuffer)
	{
		m_lightColorBuffer->Release();
		m_lightColorBuffer = 0;
	}

	if(m_lightPositionBuffer)
	{
		m_lightPositionBuffer->Release();
		m_lightPositionBuffer = 0;
	}

	if(m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	if(m_cameraBuffer)
	{
		m_cameraBuffer->Release();
		m_cameraBuffer = 0;
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

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hWnd, WCHAR* shaderFilename)
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

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	CameraBufferType* dataPtr3;

	LightPositionBufferType* dataPtr4;
	LightColorBufferType* dataPtr5;

	unsigned int bufferNumber;

	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

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

	result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if(FAILED(result))
	{
		return false;
	}
	
	dataPtr3 = (CameraBufferType*)mappedResource.pData;

	dataPtr3->cameraPosition = cameraPosition;
	dataPtr3->padding = 0.0f;

	deviceContext->Unmap(m_cameraBuffer, 0);

	bufferNumber = 1;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1,&m_cameraBuffer);

	deviceContext->PSSetShaderResources(0, 1,&texture);

	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if(FAILED(result))
	{
		return false;
	}

	dataPtr2 = (LightBufferType*)mappedResource.pData;

	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->specularColor = specularColor;
	dataPtr2->specularPower = specularPower;

	deviceContext->Unmap(m_lightBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	result = deviceContext->Map(m_lightPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	dataPtr4 = (LightPositionBufferType*)mappedResource.pData;

	dataPtr4->lightPosition[0] = D3DXVECTOR4(0, 0, 0, 0);
	dataPtr4->lightPosition[1] = D3DXVECTOR4(0, 0, 0, 0);
	dataPtr4->lightPosition[2] = D3DXVECTOR4(0, 0, 0, 0);
	dataPtr4->lightPosition[3] = D3DXVECTOR4(0, 0, 0, 0);

	deviceContext->Unmap(m_lightPositionBuffer, 0);

	bufferNumber = 2;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_lightPositionBuffer);

	result = deviceContext->Map(m_lightColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	dataPtr5 = (LightColorBufferType*)mappedResource.pData;

	dataPtr5->diffuseColor[0] = D3DXVECTOR4(0, 0, 0, 0);
	dataPtr5->diffuseColor[1] = D3DXVECTOR4(0, 0, 0, 0);
	dataPtr5->diffuseColor[2] = D3DXVECTOR4(0, 0, 0, 0);
	dataPtr5->diffuseColor[3] = D3DXVECTOR4(0, 0, 0, 0);

	deviceContext->Unmap(m_lightColorBuffer, 0);

	bufferNumber = 1;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightColorBuffer);

	return true;
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower, D3DXVECTOR4 pointLightColors[], D3DXVECTOR4 lightPositions[])
{
	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	CameraBufferType* dataPtr3;

	LightPositionBufferType* dataPtr4;
	LightColorBufferType* dataPtr5;

	unsigned int bufferNumber;

	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

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

	result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if(FAILED(result))
	{
		return false;
	}
	
	dataPtr3 = (CameraBufferType*)mappedResource.pData;

	dataPtr3->cameraPosition = cameraPosition;
	dataPtr3->padding = 0.0f;

	deviceContext->Unmap(m_cameraBuffer, 0);

	bufferNumber = 1;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1,&m_cameraBuffer);

	deviceContext->PSSetShaderResources(0, 1,&texture);

	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if(FAILED(result))
	{
		return false;
	}

	dataPtr2 = (LightBufferType*)mappedResource.pData;

	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->specularColor = specularColor;
	dataPtr2->specularPower = specularPower;

	deviceContext->Unmap(m_lightBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	result = deviceContext->Map(m_lightPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0 , &mappedResource);

	if(FAILED(result))
	{
		return false;
	}

	dataPtr4 = (LightPositionBufferType*)mappedResource.pData;

	dataPtr4->lightPosition[0] = lightPositions[0];
	dataPtr4->lightPosition[1] = lightPositions[1];
	dataPtr4->lightPosition[2] = lightPositions[2];
	dataPtr4->lightPosition[3] = lightPositions[3];

	deviceContext->Unmap(m_lightPositionBuffer, 0);

	bufferNumber = 2;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_lightPositionBuffer);

	result = deviceContext->Map(m_lightColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if(FAILED(result))
	{
		return false;
	}

	dataPtr5 = (LightColorBufferType*)mappedResource.pData;

	dataPtr5->diffuseColor[0] = pointLightColors[0];
	dataPtr5->diffuseColor[1] = pointLightColors[1];
	dataPtr5->diffuseColor[2] = pointLightColors[2];
	dataPtr5->diffuseColor[3] = pointLightColors[3];

	deviceContext->Unmap(m_lightColorBuffer, 0);

	bufferNumber = 1;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightColorBuffer);

	return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}