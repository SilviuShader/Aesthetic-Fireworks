#include "BilboardClass.h"

float BilboardClass::GetRotation(D3DXVECTOR3 cameraPosition, D3DXVECTOR3 modelPosition)
{
	double angle;
	float rotation;

	angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0f / D3DX_PI);

	rotation = (float)angle * 0.0174532925f;

	return rotation;
}