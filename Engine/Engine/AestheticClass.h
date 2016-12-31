#pragma once

#include "D3DClass.h"

#include "BitmapClass.h"
#include "RenderTextureClass.h"
#include "TextureShaderClass.h"
#include "FireworksControllerClass.h"
#include "FloorClass.h"
#include "SoundClass.h"

#include "CameraClass.h"

class AestheticClass
{
public:

	AestheticClass();
	AestheticClass(const AestheticClass&);
	~AestheticClass();

	bool Initialize(int, int, D3DClass*, HWND, CameraClass*, SoundClass*, bool);
	void Shutdown();
	void Update(float, CameraClass*);
	bool Render(float, D3DXMATRIX, D3DXMATRIX, CameraClass*);

private:

	void UpdateCamera(float, CameraClass*);

	void DrawNormalScene(float, D3DXMATRIX, D3DXMATRIX, CameraClass*);
	void DrawAestheticScene(float, D3DXMATRIX, D3DXMATRIX, CameraClass*);
	void RenderInFloor(float, D3DXMATRIX, D3DXMATRIX, CameraClass*);

private:

	int m_screenWidth;
	int m_screenHeight;

	bool m_fullAesthetic;

	D3DClass* m_d3D;
	SoundClass* m_sound;

	FireworksControllerClass* m_fireworksController;
	FloorClass* m_floor;

	BitmapClass* m_backgroundBitmap;
	RenderTextureClass* m_backgroundTexture;
	RenderTextureClass* m_floorTexture;

	TextureShaderClass* m_textureShader;

	D3DXMATRIX m_identityMatrix;
	D3DXMATRIX m_baseViewMatrix;

	bool m_gotBaseViewMatrix;
};