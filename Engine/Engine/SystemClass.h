#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include "InputClass.h"
#include "GraphicsClass.h"
#include "SoundClass.h"

#include "TimerClass.h"
#include "PositionClass.h"

#include "FpsClass.h"
#include "CpuClass.h"
#include "NetworkClass.h"

class SystemClass
{
public:

	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:

	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:

	LPCWSTR m_applicationName;
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	InputClass* m_input;
	GraphicsClass* m_graphics;
	SoundClass* m_sound;

	TimerClass* m_timer;
	PositionClass* m_position;

	FpsClass* m_fps;
	CpuClass* m_cpu;
	NetworkClass* m_network;

	int m_posX;
	int m_posY;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* applicationHandle = 0;