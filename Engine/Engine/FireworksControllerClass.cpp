#include "FireworksControllerClass.h"

D3DXVECTOR4 colorPalete[] =
{
	D3DXVECTOR4(0.8f, 0.0f, 0.8f, 1.0f),
	D3DXVECTOR4(0.8f, 0.8f, 0.0f, 1.0f),
	D3DXVECTOR4(0.0f, 0.8f, 0.8f, 1.0f),
	D3DXVECTOR4(0.8f, 0.8f, 0.8f, 1.0f),
	D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f),
	D3DXVECTOR4(0.0f, 0.9f, 0.7f, 1.0f),
	D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
	D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f),
	D3DXVECTOR4(0.9f, 0.5f, 0.0f, 1.0f)
};

FireworksControllerClass::FireworksControllerClass() :

	m_fireworks(NULL),
	m_activeFireworksCount(0),
	m_triggered(NULL),
	m_prevZ(0.0f),
	m_currentZ(0.0f),
	m_lastType(false)

{
}

FireworksControllerClass::FireworksControllerClass(const FireworksControllerClass& other)
{
}

FireworksControllerClass::~FireworksControllerClass()
{
}

bool FireworksControllerClass::Initialize(D3DClass* d3D, HWND hWnd, CameraClass* camera)
{
	m_d3D = d3D;
	m_hWnd = hWnd;

	m_prevZ = camera->GetPosition().z - 1.0f;
	m_currentZ = m_prevZ;

	m_fireworks = new FireworkClass*[MAX_FIREWORKS];

	if (!m_fireworks)
	{
		return false;
	}

	memset(m_fireworks, NULL, sizeof(FireworkClass*) * MAX_FIREWORKS);

	m_triggered = new bool[MAX_FIREWORKS];

	if (!m_triggered)
	{
		return false;
	}

	memset(m_triggered, false, sizeof(bool) * MAX_FIREWORKS);

	return true;
}

void FireworksControllerClass::Update(float frameTime, CameraClass* camera, SoundClass* sound)
{
	CheckForNulls(sound);
	TriggerNew(camera);
	UpdateTriggered(frameTime);
	RemoveFinished();
}

void FireworksControllerClass::Render(float screenDepth, D3DXMATRIX projection, CameraClass* camera, bool inTexture, float floorHeight)
{
	for (int i = 0; i < MAX_FIREWORKS; i++)
	{
		if (m_triggered[i])
		{
			m_fireworks[i]->Render(screenDepth, projection, camera, inTexture, floorHeight);
		}
	}
}

void FireworksControllerClass::Shutdown()
{
	for (int i = 0; i < MAX_FIREWORKS; i++)
	{
		if (m_fireworks[i])
		{
			m_fireworks[i]->Shutdown();
			delete m_fireworks[i];
			m_fireworks[i] = NULL;
		}
	}

	delete[] m_fireworks;
	delete[] m_triggered;
}

FireworkClass** FireworksControllerClass::GetFireworks()
{
	return m_fireworks;
}

void FireworksControllerClass::CheckForNulls(SoundClass* sound)
{
	bool result;
	int color;

	m_sound = sound;

	if (m_activeFireworksCount != MAX_FIREWORKS)
	{
		for (int i = 0; i < MAX_FIREWORKS; i++)
		{
			if (!m_fireworks[i])
			{
				m_prevZ += FIREWORKS_DISTANCE;
				
				m_fireworks[i] = new FireworkClass();

				if (!m_fireworks[i])
				{
					return;
				}

				float finX = (rand() % 170);
				finX /= 10.0f;

				float height = (rand() % 30) + 10;
				height /= 10.0f;

				float crtX = 2.5f;

				if (!m_lastType)
				{
					finX = -finX;
					crtX = -crtX;
				}

				color = rand() % (sizeof(colorPalete) / sizeof(D3DXVECTOR4));

				result = m_fireworks[i]->Initialize(D3DXVECTOR3(crtX, -1.0f, m_prevZ), D3DXVECTOR3(finX, height, m_prevZ), colorPalete[color], "../Engine/Data/sphere.txt", m_d3D, m_hWnd, sound);

				if (!result)
				{
					return;
				}

				m_activeFireworksCount++;
				m_lastType = !m_lastType;
			}
		}
	}
}

void FireworksControllerClass::TriggerNew(CameraClass* camera)
{
	D3DXVECTOR3 cameraPos;
	int minIx = -1;

	cameraPos = camera->GetPosition();

	if (cameraPos.z >= m_currentZ)
	{
		for (int i = 0; i < MAX_FIREWORKS; i++)
		{
			if(m_fireworks[i]->GetCurrentZ() > m_currentZ)
			{ 
				if (minIx == -1)
					minIx = i;
				else
				{
					if (m_fireworks[i]->GetCurrentZ() < m_fireworks[minIx]->GetCurrentZ())
					{
						minIx = i;
					}
				}
			}
		}

		m_sound->PlayLaunch(m_fireworks[minIx]->GetCrtX() / 5.0f);
		m_triggered[minIx] = true;
		
		m_currentZ = m_fireworks[minIx]->GetCurrentZ();
	}
}

void FireworksControllerClass::UpdateTriggered(float frameTime)
{
	if (frameTime >= 100.0f)
		frameTime = 100.0f;

	for (int i = 0; i < MAX_FIREWORKS; i++)
	{
		if (m_triggered[i])
		{
			m_fireworks[i]->Update(frameTime);
		}
	}
}

void FireworksControllerClass::RemoveFinished()
{
	for(int i=0;i<MAX_FIREWORKS;i++)
	{
		if (m_triggered[i])
		{
			if (m_fireworks[i]->Finished())
			{
				m_fireworks[i]->Shutdown();
				delete m_fireworks[i];
				m_fireworks[i] = NULL;

				m_triggered[i] = false;
				m_activeFireworksCount--;
			}
		}
	}
}
