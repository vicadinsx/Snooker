#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "matrix.h"

namespace math {

	class Object {
		Matrix4 _model;
		float _radius;
		Vector2 _direction;
		Vector2 _speed;
		Vector2 _acceleration;

	public:
		Object(){}
		Object(Matrix4 m, float r, Vector2 d, Vector2 s, Vector2 a) {
			_model = m;
			_radius = r;
			_direction = d;
			_speed = s;
			_acceleration = a;
		}
		~Object(){}
		Matrix4 model() { return this->_model; }
		float radius() { return this->_radius; }
		Vector2 direction() { return this->_direction; }
		Vector2 speed() { return this->_speed; }
		Vector2 acceleration() { return this->_acceleration; }

		void setModel(Matrix4 m) { _model = m; }
		void setRadius(float r) { _radius = r; }
		void setDirection(Vector2 d) { _direction = d; }
		void setSpeed(Vector2 s) { _speed = s; }
		void setAcceleration(Vector2 a) { _acceleration = a; }

		void update(int timeElapsed)
		{
			_speed = _speed + _acceleration;
			_model = _model * Create4DTranslation(_speed.x, _speed.y, 0);
		}
	};
}

#endif;