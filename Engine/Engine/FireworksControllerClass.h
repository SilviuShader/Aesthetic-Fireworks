#pragma once

const int MAX_FIREWORKS = 10;
const float FIREWORKS_DISTANCE = 20.0f;

#include "D3DClass.h"
#include "FireworkClass.h"
#include "SoundClass.h"

class FireworksControllerClass
{
public:

	FireworksControllerClass();
	FireworksControllerClass(const FireworksControllerClass&);
	~FireworksControllerClass();

	bool Initialize(D3DClass*, HWND, CameraClass*);
	void Update(float, CameraClass*, SoundClass*);
	void Render(float, D3DXMATRIX, CameraClass*, bool, float);
	void Shutdown();

	FireworkClass** GetFireworks();

private:

	void CheckForNulls(SoundClass*);
	void TriggerNew(CameraClass*);
	void UpdateTriggered(float);
	void RemoveFinished();

private:

	D3DClass* m_d3D;
	SoundClass* m_sound;
	HWND m_hWnd;

	FireworkClass** m_fireworks;
	bool* m_triggered;
	int m_activeFireworksCount;

	float m_prevZ;
	bool m_lastType;
	float m_currentZ;
};