#include <D3DX10math.h>

class PointLightClass
{
public:

	PointLightClass();
	PointLightClass(const PointLightClass&);
	~PointLightClass();

	void SetDiffuseColor(float, float, float, float);
	void SetPosition(float, float, float);

	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR4 GetPosition();

private:

	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR4 m_position;
};