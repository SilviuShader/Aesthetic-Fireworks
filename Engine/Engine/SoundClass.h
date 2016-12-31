#pragma once

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>

#include <stdio.h>

class SoundClass
{
private:

	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

public:

	SoundClass();
	SoundClass(const SoundClass&);
	~SoundClass();

	bool Initialize(HWND);
	void Shutdown();

	bool PlayWaveFile(float);
	bool PlayLaunch(float);

private:

	bool InitializeDirectSound(HWND);
	void ShutdownDirectSound();

	bool LoadWaveFile(char*, IDirectSoundBuffer8**, IDirectSound3DBuffer8**);
	void ShutdownWaveFile(IDirectSoundBuffer8**, IDirectSound3DBuffer8**);

private:

	IDirectSound8* m_directSound;
	IDirectSoundBuffer* m_primaryBuffer;

	IDirectSound3DListener8* m_listener;

	IDirectSoundBuffer8* m_secondaryBuffer1;
	IDirectSound3DBuffer8* m_secondary3DBuffer1;

	IDirectSoundBuffer8* m_secondaryBuffer2;
	IDirectSound3DBuffer8* m_secondary3DBuffer2;
};