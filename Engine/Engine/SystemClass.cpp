#include "SystemClass.h"

SystemClass::SystemClass() :
	
	m_input(0),
	
	m_graphics(0),
	m_sound(0),

	m_timer(0),

	m_position(0),

	m_fps(0),
	m_cpu(0),

	m_network(0)

{
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth;
	int screenHeight;

	bool fullAesthetic = true;

	bool result;

	screenWidth = 0;
	screenHeight = 0;

	if (MessageBox(NULL, L"Would you like to run in fullscreen mode?", L"Display Mode", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		FULL_SCREEN = false;
	}
	else
	{
		FULL_SCREEN = true;
	}

	if(MessageBox(NULL, L"Want full Aesthetics?", L"Graphics Quality", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		fullAesthetic = false;
	}

	InitializeWindows(screenWidth,screenHeight);

	m_input = new InputClass();

	if(!m_input)
	{
		return false;
	}

	result = m_input->Initialize(m_hInstance, m_hWnd, screenWidth, screenHeight);

	if(!result)
	{
		MessageBox(m_hWnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	m_sound = new SoundClass();

	if (!m_sound)
	{
		return false;
	}

	result = m_sound->Initialize(m_hWnd);

	if (!result)
	{
		return false;
	}

	m_graphics = new GraphicsClass();

	if(!m_graphics)
	{
		return false;
	}

	result = m_graphics->Initialize(screenWidth, screenHeight, m_hWnd, m_sound, FULL_SCREEN, fullAesthetic);

	if(!result)
	{
		return false;
	}

	m_fps = new FpsClass();

	if(!m_fps)
	{
		return false;
	}

	m_fps->Initialize();

	m_cpu = new CpuClass();

	if(!m_cpu)
	{
		return false;
	}

	m_cpu->Initialize();

	if(NETWORK_ENABLED)
	{
		m_network = new NetworkClass();

		if(!m_network)
		{
			return false;
		}

		result = m_network->Initialize();

		if(!result)
		{
			MessageBox(m_hWnd, L"Could not initialize the network object.", L"Error", MB_OK);
			return false;
		}

		result = m_network->ConnectToServer(SERVER_ADDRESS, SERVER_PORT);

		if(!result)
		{
			MessageBox(m_hWnd, L"Could not connect to server.", L"Error", MB_OK);
			return false;
		}
	}

	m_timer = new TimerClass();

	if(!m_timer)
	{
		return false;
	}

	result = m_timer->Initialize();

	if(!result)
	{
		MessageBox(m_hWnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	m_position = new PositionClass();

	if(!m_position)
	{
		return false;
	}

	m_position->SetPosition(0.0f, 1.5f, -11.0f);

	return true;
}

void SystemClass::Shutdown()
{
	if(NETWORK_ENABLED)
	{
		if(m_network)
		{
			m_network->Shutdown();
			delete m_network;
			m_network = 0;
		}
	}
	if(m_position)
	{
		delete m_position;
		m_position = 0;
	}

	if(m_timer)
	{
		delete m_timer;
		m_timer = 0;
	}

	if(m_graphics)
	{
		m_graphics->Shutdown();
		delete m_graphics;
		m_graphics = 0;
	}

	if(m_input)
	{
		m_input->Shutdown();
		delete m_input;
		m_input = 0;
	}

	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;

	while(!done)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Frame();

			if(!result)
			{
				done = true;
			}
		}

		if(m_input->IsEscapePressed() == true)
		{
			done = true;
		}
	}
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool SystemClass::Frame()
{
	bool keyDown, result;
	float positionX, positionY, positionZ;

	m_timer->Frame();

	result = m_input->Frame(m_posX, m_posY);

	if(!result)
	{
		return false;
	}

	m_position->SetFrameTime(m_timer->GetTime());

	keyDown = m_input->IsLeftArrowPressed();
	m_position->MoveLeft(keyDown);

	keyDown = m_input->IsRightArrowPressed();
	m_position->MoveRight(keyDown);

	m_position->GetPosition(positionX, positionY, positionZ);

	m_fps->Frame();
	m_cpu->Frame();

	if(NETWORK_ENABLED)
	{
		m_network->Frame();
	}

	result = m_graphics->SetFps(m_fps->GetFps());

	if(!result)
	{
		return false;
	}

	result = m_graphics->SetCpu(m_cpu->GetCpuPercentage());

	if(!result)
	{
		return false;
	}

	if(NETWORK_ENABLED)
	{
		result = m_graphics->SetLatency(m_network->GetLatency());

		if(!result)
		{
			return false;
		}
	}
	else
	{
		result = m_graphics->SetLatency(0);

		if(!result)
		{
			return false;
		}
	}

	result = m_graphics->Frame(m_timer->GetTime());
	
	if(!result)
	{
		return false;
	}

	return true;
}

void SystemClass::InitializeWindows(int& screenWidth,int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	
	int posX;
	int posY;

	applicationHandle = this;

	m_hInstance = GetModuleHandle(NULL);

	m_applicationName = L"Engine";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if(FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));

		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		
		posX = posY = 0;
	}
	else
	{
		screenWidth = 1280;
		screenHeight = 720;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	
	m_posX = posX;
	m_posY = posY;

	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	ShowCursor(true);

	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(m_hWnd);

	m_hWnd = NULL;

	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = NULL;

	applicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uMessage)
	{
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

	default:
		{
			return applicationHandle->MessageHandler(hWnd, uMessage, wParam, lParam);
		}
	}
}