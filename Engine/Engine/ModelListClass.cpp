#include "ModelListClass.h"

ModelListClass::ModelListClass() :
	
	m_modelInfoList(0)

{
}

ModelListClass::ModelListClass(const ModelListClass&)
{
}

ModelListClass::~ModelListClass()
{
}

bool ModelListClass::Initialize(int numModels)
{
	float red, green, blue;

	m_modelCount = numModels;

	m_modelInfoList = new ModelInfoType[m_modelCount];

	if(!m_modelInfoList)
	{
		return false;
	}

	srand((unsigned int)time(NULL));

	for(int i=0;i<m_modelCount;i++)
	{
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		m_modelInfoList[i].color = D3DXVECTOR4(red, green, blue, 1.0f);

		m_modelInfoList[i].positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_modelInfoList[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_modelInfoList[i].positionZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f) + 5.0f;
	}

	return true;
}

void ModelListClass::Shutdown()
{
	if(m_modelInfoList)
	{
		delete[] m_modelInfoList;
		m_modelInfoList = 0;
	}

	return;
}

int ModelListClass::GetModelCount()
{
	return m_modelCount;
}

void ModelListClass::GetData(int index, float& positionX, float& positionY, float& positionZ, D3DXVECTOR4& color)
{
	positionX = m_modelInfoList[index].positionX;
	positionY = m_modelInfoList[index].positionY;
	positionZ = m_modelInfoList[index].positionZ;

	color = m_modelInfoList[index].color;

	return;
}