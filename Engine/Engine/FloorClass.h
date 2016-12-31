#pragma once

#include "D3DClass.h"
#include "ModelClass.h"
#include "ReflectionShaderClass.h"
#include "CameraClass.h"
#include "LightShaderClass.h"
#include "LightClass.h"
#include "FireworkClass.h"

const float SCALE_X = 0.2f;
const float SCALE_Y = 1.0f;
const float SCALE_Z = 30.0f;

const float FLOOR_HEIGHT = -2.0f;

class FloorClass
{
public:

	FloorClass();
	FloorClass(const FloorClass&);
	~FloorClass();

	bool Initialize(D3DClass*, HWND);
	void Update(CameraClass*);
	void Render(CameraClass*, ID3D11ShaderResourceView*, D3DXMATRIX, FireworkClass**, int);
	void Shutdown();

	float GetHeight();

private:

	void RenderCylinders(CameraClass* ,FireworkClass**, int, D3DXMATRIX);

private:

	D3DClass* m_d3D;
	float m_height;

	ModelClass* m_model;
	ModelClass* m_cylinder;

	ReflectionShaderClass* m_shader;
	LightShaderClass* m_lightShader;

	D3DXVECTOR3 m_position;
};