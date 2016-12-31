#include "AestheticClass.h"

AestheticClass::AestheticClass() :

	m_screenWidth(0),
	m_screenHeight(0),

	m_d3D(NULL),
	m_fireworksController(NULL),

	m_backgroundBitmap(NULL),
	m_backgroundTexture(NULL),

	m_textureShader(NULL),
	m_gotBaseViewMatrix(false),

	m_floor(NULL),
	m_floorTexture(NULL),

	m_fullAesthetic(true)

{
	D3DXMatrixIdentity(&m_identityMatrix);
}

AestheticClass::AestheticClass(const AestheticClass& other)
{
}

AestheticClass::~AestheticClass()
{
}

bool AestheticClass::Initialize(int screenWidth, int screenHeight, D3DClass* d3D, HWND hWnd, CameraClass* camera, SoundClass* sound, bool fullAesthetic)
{
	bool result;

	m_fullAesthetic = fullAesthetic;

	m_sound = sound;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_d3D = d3D;

	m_fireworksController = new FireworksControllerClass();

	if (!m_fireworksController)
	{
		return false;
	}

	result = m_fireworksController->Initialize(m_d3D, hWnd, camera);

	if (!result)
	{
		return false;
	}

	m_backgroundBitmap = new BitmapClass();

	if (!m_backgroundBitmap)
	{
		return false;
	}

	result = m_backgroundBitmap->Initialize(m_d3D->GetDevice(), m_screenWidth, m_screenHeight, m_screenWidth, m_screenHeight);

	if (!result)
	{
		return false;
	}

	m_backgroundTexture = new RenderTextureClass();

	if (!m_backgroundTexture)
	{
		return false;
	}

	result = m_backgroundTexture->Initialize(m_d3D->GetDevice(), m_screenWidth, m_screenHeight);
	
	if (!result)
	{
		return false;
	}
	
	m_backgroundTexture->SetRenderTarget(m_d3D->GetDeviceContext(), m_d3D->GetDepthStencilView());
	m_backgroundTexture->ClearRenderTarget(m_d3D->GetDeviceContext(), m_d3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);
	m_d3D->SetBackBufferRenderTarget();

	m_textureShader = new TextureShaderClass();

	if (!m_textureShader)
	{
		return false;
	}

	result = m_textureShader->Initialize(m_d3D->GetDevice(), hWnd);

	if (!result)
	{
		return false;
	}

	m_floor = new FloorClass();

	if (!m_floor)
	{
		return false;
	}

	result = m_floor->Initialize(m_d3D, hWnd);

	if (!result)
	{
		return false;
	}

	m_floorTexture = new RenderTextureClass();

	if (!m_floorTexture)
	{
		return false;
	}

	result = m_floorTexture->Initialize(m_d3D->GetDevice(), screenWidth, screenHeight);

	if (!result)
	{
		return false;
	}
	
	return true;
}

void AestheticClass::Shutdown()
{
	if (m_floorTexture)
	{
		m_floorTexture->Shutdown();
		delete m_floorTexture;
		m_floorTexture = NULL;
	}
	
	if (m_floor)
	{
		m_floor->Shutdown();
		delete m_floor;
		m_floor = NULL;
	}

	if (m_fireworksController)
	{
		m_fireworksController->Shutdown();
		delete m_fireworksController;
		m_fireworksController = NULL;
	}

	if (m_backgroundTexture)
	{
		m_backgroundTexture->Shutdown();
		delete m_backgroundTexture;
		m_backgroundTexture = NULL;
	}

	if (m_backgroundBitmap)
	{
		m_backgroundBitmap->Shutdown();
		delete m_backgroundBitmap;
		m_backgroundBitmap = NULL;
	}

	if (m_textureShader)
	{
		m_textureShader->Shutdown();
		delete m_textureShader;
		m_textureShader = NULL;
	}
}

void AestheticClass::Update(float frameTime, CameraClass* camera)
{
	m_fireworksController->Update(frameTime, camera, m_sound);
	UpdateCamera(frameTime, camera);
	m_floor->Update(camera);
}

bool AestheticClass::Render(float screenDepth, D3DXMATRIX projection, D3DXMATRIX ortho, CameraClass* camera)
{
	D3DXMATRIX view;

	if (!m_gotBaseViewMatrix)
	{
		camera->GetViewMatrix(m_baseViewMatrix);
		m_gotBaseViewMatrix = true;
	}
	
	camera->GetViewMatrix(view);

	m_floorTexture->SetRenderTarget(m_d3D->GetDeviceContext(), m_d3D->GetDepthStencilView());
	m_floorTexture->ClearRenderTarget(m_d3D->GetDeviceContext(), m_d3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);
	RenderInFloor(screenDepth, projection, ortho, camera);

	m_backgroundTexture->SetRenderTarget(m_d3D->GetDeviceContext(), m_d3D->GetDepthStencilView());
	DrawAestheticScene(screenDepth, projection, ortho, camera);

	m_d3D->SetBackBufferRenderTarget();
	DrawAestheticScene(screenDepth, projection, ortho, camera);

	m_d3D->TurnZBufferOff();

	m_backgroundBitmap->Render(m_d3D->GetDeviceContext(), 0, 0);
	m_textureShader->Render(m_d3D->GetDeviceContext(), m_backgroundBitmap->GetIndexCount(), m_identityMatrix, m_baseViewMatrix, ortho, m_backgroundTexture->GetShaderResourceView());
	
	m_d3D->TurnZBufferOn();
	
	DrawNormalScene(screenDepth, projection, ortho, camera);

	return true;
}

void AestheticClass::UpdateCamera(float frameTime, CameraClass* camera)
{
	if (frameTime >= 100.0f)
		frameTime = 100.0f;

	D3DXVECTOR3 pos;
	pos = camera->GetPosition();
	pos.z += (0.008f * frameTime);
	camera->SetPosition(pos.x, pos.y, pos.z);
}

void AestheticClass::DrawNormalScene(float screenDepth, D3DXMATRIX projection, D3DXMATRIX ortho, CameraClass* camera)
{
	m_fireworksController->Render(screenDepth, projection, camera, false, 0.0f);
	m_floor->Render(camera, m_floorTexture->GetShaderResourceView(), projection, m_fireworksController->GetFireworks(), MAX_FIREWORKS);
}

void AestheticClass::DrawAestheticScene(float screenDepth, D3DXMATRIX projection, D3DXMATRIX ortho, CameraClass* camera)
{
	if (m_fullAesthetic)
	{
		DrawNormalScene(screenDepth, projection, ortho, camera);
	}
}

void AestheticClass::RenderInFloor(float screenDepth, D3DXMATRIX projection, D3DXMATRIX ortho, CameraClass* camera)
{
	m_fireworksController->Render(screenDepth, projection, camera, true, m_floor->GetHeight());
}
