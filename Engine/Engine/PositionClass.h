#pragma once

#include <math.h>

class PositionClass
{
public:

	PositionClass();
	PositionClass(const PositionClass&);
	~PositionClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	void SetFrameTime(float);

	void GetPosition(float&, float&, float&);
	void GetRotation(float&, float&, float&);

	void GetRotation(float&);

	void TurnLeft(bool);
	void TurnRight(bool);

	void MoveLeft(bool);
	void MoveRight(bool);

private:

	float m_frameTime;

	float m_positionX;
	float m_positionY;
	float m_positionZ;

	float m_rotationX;
	float m_rotationY;
	float m_rotationZ;
	
	float m_leftTurnSpeed, m_rightTurnSpeed;
	float m_leftSpeed, m_rightSpeed;
};