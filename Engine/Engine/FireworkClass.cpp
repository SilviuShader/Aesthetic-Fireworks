#include "FireworkClass.h"

using namespace std;

FireworkClass::FireworkClass() :

	m_model(NULL),
	m_lightShader(NULL),
	m_light(NULL),

	m_positions(NULL),

	m_stage(LAUNCH_STAGE),
	m_finishedAnimation(false),
	m_heightForce(LAUNCH_FORCE),
	m_frustum(NULL)

{
}

FireworkClass::FireworkClass(const FireworkClass& other)
{
}

FireworkClass::~FireworkClass()
{
}

bool FireworkClass::Initialize(D3DXVECTOR3 startPoint, D3DXVECTOR3 explodePoint, D3DXVECTOR4 lightColor, const char* modelFilename, D3DClass* d3D, HWND hWnd, SoundClass* sound)
{
	bool result;

	m_sound = sound;

	m_startPoint = startPoint;
	m_finishPoint = explodePoint;

	m_direction = m_finishPoint - m_startPoint;
	D3DXVec3Normalize(&m_direction, &m_direction);

	m_d3D = d3D;
	
	m_model = new ModelClass();

	if (!m_model)
	{
		return false;
	}

	result = m_model->Initialize(m_d3D->GetDevice(), "../Engine/Data/square.txt", L"../Engine/Data/Spot.dds");

	if (!result)
	{
		return false;
	}

	m_lightShader = new LightShaderClass();

	if (!m_lightShader)
	{
		return false;
	}

	result = m_lightShader->Initialize(m_d3D->GetDevice(), hWnd);

	if (!result)
	{
		return false;
	}

	m_light = new LightClass();

	if (!m_light)
	{
		return false;
	}

	m_light->SetAmbientColor(lightColor.x / 3.0f, lightColor.y / 3.0f, lightColor.z / 3.0f, lightColor.w / 3.0f);
	m_light->SetDiffuseColor(lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	m_light->SetDirection(0.0f, 0.0f, 1.0f);
	m_light->SetSpecularColor(lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	m_light->SetSpecularPower(32.0f);

	result = LoadTXT(modelFilename);

	if (!result)
	{
		return false;
	}

	m_frustum = new FrustumClass();

	if (!m_frustum)
	{
		return false;
	}

	return true;
}

void FireworkClass::Shutdown()
{
	if (m_frustum)
	{
		delete m_frustum;
		m_frustum = NULL;
	}

	if (m_model)
	{
		m_model->Shutdown();
		delete m_model;
		m_model = NULL;
	}

	if (m_lightShader)
	{
		m_lightShader->Shutdown();
		delete m_lightShader;
		m_lightShader = NULL;
	}

	if (m_light)
	{
		delete m_light;
		m_light = NULL;
	}

	if (m_positions)
	{
		delete[] m_positions;
		m_positions = NULL;
	}
}

void FireworkClass::Update(float frameTime)
{
	if (!m_finishedAnimation)
	{
		GravityUpdate(frameTime);

		switch (m_stage)
		{
		case LAUNCH_STAGE:
			LaunchUpdate(frameTime);
			break;
		case EXPLODE_STAGE:
			ExplodeUpdate(frameTime);
			CheckFinished();
			break;
		}
	}
}

void FireworkClass::Render(float screenDepth, D3DXMATRIX projection, CameraClass* camera, bool inTexture, float floorHeight)
{
	bool shine;

	if (!m_finishedAnimation)
	{
		D3DXMATRIX view;
		D3DXMATRIX translation;
		D3DXMATRIX scale;
		D3DXMATRIX rotation;
		D3DXMATRIX world;
		int counts;

		if (!inTexture)
		{
			camera->GetViewMatrix(view);
		}
		else
		{
			camera->RenderReflection(floorHeight);
			view = camera->GetReflectionViewMatrix();
		}

		m_frustum->ConstructFrustrum(screenDepth, projection, view);

		m_d3D->TurnOnAlphaBlending();

		switch (m_stage)
		{
		case LAUNCH_STAGE:
			D3DXMatrixScaling(&scale, PARTICLE_LAUNCH_SCALE, PARTICLE_LAUNCH_SCALE, PARTICLE_LAUNCH_SCALE);
			counts = 1;
			break;
		case EXPLODE_STAGE:
			counts = m_positionsCount;
			D3DXMatrixScaling(&scale, PARTICLE_SCALE, PARTICLE_SCALE, PARTICLE_SCALE);
			break;
		}

		for (int i = 0; i < counts; i++)
		{
			if (m_frustum->CheckPoint(m_positions[i].x, m_positions[i].y, m_positions[i].z))
			{
				shine = rand() % 2;

				if (!shine)
				{
					D3DXMatrixTranslation(&translation, m_positions[i].x, m_positions[i].y, m_positions[i].z);
					D3DXMatrixRotationY(&rotation, BilboardClass::GetRotation(camera->GetPosition(), D3DXVECTOR3(m_positions[i].x, m_positions[i].y, m_positions[i].z)));

					D3DXMatrixMultiply(&world, &scale, &translation);
					D3DXMatrixMultiply(&world, &rotation, &world);

					m_model->Render(m_d3D->GetDeviceContext());
					m_lightShader->Render(m_d3D->GetDeviceContext(), m_model->GetIndexCount(), world, view, projection, m_model->GetTexture(), m_light->GetDirection(), m_light->GetAmbientColor(), m_light->GetDiffuseColor(), camera->GetPosition(), m_light->GetSpecularColor(), m_light->GetSpecularPower());
				}
			}
		}

		m_d3D->TurnOffAlphaBlending();
	}
}

float FireworkClass::GetCurrentZ()
{
	return m_finishPoint.z;
}

float FireworkClass::GetCrtX()
{
	return m_startPoint.x;
}

bool FireworkClass::Finished()
{
	return m_finishedAnimation;
}

D3DXVECTOR3 FireworkClass::GetPosition()
{
	return m_finishPoint;
}

LightClass* FireworkClass::GetLight()
{
	return m_light;
}

void FireworkClass::GravityUpdate(float frameTime)
{
	m_heightForce -= (frameTime * GRAVITY_MULTIPLIER);
}

void FireworkClass::LaunchUpdate(float frameTime)
{
	for (int i = 0; i < m_positionsCount; i++)
	{
		m_positions[i].accumulatedHeight -= (frameTime * GRAVITY_MULTIPLIER);

		m_positions[i].x += (XZ_FORCE * m_direction.x * frameTime) * SPEED_MULTIPLIER;
		m_positions[i].y += (m_heightForce * m_direction.y * frameTime) * SPEED_MULTIPLIER;
		m_positions[i].z += (XZ_FORCE * m_direction.z * frameTime) * SPEED_MULTIPLIER;
	}

	if (m_positions[0].y >= m_finishPoint.y)
	{
		m_stage = EXPLODE_STAGE;
		m_sound->PlayWaveFile(m_finishPoint.x/5.0f);
	}
}

void FireworkClass::CheckFinished()
{
	bool allFinished = true;

	for (int i = 0; i < m_positionsCount; i++)
	{
		if (m_positions[i].y >= m_startPoint.y)
		{
			allFinished = false;
			break;
		}
	}

	m_finishedAnimation = allFinished;
}

void FireworkClass::ExplodeUpdate(float frameTime)
{
	for (int i = 0; i < m_positionsCount; i++)
	{
		m_positions[i].accumulatedHeight -= (frameTime * GRAVITY_MULTIPLIER);

		m_positions[i].x += (XZ_FORCE * m_positions[i].dx * frameTime) * SPEED_MULTIPLIER;
		m_positions[i].y += (m_positions[i].accumulatedHeight * frameTime) * SPEED_MULTIPLIER;
		m_positions[i].z += (XZ_FORCE * m_positions[i].dz * frameTime) * SPEED_MULTIPLIER;
	}
}

bool FireworkClass::LoadTXT(const char* filename)
{
	ifstream fin;
	char input;
	int i;

	float junk;

	fin.open(filename);

	if (fin.fail())
	{
		return false;
	}

	fin.get(input);

	while (input != ':')
	{
		fin.get(input);
	}

	fin >> m_positionsCount;

	m_positions = new PositionType[m_positionsCount];

	if (!m_positions)
	{
		return false;
	}

	fin.get(input);

	while (input != ':')
	{
		fin.get(input);
	}

	fin.get(input);
	fin.get(input);

	for (int i = 0; i<m_positionsCount; i++)
	{
		
		fin >> junk >> junk >> junk;
		fin >> junk >> junk;
		fin >> m_positions[i].dx >> m_positions[i].dy >> m_positions[i].dz;

		m_positions[i].x = m_startPoint.x;
		m_positions[i].y = m_startPoint.y;
		m_positions[i].z = m_startPoint.z;

		m_positions[i].accumulatedHeight = m_positions[i].dy * XZ_FORCE;
	}

	fin.close();

	return true;
}
