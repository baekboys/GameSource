#ifndef GRAVITY_H
#define GRAVITY_H

#include "irrlicht.h"

class CGravity
{
public :
	// �����ڿ� �Ҹ���
	CGravity()
		: m_direction(irr::core::vector3df(0,0,-1)), m_scalar(9.8f), m_angleDeltaValue(5.f), 
		m_isForwardDirectionGravity(true), m_maxLeftAngle(270.f), m_maxRightangle(90.f) {}
	CGravity(irr::core::vector3df direction, irr::f32 scalar, irr::f32 angleDelta, bool isForwardDirectionGravity)
		: m_direction(direction), m_scalar(scalar), m_angleDeltaValue(angleDelta), m_isForwardDirectionGravity(true),
		m_maxLeftAngle(270.f), m_maxRightangle(90.f) {}
	~CGravity() {}

	// �߷� ���� ����
	inline void setDirection(irr::f32 x, irr::f32 y, irr::f32 z)
	{
		m_direction.X = x; m_direction.Y = y; m_direction.Z = z;
		m_direction.normalize();
	}

	// �߷� ���� ����
	inline void setScalar(irr::f32 scalar)
	{
		m_scalar = scalar;
	}

	// ������ȭ�� ����
	inline void setAngleDeltaValue(irr::f32 angle)
	{
		m_angleDeltaValue = angle;
	}

	void set(irr::f32 x, irr::f32 y, irr::f32 z, irr::f32 scalar, irr::f32 angle, bool isForwardDirectionGravity)
	{
		m_direction.X = x; m_direction.Y = y; m_direction.Z = z;
		m_direction.normalize();

		m_scalar = scalar;

		m_angleDeltaValue = angle;

		m_isForwardDirectionGravity = isForwardDirectionGravity;
	}

	// ������ �̿��Ͽ� �߷��� ������ ��ȭ��Ŵ. degree : ����
	void changeDirectionFromDegree(irr::f32 degree)
	{
		/*irr::core::quaternion qt;
		qt.fromAngleAxis(degree * irr::core::DEGTORAD, irr::core::vector3df(0, 1, 0));
		m_direction = qt * m_direction;*/

		// ���� ��ȭ��
		irr::f32 delta_angleValue = getGravityDirectionAngle() - degree;
		
		// ������ȭ
		m_direction.rotateXZBy(delta_angleValue);
	}

	// �������(������ȭ��)�� �̿��Ͽ� �߷��� ������ ��ȭ��Ŵ true: �ð����, false : �ݽð����
	bool changeDirectionFromDeltaValue(bool clockwise)
	{

		/////////////////////////////////
		//				0					//
		//									//
		//	270		+			90		//
		//									//
		//				180				//
		/////////////////////////////////
		
		// �ð�������� ������ ��ȭ��Ű�� m_angleDeltaValue ��ŭ ���Ѵ�.
		// �ݽð�����̸� ���� �ȴ�.


		// ���� ��ȭ�� ���� ���ʹϾ�
		irr::core::quaternion qt;
		// ���� ��ȭ��
		irr::f32 deltaValue;

		// ������ ���� ������
		irr::f32 currentAngle = getGravityDirectionAngle();

		// �߷¹����� 0,0,-1 �� �������̸�
		if(m_isForwardDirectionGravity)
		{
			// �߷��� �ð�������� ��ȭ��Ų�ٸ�
			if(clockwise)
			{
				// ���� �̸� ����Ͽ� ���� �̻� ����ٸ�
				if(currentAngle + m_angleDeltaValue > m_maxLeftAngle)
				{
					// ���� ����
					m_direction.set(-1,0,0);
					return false;
				}

				// ������ ����� �ʾ����Ƿ� ��ȭ��ų ������ ����.
				else
					deltaValue = m_angleDeltaValue;
			}

			// �߷��� �ð�ݴ�������� ��ȭ��Ų�ٸ�
			else
			{
				// ���� �̸� ����Ͽ� ���� �̻� ����ٸ�
				if(currentAngle - m_angleDeltaValue < m_maxRightangle)
				{
					// ���� ����
					m_direction.set(1,0,0);
					return false;
				}

				// ������ ����� �ʾ����Ƿ� ��ȭ��ų ������ ����.
				else
					deltaValue = m_angleDeltaValue * -1.f;
			}
		}

		// �߷��� ������ (0,0,1) �� �������̶��
		else
		{

		}

		// ��ȭ��ų ȸ������ ������ ���ʹϾ� ����.
		qt.fromAngleAxis(deltaValue * irr::core::DEGTORAD, irr::core::vector3df(0, 1, 0));

		// ���ʹϾ��� �̿��Ͽ� �߷¹����� ��ȭ��Ŵ.
		m_direction = qt * m_direction;

		return true;
	}

	// ���� �߷������� ������ ���
	inline irr::f32 getGravityDirectionAngle()
	{
		return m_direction.getHorizontalAngle().Y;
	}

	// �߷� ���� ���
	inline irr::core::vector3df getGravityVector()
	{
		return m_direction * m_scalar;
	}

	// �߷� ���⺤�� ���
	inline irr::core::vector3df getDirection()
	{
		irr::core::vector3df dir = m_direction.normalize();
		//dir.Y = 0;
		return dir;
	}

	// �߷� ���� ���
	inline irr::f32 getScalar()
	{
		return m_scalar;
	}

	// ���� ��ȭ�� ���
	inline irr::f32 getDeltaValue()
	{
		return m_angleDeltaValue;
	}

	inline void reverseGravity()
	{
		if(m_isForwardDirectionGravity)
		{
			m_isForwardDirectionGravity = false;
			m_direction.set(0,0,1.f);
		}
		else
		{
			m_isForwardDirectionGravity = true;
			m_direction.set(0,0,-1.f);
		}
	}

private :
	irr::core::vector3df m_direction;
	irr::f32 m_scalar;
	irr::f32 m_angleDeltaValue;
	bool m_isForwardDirectionGravity;

	const irr::f32 m_maxLeftAngle;
	const irr::f32 m_maxRightangle;
};

#endif