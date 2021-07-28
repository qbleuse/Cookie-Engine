#include "ECS/ComponentScript.hpp"

using namespace Cookie::ECS;

void ComponentScript::ToDefault()
{
	scripts.clear();
}

void ComponentScript::Start()
{
	for (int i = 0; i < scripts.size(); ++i)
		scripts[i].Start();
}
void ComponentScript::Update()
{
	for (int i = 0; i < scripts.size(); ++i)
		scripts[i].Update();
}		