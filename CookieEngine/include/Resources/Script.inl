#ifndef __SCRIPT_INL__
#define __SCRIPT_INL__


namespace Cookie
{
	namespace Resources
	{
		
		inline ScriptObject Script::CreateObject(std::string entityId)
		{
			return ScriptObject(*this, entityId);
		}
		inline bool Script::isUpToDate()
		{
			return (lastUpdateTime == std::filesystem::last_write_time(filename));
		}
		inline void Script::UpdateContent()
		{
			std::cout << "Updating Script\n";
			lastUpdateTime = std::filesystem::last_write_time(filename);


			state.script_file(filename);
			construct = state["Construct"];
			start = state["Start"];
			update = state["Update"];

			for (int i = 0; i < ScriptObjectsChild.size(); ++i)
			{
				//ScriptObjectsChild[i]->table.clear();
				construct(ScriptObjectsChild[i]->table);
			}
		}


		inline void ScriptObject::Start() const
		{
			script->start(table);
		}
		inline void ScriptObject::Update() const
		{
			script->update(table);
		}

	}
}



#endif