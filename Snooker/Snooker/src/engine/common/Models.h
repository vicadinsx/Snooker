#pragma once
#ifndef __ModelsManager_H__
#define __ModelsManager_H__

#include <vector>
#include <string>
#include <map>
#include "types\Object.h"
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

		static ModelsManager* instance()
		{
			static ModelsManager instance;
			return &instance;
		}

	};

}

#endif;