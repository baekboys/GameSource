#ifndef GRAVITY_H
#define GRAVITY_H

#include "irrlicht.h"

class CGravity
{
public :
	// 생성자와 소멸자
	CGravity()
		: m_direction(irr::core::vector3df(0,0,-1)), m_scalar(9.8f), m_angleDeltaValue(5.f), 
		m_isForwardDirectionGravity(true), m_maxLeftAngle(270.f), m_maxRightangle(90.f) {}
	CGravity(irr::core::vector3df direction, irr::f32 scalar, irr::f32 angleDelta, bool isForwardDirectionGravity)
		: m_direction(direction), m_scalar(scalar), m_angleDeltaValue(angleDelta), m_isForwardDirectionGravity(true),
		m_maxLeftAngle(270.f), m_maxRightangle(90.f) {}
	~CGravity() {}

	// 중력 방향 설정
	inline void setDirection(irr::f32 x, irr::f32 y, irr::f32 z)
	{
		m_direction.X = x; m_direction.Y = y; m_direction.Z = z;
		m_direction.normalize();
	}

	// 중력 세기 설정
	inline void setScalar(irr::f32 scalar)
	{
		m_scalar = scalar;
	}

	// 각도변화량 설정
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

	// 각도를 이용하여 중력의 방향을 변화시킴. degree : 각도
	void changeDirectionFromDegree(irr::f32 degree)
	{
		/*irr::core::quaternion qt;
		qt.fromAngleAxis(degree * irr::core::DEGTORAD, irr::core::vector3df(0, 1, 0));
		m_direction = qt * m_direction;*/

		// 각도 변화량
		irr::f32 delta_angleValue = getGravityDirectionAngle() - degree;
		
		// 각도변화
		m_direction.rotateXZBy(delta_angleValue);
	}

	// 멤버변수(각도변화량)을 이용하여 중력의 방향을 변화시킴 true: 시계방향, false : 반시계방향
	bool changeDirectionFromDeltaValue(bool clockwise)
	{

		/////////////////////////////////
		//				0					//
		//									//
		//	270		+			90		//
		//									//
		//				180				//
		/////////////////////////////////
		
		// 시계방향으로 각도를 변화시키면 m_angleDeltaValue 만큼 더한다.
		// 반시계방향이면 빼면 된다.


		// 각도 변화에 사용될 쿼터니언
		irr::core::quaternion qt;
		// 각도 변화량
		irr::f32 deltaValue;

		// 현재의 각도 얻어오기
		irr::f32 currentAngle = getGravityDirectionAngle();

		// 중력방향이 0,0,-1 즉 정방향이면
		if(m_isForwardDirectionGravity)
		{
			// 중력을 시계방향으로 변화시킨다면
			if(clockwise)
			{
				// 값을 미리 계산하여 범위 이상 벗어난다면
				if(currentAngle + m_angleDeltaValue > m_maxLeftAngle)
				{
					// 값을 고정
					m_direction.set(-1,0,0);
					return false;
				}

				// 범위에 벗어나지 않았으므로 변화시킬 각도를 설정.
				else
					deltaValue = m_angleDeltaValue;
			}

			// 중력을 시계반대방향으로 변화시킨다면
			else
			{
				// 값을 미리 계산하여 범위 이상 벗어난다면
				if(currentAngle - m_angleDeltaValue < m_maxRightangle)
				{
					// 값을 고정
					m_direction.set(1,0,0);
					return false;
				}

				// 범위에 벗어나지 않았으므로 변화시킬 각도를 설정.
				else
					deltaValue = m_angleDeltaValue * -1.f;
			}
		}

		// 중력의 방향이 (0,0,1) 즉 역방향이라면
		else
		{

		}

		// 변화시킬 회전값을 가지는 쿼터니언 설정.
		qt.fromAngleAxis(deltaValue * irr::core::DEGTORAD, irr::core::vector3df(0, 1, 0));

		// 쿼터니언을 이용하여 중력방향을 변화시킴.
		m_direction = qt * m_direction;

		return true;
	}

	// 현재 중략방향을 각도로 얻기
	inline irr::f32 getGravityDirectionAngle()
	{
		return m_direction.getHorizontalAngle().Y;
	}

	// 중력 벡터 얻기
	inline irr::core::vector3df getGravityVector()
	{
		return m_direction * m_scalar;
	}

	// 중력 방향벡터 얻기
	inline irr::core::vector3df getDirection()
	{
		irr::core::vector3df dir = m_direction.normalize();
		//dir.Y = 0;
		return dir;
	}

	// 중력 세기 얻기
	inline irr::f32 getScalar()
	{
		return m_scalar;
	}

	// 각도 변화량 얻기
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