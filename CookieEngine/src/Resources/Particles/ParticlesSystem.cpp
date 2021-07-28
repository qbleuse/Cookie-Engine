#include "ParticlesHandler.hpp"

using namespace Cookie::Resources::Particles;

void ParticlesSystem::generate()
{
	for (int j = 0; j < particlesEmiter.size(); j++)
		for (int i = 0; i < particlesEmiter[j].generators.size(); i++)
			particlesEmiter[j].generators[i]->generate(&data[j], 0, data[j].countAlive);
}

void ParticlesSystem::Update()
{
	needToBeRemoved = false;
	for (int j = 0; j < data.size(); j++)
	{
		if (particlesEmiter.size() > j)
		{
			for (int k = 0; k < particlesEmiter[j].updates.size(); k++)
				particlesEmiter[j].updates[k]->Update(&data[j]);
		}
		if (data[j].countAlive <= 0 && data[j].canRemoved)
			needToBeRemoved = true;
		else if (!data[j].canRemoved && needToBeRemoved && data[j].countAlive <= 0)
			needToBeRemoved = true;
		else
			needToBeRemoved = false;
	}

	if (needToBeRemoved)
	{
		data.clear();
		particlesEmiter.clear();
	}
}

void ParticlesSystem::Draw(const Render::Camera& cam, Render::Frustrum& frustrum)
{
	for (int j = 0; j < data.size(); j++)
	{
		std::vector<Cookie::Render::InstancedData> newData;
		for (int i = 0; i < data[j].countAlive; i++)
		{
			Cookie::Core::Math::Vec4 pos = Cookie::Core::Math::Vec4(data[j].data[i].pos, 1);

			if (ParticlesHandler::TestFrustrum(frustrum, pos))
				continue;

			Cookie::Render::InstancedData temp;
			temp.World = Cookie::Core::Math::Mat4::TRS(data[j].data[i].pos, data[j].data[i].rot, data[j].data[i].scale);
			temp.Color = data[j].data[i].col;
			temp.isBillboard = data[j].data[i].isBillboard;
			newData.push_back(temp);
		}

		if (newData.size() > 0)
			ParticlesHandler::shader.Draw(cam, data[j].mesh, data[j].texture, newData);
	}
}