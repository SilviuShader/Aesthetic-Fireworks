#include "PositionClass.h"

PositionClass::PositionClass() :

	m_frameTime(0.0f),

	m_positionX(0.0f),
	m_positionY(0.0f),
	m_positionZ(0.0f),

	m_rotationX(0.0f),
	m_rotationY(0.0f),
	m_rotationZ(0.0f),

	m_leftTurnSpeed(0.0f),
	m_rightTurnSpeed(0.0f),

	m_leftSpeed(0),
	m_rightSpeed(0)

{
}

PositionClass::PositionClass(const PositionClass&)
{
}

PositionClass::~PositionClass()
{
}

void PositionClass::SetPosition(float positionX, float positionY, float positionZ)
{
	m_positionX = positionX;
	m_positionY = positionY;
	m_positionZ = positionZ;
	return;
}

void PositionClass::SetRotation(float rotationX, float rotationY, float rotationZ)
{
	m_rotationX = rotationX;
	m_rotationY = rotationY;
	m_rotationZ = rotationZ;
	return;
}

void PositionClass::SetFrameTime(float time)
{
	m_frameTime = time;
	return;
}

void PositionClass::GetPosition(float& x, float& y, float& z)
{
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;
}

void PositionClass::GetRotation(float& x, float& y, float& z)
{
	x = m_rotationX;
	y = m_rotationY;
	z = m_rotationZ;
}

void PositionClass::GetRotation(float& y)
{
	y = m_rotationY;
	return;
}

void PositionClass::TurnLeft(bool keydown)
{
	if(keydown)
	{
		m_leftTurnSpeed += m_frameTime * 0.01f;

		if(m_leftTurnSpeed > (m_frameTime * 0.15f))
		{
			m_leftTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime * 0.005f;

		if(m_leftTurnSpeed < 0.0f)
		{
			m_leftTurnSpeed = 0.0f;
		}
	}

	m_rotationY -= m_leftTurnSpeed;

	if(m_rotationY < 0.0f)
	{
		m_rotationY += 360.0f;
	}

	return;
}

void PositionClass::TurnRight(bool keydown)
{
	if(keydown)
	{
		m_rightTurnSpeed += m_frameTime * 0.01f;

		if(m_rightTurnSpeed > (m_frameTime * 0.15f))
		{
			m_rightTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime * 0.005f;

		if(m_rightTurnSpeed < 0.0f)
		{
			m_rightTurnSpeed = 0.0f;
		}
	}

	m_rotationY += m_rightTurnSpeed;

	if(m_rotationY > 360.0f)
	{
		m_rotationY -= 360.0f;
	}

	return;
}

void PositionClass::MoveLeft(bool keydown)
{
	float radians;

	if(keydown)
	{
		m_leftSpeed += m_frameTime * 0.001f;

		if(m_leftSpeed > (m_frameTime * 0.03f))
		{
			m_leftSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		m_leftSpeed -= m_frameTime * 0.0007f;

		if(m_leftSpeed < 0.0f)
		{
			m_leftSpeed = 0.0f;
		}
	}

	radians = m_rotationY * 0.0174532925f;

	m_positionX -= cosf(radians) * m_leftSpeed;
	m_positionZ -= sinf(radians) * m_leftSpeed;
}

void PositionClass::MoveRight(bool keydown)
{
	float radians;

	if(keydown)
	{
		m_rightSpeed += m_frameTime * 0.001f;

		if(m_rightSpeed > (m_frameTime * 0.03f))
		{
			m_rightSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		m_rightSpeed -= m_frameTime * 0.0007f;

		if(m_rightSpeed < 0.0f)
		{
			m_rightSpeed = 0.0f;
		}
	}

	radians = m_rotationY * 0.0174532925f;

	m_positionX += cosf(radians) * m_rightSpeed;
	m_positionZ += sinf(radians) * m_rightSpeed;
}