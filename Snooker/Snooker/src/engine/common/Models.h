#ifndef __ModelsManager_H__
#define __ModelsManager_H__

#include <vector>
#include <string>
#include <map>
#include "math\object.h"
#include "visual\scene.h"

namespace engine {

	using math::Object;

	class ModelsManager {
		std::map<std::string, Object*> objects;

		ModelsManager() {}
		~ModelsManager() {}
		ModelsManager(ModelsManager const&);		// Don't Implement.
		void operator=(ModelsManager const&);		// Don't implement

	public:
		Object* get(std::string name) {
			std::map<std::string, Object*>::iterator it;
			for (it = objects.begin(); it != objects.end(); it++) {
				if (it->first.compare(name) == 0)
					return it->second;
			}
			throw "Name does not exist";
		}

		void add(Object* o, std::string name) { objects[name] = o; }

		void updateObjects(int timeElapsed)
		{
			std::map<std::string, Object*>::iterator it;

			for (it = objects.begin(); it != objects.end(); it++) {
				it->second->updatePosition(timeElapsed);
			}


			for (it = objects.begin(); it != objects.end(); it++) {
				calculateCollisions(it->first, it->second);
			}

			for (it = objects.begin(); it != objects.end(); it++) {
				calculateCollisionsWithWall(it->second);
			}
		}

		void calculateCollisionsWithWall(Object *o) {
			Matrix4 m = o->model();
			if (o->model().getElement(0, 3) < -8.1) {
				o->setSpeed(Vector2(-o->speed().x*0.90f, o->speed().y*0.90f));
				o->setAcceleration(Vector2(-o->acceleration().x*0.90f, o->acceleration().y*0.90f));
				m.setElement(0, 3, -8.0f);
			}
			if (o->model().getElement(0, 3) > 7.1) {
				o->setSpeed(Vector2(-o->speed().x*0.90f, o->speed().y*0.90f));
				o->setAcceleration(Vector2(-o->acceleration().x*0.90f, o->acceleration().y*0.90f));
				m.setElement(0, 3, 7.0f);
			}
			if (o->model().getElement(1, 3) < -1.8) {
				o->setSpeed(Vector2(o->speed().x*0.90f, -o->speed().y*0.90f));
				o->setAcceleration(Vector2(o->acceleration().x*0.90f, -o->acceleration().y*0.90f));
				m.setElement(1, 3, -1.7f);
			}
			if (o->model().getElement(1, 3) > 7) {
				o->setSpeed(Vector2(o->speed().x*0.90f, -o->speed().y*0.90f));
				o->setAcceleration(Vector2(o->acceleration().x*0.90f, -o->acceleration().y*0.90f));
				m.setElement(1, 3, 6.9f);
			}

			o->setModel(m * math::Create4DTranslation(o->speed().x, o->speed().y, 0));
		}

		//Calculate collisions
		void calculateCollisions(std::string name, Object *o){
			std::map<std::string, Object*>::iterator it;
			for (it = objects.begin(); it != objects.end(); it++) {
				if (it->first.compare(name) == 0)
				{
					continue;
				}
				//if (overlaps(o, it->second)) {
					if (colliding(o, it->second)) {
						collide(o, it->second);
					}
				//}
			}
		}

		bool isUpdating() {
			std::map<std::string, Object*>::iterator it;
			for (it = objects.begin(); it != objects.end(); it++) {
				if (abs(it->second->speed().x) > 0.0f || abs(it->second->speed().y > 0.0f))
					return true;
			}
			return false;
		}

		static ModelsManager* instance()
		{
			static ModelsManager instance;
			return &instance;
		}

	};

}
#endif;