#include "InputClass.h"

InputClass::InputClass() :

	m_directInput(0),
	m_keyboard(0),
	m_mouse(0)

{
}

InputClass::InputClass(const InputClass& other)
{
}

InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hInstance, HWND hWnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_mouseX = 0;
	m_mouseY = 0;

	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);

	if(FAILED(result))
	{
		return false;
	}

	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);

	if(FAILED(result))
	{
		return false;
	}

	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);

	if(FAILED(result))
	{
		return false;
	}

	result = m_keyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);

	if(FAILED(result))
	{
		return false;
	}

	result = m_keyboard->Acquire();

	if(FAILED(result))
	{
		return false;
	}

	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);

	if(FAILED(result))
	{
		return false;
	}

	result = m_mouse->SetDataFormat(&c_dfDIMouse);

	if(FAILED(result))
	{
		return false;
	}

	result = m_mouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	if(FAILED(result))
	{
		return false;
	}

	result = m_mouse->Acquire();

	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void InputClass::Shutdown()
{
	if(m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	if(m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

bool InputClass::Frame(int posX, int posY)
{
	bool result;

	result = ReadKeyboard();

	if(!result)
	{
		return false;
	}

	result = ReadMouse();

	if(!result)
	{
		return false;
	}

	ProcessInput(posX, posY);

	return true;
}

bool InputClass::IsEscapePressed()
{
	if(m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsLeftArrowPressed()
{
	if(m_keyboardState[DIK_LEFT] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsRightArrowPressed()
{
	if(m_keyboardState[DIK_RIGHT] & 0x80)
	{
		return true;
	}

	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;

	return;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);

	if(FAILED(result))
	{
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);

		if(FAILED(result))
	{
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput(int posX, int posY)
{
	POINT position;

	GetCursorPos(&position);

	m_mouseX = position.x;
	m_mouseY = position.y;

	m_mouseX -= posX;
	m_mouseY -= posY;

	if(m_mouseX < 0)
	{
		m_mouseX = 0;
	}

	if(m_mouseY < 0)
	{
		m_mouseY = 0;
	}

	if(m_mouseX > m_screenWidth)
	{
		m_mouseX = m_screenWidth;
	}

	if(m_mouseY > m_screenHeight)
	{
		m_mouseY = m_screenHeight;
	}

	return;
}