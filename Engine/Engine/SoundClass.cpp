#include "SoundClass.h"

SoundClass::SoundClass() :

	m_directSound(0),
	m_primaryBuffer(0),

	m_listener(0),
	m_secondaryBuffer1(0),
	m_secondary3DBuffer1(0),

	m_secondaryBuffer2(0),
	m_secondary3DBuffer2(0)

{
}

SoundClass::SoundClass(const SoundClass& other)
{
}

SoundClass::~SoundClass()
{
}

bool SoundClass::Initialize(HWND hWnd)
{
	bool result;

	result = InitializeDirectSound(hWnd);

	if(!result)
	{
		return false;
	}

	result = LoadWaveFile("../Engine/data/explosion.wav", &m_secondaryBuffer1, &m_secondary3DBuffer1);

	if(!result)
	{
		return false;
	}

	result = LoadWaveFile("../Engine/data/sound02.wav", &m_secondaryBuffer2, &m_secondary3DBuffer2);

	if (!result)
	{
		return false;
	}

	result = PlayWaveFile(0.0f);

	if(!result)
	{
		return false;
	}

	return true;
}

void SoundClass::Shutdown()
{
	ShutdownWaveFile(&m_secondaryBuffer1, &m_secondary3DBuffer1);

	ShutdownDirectSound();

	return;
}

bool SoundClass::InitializeDirectSound(HWND hWnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	result = DirectSoundCreate8(NULL, &m_directSound, NULL);

	if(FAILED(result))
	{
		return false;
	}

	result = m_directSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);

	if(FAILED(result))
	{
		return false;
	}

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	result = m_directSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);

	if(FAILED(result))
	{
		return false;
	}

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	result = m_primaryBuffer->SetFormat(&waveFormat);

	if(FAILED(result))
	{
		return false;
	}

	result = m_primaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&m_listener);

	if(FAILED(result))
	{
		return false;
	}

	m_listener->SetPosition(0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE);

	return true;
}

void SoundClass::ShutdownDirectSound()
{
	if(m_listener)
	{
		m_listener->Release();
		m_listener = 0;
	}

	if(m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	if(m_directSound)
	{
		m_directSound->Release();
		m_directSound = 0;
	}
}

bool SoundClass::LoadWaveFile(char* filename, IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;

	error = fopen_s(&filePtr, filename, "rb");

	if(error != 0)
	{
		return false;
	}

	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);

	if(count != 1)
	{
		return false;
	}

	if((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') || (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	if((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') || (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	if(waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	if(waveFileHeader.numChannels != 1)
	{
		return false;
	}

	if(waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	if(waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') || (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 1;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	result = m_directSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);

	if(FAILED(result))
	{
		return false;
	}

	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);

	if(FAILED(result))
	{
		return false;
	}

	tempBuffer->Release();
	tempBuffer = 0;

	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	waveData = new unsigned char[waveFileHeader.dataSize];

	if(!waveData)
	{
		return false;
	}

	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);

	if(count != waveFileHeader.dataSize)
	{
		return false;
	}

	error = fclose(filePtr);

	if(error != 0)
	{
		return false;
	}

	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);

	if(FAILED(result))
	{
		return false;
	}

	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);

	if(FAILED(result))
	{
		return false;
	}

	delete[] waveData;
	waveData = 0;

	result = (*secondaryBuffer)->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&*secondary3DBuffer);

	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer)
{
	if(*secondary3DBuffer)
	{
		(*secondary3DBuffer)->Release();
		*secondary3DBuffer = 0;
	}

	if(*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}

	return;
}

bool SoundClass::PlayWaveFile(float positionX)
{
	HRESULT result;
	float positionY, positionZ;

	positionY = 0.0f;
	positionZ = 0.0f;

	result = m_secondaryBuffer1->SetCurrentPosition(0);

	if(FAILED(result))
	{
		return false;
	}

	result = m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX);
	
	if(FAILED(result))
	{
		return false;
	}

	m_secondary3DBuffer1->SetPosition(positionX, positionY, positionZ, DS3D_IMMEDIATE);

	result = m_secondaryBuffer1->Play(0, 0, 0);

	if(FAILED(result))
	{
		return false;
	}

	return true;
}

bool SoundClass::PlayLaunch(float positionX)
{
	HRESULT result;
	float positionY, positionZ;

	positionY = 0.0f;
	positionZ = 0.0f;

	result = m_secondaryBuffer2->SetCurrentPosition(0);

	if (FAILED(result))
	{
		return false;
	}

	result = m_secondaryBuffer2->SetVolume(DSBVOLUME_MAX);

	if (FAILED(result))
	{
		return false;
	}

	m_secondary3DBuffer2->SetPosition(positionX, positionY, positionZ, DS3D_IMMEDIATE);

	result = m_secondaryBuffer2->Play(0, 0, 0);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}
