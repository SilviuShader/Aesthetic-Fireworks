#pragma once

#include <fstream>

#include "D3DClass.h"

#include "CameraClass.h"
#include "ModelClass.h"
#include "LightShaderClass.h"
#include "LightClass.h"
#include "BilboardClass.h"
#include "SoundClass.h"

#include "FrustumClass.h"

const float LAUNCH_FORCE = 90.0f;
const float XZ_FORCE = 35.0f;
const float GRAVITY_MULTIPLIER = 0.0098f;
const float SPEED_MULTIPLIER = 0.00015f;
const float PARTICLE_SCALE = 0.014f;
const float PARTICLE_LAUNCH_SCALE = 0.1f;

class FireworkClass
{
private:

	struct PositionType
	{
		float x, y, z;
		float dx, dy, dz;

		float accumulatedHeight;
	};

	enum FireworkStage
	{
		LAUNCH_STAGE = 0,
		EXPLODE_STAGE = 1
	};

public:

	FireworkClass();
	FireworkClass(const FireworkClass&);
	~FireworkClass();

	bool Initialize(D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR4, const char*, D3DClass*, HWND, SoundClass*);
	void Shutdown();
	void Update(float);
	void Render(float, D3DXMATRIX, CameraClass*, bool, float);

	float GetCurrentZ();
	float GetCrtX();

	bool Finished();

	D3DXVECTOR3 GetPosition();
	LightClass* GetLight();

private:

	void GravityUpdate(float);
	void LaunchUpdate(float);
	void CheckFinished();
	void ExplodeUpdate(float);
	bool LoadTXT(const char*);

private:

	D3DXVECTOR3 m_startPoint;
	D3DXVECTOR3 m_finishPoint;
	D3DXVECTOR3 m_direction;

	float m_heightForce;

	D3DClass* m_d3D;

	ModelClass* m_model;
	LightShaderClass* m_lightShader;
	LightClass* m_light;

	PositionType* m_positions;

	FrustumClass* m_frustum;

	SoundClass* m_sound;

	int m_positionsCount;

	FireworkStage m_stage;

	bool m_finishedAnimation;
};