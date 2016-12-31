#pragma once

#include <d3dx10math.h>

class CameraClass
{
public:

	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX&);

	void RenderReflection(float);
	D3DXMATRIX GetReflectionViewMatrix();

private:

	float m_positionX;
	float m_positionY;
	float m_positionZ;

	float m_rotationX;
	float m_rotationY;
	float m_rotationZ;

	D3DXMATRIX m_viewMatrix;

	D3DXMATRIX m_reflectionViewMatrix;
};