#include "FloorClass.h"

FloorClass::FloorClass() :

	m_height(FLOOR_HEIGHT),
	m_model(NULL),
	m_shader(NULL),
	m_cylinder(NULL),
	m_lightShader(NULL)

{
}

FloorClass::FloorClass(const FloorClass& other)
{
}

FloorClass::~FloorClass()
{
}

bool FloorClass::Initialize(D3DClass* d3D, HWND hWnd)
{
	bool result;

	m_d3D = d3D;

	m_model = new ModelClass();

	if (!m_model)
	{
		return false;
	}

	result = m_model->Initialize(m_d3D->GetDevice(), "../Engine/Data/floor.txt", L"../Engine/Data/blue01.dds");

	if (!result)
	{
		return false;
	}

	m_shader = new ReflectionShaderClass();

	if (!m_shader)
	{
		return false;
	}

	result = m_shader->Initialize(m_d3D->GetDevice(), hWnd);

	if (!result)
	{
		return false;
	}

	m_cylinder = new ModelClass();

	if (!m_cylinder)
	{
		return false;
	}

	result = m_cylinder->Initialize(m_d3D->GetDevice(), "../Engine/Data/cylinder.txt", L"../Engine/Data/seafloor.dds");

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

	return true;
}

void FloorClass::Update(CameraClass* camera)
{
	D3DXVECTOR3 cameraPos = camera->GetPosition();
	m_position = cameraPos;
	m_position.y = m_height;
}

void FloorClass::Render(CameraClass* camera, ID3D11ShaderResourceView* reflectionTexture, D3DXMATRIX projection, FireworkClass** fireworks, int fireworksCount)
{
	D3DXMATRIX translation;
	D3DXMATRIX scale;
	D3DXMATRIX world;
	D3DXMATRIX view;

	D3DXMatrixTranslation(&translation, m_position.x, m_position.y, m_position.z);
	D3DXMatrixScaling(&scale, SCALE_X, SCALE_Y, SCALE_Z);

	D3DXMatrixMultiply(&world, &scale, &translation);

	camera->GetViewMatrix(view);

	m_model->Render(m_d3D->GetDeviceContext());
	m_shader->Render(m_d3D->GetDeviceContext(), m_model->GetIndexCount(), world, view, projection, m_model->GetTexture(), reflectionTexture, camera->GetReflectionViewMatrix());

	RenderCylinders(camera, fireworks, fireworksCount, projection);
}

void FloorClass::Shutdown()
{
	if (m_model)
	{
		m_model->Shutdown();
		delete m_model;
		m_model = NULL;
	}

	if (m_cylinder)
	{
		m_cylinder->Shutdown();
		delete m_cylinder;
		m_cylinder = NULL;
	}

	if (m_shader)
	{
		m_shader->Shutdown();
		delete m_shader;
		m_shader = NULL;
	}

	if (m_lightShader)
	{
		m_lightShader->Shutdown();
		delete m_lightShader;
		m_lightShader = NULL;
	}
}

float FloorClass::GetHeight()
{
	return m_height;
}

void FloorClass::RenderCylinders(CameraClass* camera, FireworkClass** fireworks, int fireworksCount, D3DXMATRIX projection)
{
	D3DXMATRIX translation;
	D3DXMATRIX view;
	D3DXMATRIX world;
	LightClass* light;

	float crtX;

	D3DXMATRIX scale;

	camera->GetViewMatrix(view);

	D3DXMatrixScaling(&scale, 0.3f, 3.0f, 0.3f);

	for (int i = 0; i < fireworksCount; i++)
	{
		if (fireworks[i])
		{
			crtX = fireworks[i]->GetCrtX();

			if (crtX >= 0)
				crtX -= 0.7f;
			else
				crtX += 0.7f;

			light = fireworks[i]->GetLight();
			D3DXMatrixTranslation(&translation, crtX, m_height - 1.0f - 4.0f, fireworks[i]->GetPosition().z);
			D3DXMatrixMultiply(&world, &scale, &translation);
			m_cylinder->Render(m_d3D->GetDeviceContext());

			m_lightShader->Render(m_d3D->GetDeviceContext(), m_cylinder->GetIndexCount(), world, view, projection, m_cylinder->GetTexture(), light->GetDirection(), light->GetAmbientColor(), light->GetDiffuseColor(), camera->GetPosition(), light->GetSpecularColor(), light->GetSpecularPower());
		}
	}
}


