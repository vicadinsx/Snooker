#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "matrix.h"
#include "quaternion.h"
#include <cmath>

namespace math {
	class Object {

		Matrix4 _model;
		float _radius;
		Vector2 _direction;
		Vector2 _speed;
		Vector2 _acceleration;
		float _mass;
		float friction = 0.0002;
		Quaternion rotationQuaternion = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

	public:
		Object(){}
		Object(Matrix4 m, float r, Vector2 d, Vector2 s, Vector2 a, float mass) {
			_model = m;
			_radius = 0.35;
			_direction = d;
			_speed = s;
			_acceleration = a;
			_mass = mass;
		}
		~Object(){}
		Matrix4 model() { return this->_model; }
		Matrix4 modelMatrix() { return this->_model*this->rotationQuaternion.toMatrix(); }
		float radius() { return this->_radius; }
		Vector2 direction() { return this->_direction; }
		Vector2 speed() { return this->_speed; }
		Vector2 acceleration() { return this->_acceleration; }
		float mass() { return _mass; }

		void setModel(Matrix4 m) { _model = m; }
		void setRadius(float r) { _radius = r; }
		void setDirection(Vector2 d) { _direction = d; }
		void setSpeed(Vector2 s) { _speed = s; }
		void setAcceleration(Vector2 a) { _acceleration = a; }
		void setMass(float m) { _mass = m; }

		//AABB overlaps method for balls
		friend bool overlaps(Object *o1, Object *o2) {
			if (o1->model().getElement(0,3) + o1->radius() + o2->radius() > o2->model().getElement(0, 3)
				&& o1->model().getElement(0, 3) < o1->radius() + o2->radius() + o2->model().getElement(0, 3)
				&& o1->model().getElement(1, 3) + o1->radius() + o2->radius() > o2->model().getElement(1, 3)
				&& o1->model().getElement(1, 3) < o1->radius() + o2->radius() + o2->model().getElement(1, 3))
				return true;
			return false;
		}

		//Check if balls are colliding
		friend bool colliding(Object *o1, Object *o2) {
			float distance = sqrt(pow(o1->model().getElement(0, 3) - o2->model().getElement(0, 3), 2) + pow(o1->model().getElement(1, 3) - o2->model().getElement(1, 3), 2));
			if (distance < o1->radius() + o2->radius())
				return true;
			return false;
		}

		//Return collision x and y position
		friend Vector2 collisionPoints(Object* o1, Object *o2) {
			float collisionPointX = ((o1->model().getElement(0, 3)*o2->radius()) + o2->model().getElement(0, 3)*o1->radius()) / (o1->radius() + o2->radius());
			float collisionPointY = ((o1->model().getElement(1, 3)*o2->radius()) + o2->model().getElement(1, 3)*o1->radius()) / (o1->radius() + o2->radius());
			return { collisionPointX, collisionPointY };
		}

		//Buggy collision
		friend void collide(Object* o1, Object* o2) {

			float newVelX1 = (o1->speed().x * (o1->mass() - o2->mass()) + (2 * o2->mass() * o2->speed().x)) / (o1->mass() + o2->mass());
			float newVelX2 = (o2->speed().x * (o2->mass() - o1->mass()) + (2 * o1->mass() * o1->speed().x)) / (o1->mass() + o2->mass());
			float newVelY1 = (o1->speed().y * (o1->mass() - o2->mass()) + (2 * o2->mass() * o2->speed().y)) / (o1->mass() + o2->mass());
			float newVelY2 = (o2->speed().y * (o2->mass() - o1->mass()) + (2 * o1->mass() * o1->speed().y)) / (o1->mass() + o2->mass());

			//firstBall.speed.x = newVelX1;
			//firstBall.speed.y = newVelY1;
			o1->setSpeed(Vector2(newVelX1, newVelY1));
			//secondBall.speed.x = newVelX2;
			//secondBall.speed.y = newVelY2;
			o2->setSpeed(Vector2(newVelX2, newVelY2));

			//firstBall.x = firstBall.x + newVelX1;
			o1->setModel(o1->model() * Create4DTranslation(o1->speed().x, 0, 0));
			o1->setModel(o1->model() * Create4DTranslation(o1->speed().x, 0, 0));
			//firstBall.y = firstBall.y + newVelY1;
			o1->setModel(o1->model() * Create4DTranslation(0, o1->speed().y, 0));
			o1->setModel(o1->model() * Create4DTranslation(0, o1->speed().y, 0));
			//secondBall.x = secondBall.x + newVelX2;
			o2->setModel(o2->model() * Create4DTranslation(o2->speed().x, 0, 0));
			o2->setModel(o2->model() * Create4DTranslation(o2->speed().x, 0, 0));
			//secondBall.y = secondBall.y + newVelY2;
			o2->setModel(o2->model() * Create4DTranslation(0, o2->speed().y, 0));
			o2->setModel(o2->model() * Create4DTranslation(0, o2->speed().y, 0));

		}

		void updatePosition(int timeElapsed)
		{
			_speed = _speed + _acceleration;
			_model = _model * Create4DTranslation(_speed.x, _speed.y, 0);

			_speed.x = _speed.x > 0 ? _speed.x - friction : _speed.x + friction;
			_speed.y = _speed.y > 0 ? _speed.y - friction : _speed.y + friction;

			_acceleration.x = _acceleration.x > 0 ? _acceleration.x - friction : _acceleration.x + friction;
			_acceleration.y = _acceleration.y > 0 ? _acceleration.y - friction : _acceleration.y + friction;

			if (fabs(_speed.x) < 0.001)
				_speed.x = 0;

			if (fabs(_speed.y) < 0.001)
				_speed.y = 0;

			if (fabs(_acceleration.x) < 0.001)
				_acceleration.x = 0;

			if (fabs(_acceleration.y) < 0.001)
				_acceleration.y = 0;

			Quaternion rotationQtrnY = Quaternion(_speed.x * 100, math::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
			Quaternion rotationQtrnX = Quaternion(-_speed.y * 100, math::Vector4(1.0f, 0.0f, 0.0f, 1.0f));

			rotationQuaternion = rotationQtrnX * rotationQtrnY * rotationQuaternion;
		}
	};
}

#endif;