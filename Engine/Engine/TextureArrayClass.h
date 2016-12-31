#pragma once

#include <D3D11.h>
#include <D3DX11tex.h>

class TextureArrayClass
{
public:

	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass&);
	~TextureArrayClass();
	
	bool Initialize(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);
	bool Initialize(ID3D11Device*, WCHAR*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView** GetTextureArray();

private:

	ID3D11ShaderResourceView* m_textures[3];
};