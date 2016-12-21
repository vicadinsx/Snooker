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
		float friction = 0.0002f;
		Quaternion rotationQuaternion = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

	public:
		Object() {}
		Object(Matrix4 m, float r, Vector2 d, Vector2 s, Vector2 a, float mass) {
			_model = m;
			_radius = 0.32f;
			_direction = d;
			_speed = s;
			_acceleration = a;
			_mass = mass;
		}
		~Object() {}

		Matrix4 model() { return this->_model; }
		Matrix4 modelMatrix() { return this->_model*this->rotationQuaternion.toMatrix(); }
		Vector2 direction() { return this->_direction; }
		Vector2 speed() { return this->_speed; }
		Vector2 acceleration() { return this->_acceleration; }
		Vector2 pos() { return Vector2(this->_model.getElement(0, 3), this->_model.getElement(1, 3)); }
		float radius() { return this->_radius; }
		float mass() { return _mass; }
		float posX() { return this->_model.getElement(0, 3); }
		float posY() { return this->_model.getElement(1, 3); }

		void setModel(Matrix4 m) { _model = m; }
		void setRadius(float r) { _radius = r; }
		void setDirection(Vector2 d) { _direction = d; }
		void setSpeed(Vector2 s) { _speed = s; }
		void setAcceleration(Vector2 a) { _acceleration = a; }
		void setMass(float m) { _mass = m; }

		//AABB overlaps method for balls
		friend bool overlaps(Object *o1, Object *o2) {
			if (o1->posX() + o1->radius() + o2->radius() > o2->posX() &&
				o1->posX() < o1->radius() + o2->radius() + o2->posX() &&
				o1->posY() + o1->radius() + o2->radius() > o2->posY() &&
				o1->posY() < o1->radius() + o2->radius() + o2->posY())
				return true;

			return false;
		}

		//Check if balls are colliding
		friend bool colliding(Object *o1, Object *o2) {

			Vector2 collisionv1(o1->pos()), collisionv2(o2->pos());
			Vector2 collision = collisionv1 - collisionv2;
			float distance = collision.norm();

			if (distance < o1->radius() + o2->radius())
				return true;

			return false;
		}

		//Return collision x and y position
		friend Vector2 collisionPoints(Object* o1, Object *o2) {
			float collisionPointX = ((o1->posX()*o2->radius()) + o2->posX()*o1->radius()) / (o1->radius() + o2->radius());
			float collisionPointY = ((o1->posY()*o2->radius()) + o2->posY()*o1->radius()) / (o1->radius() + o2->radius());
			return{ collisionPointX, collisionPointY };
		}

		friend void collide(Object* o1, Object* o2) {
			collide_1(o1, o2);
		}

		// "Light" colisions
		static void collide_1(Object* o1, Object* o2) {
			float v1x(o1->posX()), v1y(o1->posY()), v2x(o2->posX()), v2y(o2->posY()),
				disp_X, disp_Y, distance;

			Vector2 collisionv1(Vector2(v1x, v1y)), collisionv2(Vector2(v2x, v2y));
			Vector2 collision = collisionv1 - collisionv2;
			distance = collision.norm();
			collision = collision / distance;

			// store displacement to push them appart
			float displacement = (o1->radius() + o2->radius() - distance);
			disp_X = (v1x - v2x < 0 ? displacement : -displacement);
			disp_Y = (v1y - v2y < 0 ? displacement : -displacement);

			float aci = dot(o1->speed(), collision);
			float bci = dot(o2->speed(), collision);

			// 1-dimensional elastic collision equations.
			float acf = bci;
			float bcf = aci;

			// Update collision velocity components
			o1->setSpeed(o1->speed() + (collision * (acf - aci)));
			o2->setSpeed(o2->speed() + (collision * (bcf - bci)));

			// Push them apart
			o1->setModel(o1->model() * Create4DTranslation(o1->speed().x - disp_X, o1->speed().y - disp_Y, 0));
			o2->setModel(o2->model() * Create4DTranslation(o2->speed().x + disp_X, o2->speed().y + disp_Y, 0));
		}

		// "Heavy" collisions
		static void collide_2(Object* o1, Object* o2) {

			Vector2 U1x, U1y, U2x, U2y, V1x, V1y, V2x, V2y, v1temp, v1, v2, v1x, v2x, v1y, v2y, x(o1->pos() - o2->pos());
			float m1, m2, x1, x2, disp_X, disp_Y, distance(x.norm());

			// store displacement to push them appart
			float displacement = (o1->radius() + o2->radius() - distance);
			disp_X = (x.x < 0 ? displacement : -displacement);
			disp_Y = (x.y < 0 ? displacement : -displacement);

			x.normalized();
			v1 = o1->speed();
			x1 = dot(x, v1);
			v1x = x * x1;
			v1y = v1 - v1x;
			m1 = o1->mass();

			x = x * -1;
			v2 = o2->speed();
			x2 = dot(x, v2);
			v2x = x * x2;
			v2y = v2 - v2x;
			m2 = o2->mass();

			// Update collision velocity components
			o1->setSpeed(v1x*(m1 - m2) / (m1 + m2) + v2x*(2 * m2) / (m1 + m2) + v1y);
			o2->setSpeed(v1x*(2 * m1) / (m1 + m2) + v2x*(m2 - m1) / (m1 + m2) + v2y);

			// Push them apart
			o1->setModel(o1->model() * Create4DTranslation(o1->speed().x - disp_X, o1->speed().y - disp_Y, 0));
			o2->setModel(o2->model() * Create4DTranslation(o2->speed().x + disp_X, o2->speed().y + disp_Y, 0));
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

		bool isMoving() {
			if (_speed.x == 0.0f && _speed.y == 0.0f) {
				return false;
			}
			else
				return true;
		}

	};
}

#endif;