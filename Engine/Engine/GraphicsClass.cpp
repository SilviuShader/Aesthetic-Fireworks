#include "GraphicsClass.h"

GraphicsClass::GraphicsClass() :

	m_d3D(0),
	m_camera(0),

	m_text(0),

	m_aesthetic(NULL)
{
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{	
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hWnd, SoundClass* sound, bool fullScreen, bool fullAesthetic)
{
	bool result;
	D3DXMATRIX baseViewMatrix;
	FULL_SCREEN = fullScreen;
	srand(time(NULL));

	m_d3D = new D3DClass();

	if(!m_d3D)
	{
		return false;
	}

	result = m_d3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hWnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);

	if(!result)
	{
		MessageBox(hWnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	m_camera = new CameraClass();

	if(!m_camera)
	{
		return false;
	}
	
	m_camera->SetPosition(0.0f, 0.0f, -1.0f);

	m_camera->Render();

	m_camera->GetViewMatrix(baseViewMatrix);

	m_text = new TextClass();

	if(!m_text)
	{
		return false;
	}

	result = m_text->Initialize(m_d3D->GetDevice(), m_d3D->GetDeviceContext(), hWnd, screenWidth, screenHeight, baseViewMatrix);
	
	if(!result)
	{
		MessageBox(hWnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	m_camera->SetPosition(0.0f, 0.0f, -20.0f);

	m_aesthetic = new AestheticClass();

	if (!m_aesthetic)
	{
		return false;
	}

	result = m_aesthetic->Initialize(screenWidth, screenHeight, m_d3D, hWnd, m_camera, sound, fullAesthetic);

	if (!result)
	{
		MessageBox(hWnd, L"Could not initialize the aesthetic object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if (m_aesthetic)
	{
		m_aesthetic->Shutdown();
		delete m_aesthetic;
		m_aesthetic = 0;
	}

	if(m_text)
	{
		m_text->Shutdown();
		delete m_text;
		m_text = 0;
	}

	if(m_camera)
	{
		delete m_camera;
		m_camera = 0;
	}

	if(m_d3D)
	{
		m_d3D->Shutdown();
		delete m_d3D;
		m_d3D = 0;
	}

	return;
}

bool GraphicsClass::Frame(float frameTime)
{
	bool result;

	m_aesthetic->Update(frameTime, m_camera);

	result = Render();

	if(!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, projectionMatrix, viewMatrix;
	D3DXMATRIX orthoMatrix;
	bool result;

	m_d3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_camera->Render();

	m_d3D->GetWorldMatrix(worldMatrix);
	m_camera->GetViewMatrix(viewMatrix);
	m_d3D->GetProjectionMatrix(projectionMatrix);
	m_d3D->GetOrthoMatrix(orthoMatrix);

	result = m_aesthetic->Render(SCREEN_DEPTH, projectionMatrix, orthoMatrix, m_camera);

	if (!result)
	{
		return false;
	}

	m_d3D->GetWorldMatrix(worldMatrix);
	m_d3D->GetOrthoMatrix(orthoMatrix);

	m_d3D->TurnZBufferOff();
	m_d3D->TurnOnAlphaBlending();

	m_text->Render(m_d3D->GetDeviceContext(), worldMatrix, orthoMatrix);

	m_d3D->TurnOffAlphaBlending();
	m_d3D->TurnZBufferOn();

	m_d3D->EndScene();

	return true;
}

bool GraphicsClass::SetFps(int fps)
{
	m_text->SetFps(fps, m_d3D->GetDeviceContext());
	return true;
}

bool GraphicsClass::SetCpu(int cpu)
{
	m_text->SetCpu(cpu, m_d3D->GetDeviceContext());
	return true;
}

bool GraphicsClass::SetLatency(int latency)
{
	m_text->SetLatency(latency, m_d3D->GetDeviceContext());
	return true;
}