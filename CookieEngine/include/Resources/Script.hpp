#ifndef __SCRIPT_HPP__
#define __SCRIPT_HPP__

#define SOL_ALL_SAFETIES_ON 1
#include "Sol/sol.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

/*
#ifdef _WIN32
#pragma comment (lib, "liblua54.a")
#endif*/

namespace Cookie
{

	namespace Resources
	{


		class ScriptObject;


		class Script
		{
		public:
			std::string filename;
			std::filesystem::file_time_type lastUpdateTime;

			sol::state state;
			sol::function construct;
			sol::function start;
			sol::function update;

			std::vector<ScriptObject*> ScriptObjectsChild;

			Script(const std::string _filename) : filename(_filename)
			{
				state.open_libraries(sol::lib::base);
				UpdateContent();
			}
			~Script() {}

			inline ScriptObject CreateObject(std::string entityId);
			inline bool isUpToDate();
			inline void UpdateContent();
		};
				
		class ScriptObject
		{
		public:
			Script* script;
			sol::table table;


			ScriptObject(Script& aScript, std::string entityId) : script(&aScript)
			{
				table = script->state[entityId].get_or_create<sol::table>();
				script->construct(table);
				script->ScriptObjectsChild.push_back(this);
			}

			inline void Start() const;
			inline void Update() const;
		};
	}
}

#include "Resources//Script.inl"

#endif // !__SCRIPT_HPP__
