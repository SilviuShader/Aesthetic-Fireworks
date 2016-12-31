#include "FontClass.h"

using namespace std;

FontClass::FontClass() :

	m_font(0),
	m_texture(0)

{
}

FontClass::FontClass(const FontClass&)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename)
{
	bool result;

	result = LoadFontData(fontFilename);

	if(!result)
	{
		return false;
	}

	result = LoadTexture(device, textureFilename);

	if(!result)
	{
		return false;
	}

	return true;
}

void FontClass::Shutdown()
{
	ReleaseTexture();

	ReleaseFontData();

	return;
}

ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_texture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, letter;

	vertexPtr = (VertexType*)vertices;

	numLetters = (int)strlen(sentence);

	index = 0;

	for(int i=0; i<numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		if(letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + m_font[letter].size), (drawY - 16), 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX, (drawY - 16), 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_font[letter].left, 1.0f);
			index++;


			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + m_font[letter].size), drawY, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + m_font[letter].size), (drawY - 16), 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(m_font[letter].right, 1.0f);
			index++;

			drawX = drawX + m_font[letter].size + 1.0f;
		}
	}

	return;
}

bool FontClass::LoadFontData(char* filename)
{
	ifstream fin;
	char temp;

	m_font = new FontType[95];

	if(!m_font)
	{
		return false;
	}

	fin.open(filename);

	if(fin.fail())
	{
		return false;
	}

	for(int i=0;i<95;i++)
	{
		fin.get(temp);

		while(temp != ' ')
		{
			fin.get(temp);
		}

		fin.get(temp);

		while(temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_font[i].left;
		fin >> m_font[i].right;
		fin >> m_font[i].size;
	}

	fin.close();

	return true;
}

void FontClass::ReleaseFontData()
{
	if(m_font)
	{
		delete[] m_font;
		m_font = 0;
	}

	return;
}

bool FontClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	m_texture = new TextureClass();

	if(!m_texture)
	{
		return false;
	}

	result = m_texture->Initialize(device, filename);

	if(!result)
	{
		return false;
	}

	return true;
}

void FontClass::ReleaseTexture()
{
	if(m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}

	return;
}