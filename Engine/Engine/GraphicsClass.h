#pragma once

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

static bool FULL_SCREEN;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

#include "D3DClass.h"
#include "CameraClass.h"

#include "AestheticClass.h"

#include "TextClass.h"
#include "SoundClass.h"

class GraphicsClass
{
public:

	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND, SoundClass*, bool, bool);
	void Shutdown();
	bool Frame(float);

	bool SetFps(int);
	bool SetCpu(int);
	bool SetLatency(int);

private:

	bool Render();

private:

	D3DClass* m_d3D;
	CameraClass* m_camera;

	TextClass* m_text;

	AestheticClass* m_aesthetic;
};