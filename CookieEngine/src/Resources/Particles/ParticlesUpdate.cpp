#include "ParticlesUpdate.hpp"
#include "ParticlesData.hpp"
#include "ParticlesHandler.hpp"
#include "Serialization.hpp"
#include "Time.hpp"
#include "Mat4.hpp"

using namespace Cookie::Resources::Particles;

std::string Cookie::Resources::Particles::GetName(const TYPEUP& type)
{
	switch (type)
	{
	case (UPDATEVEL):
		return "UpdateVelocity";
	case (UPDATESCALE):
		return "UpdateScale";
	case (UPDATEALPHA):
		return "UpdateAlpha";
	case (COLOROVERLIFE):
		return "ColorOverLife";
	case (ENABLEGRAVITY):
		return "EnableGravity";
	case (UPDATETIME):
		return "UpdateTime";
	case (LOOP):
		return "Loop";
	case (COLLISIONWITHPLANE):
		return "CollisionWithPlane";
	case (CREATEPARTICLES):
		return "CreateParticles";
	case (SHADOW):
		return "Shadow";
	case (SPAWNEND):
		return "SpawnEnd";
	case (STOPAT):
		return "StopAt";
	}
}

void UpdateVelocity::Update(ParticlesData* p)
{
	for (int i = 0; i < p->countAlive; i++)
		p->data[i].pos += p->data[i].vel * Cookie::Core::DeltaTime();	
}

void UpdateScale::Update(ParticlesData* p)
{
	for (int i = 0; i < p->countAlive; i++)
	{
		p->data[i].scale.x = Cookie::Core::Math::Lerp(scaleEnd.x, p->data[i].scaleBegin.x, p->data[i].time / p->data[i].timeMax);
		p->data[i].scale.y = Cookie::Core::Math::Lerp(scaleEnd.y, p->data[i].scaleBegin.y, p->data[i].time / p->data[i].timeMax);
		p->data[i].scale.z = Cookie::Core::Math::Lerp(scaleEnd.z, p->data[i].scaleBegin.z, p->data[i].time / p->data[i].timeMax);
	}
}

void UpdateAlpha::Update(ParticlesData* p)
{
	for (int i = 0; i < p->countAlive; i++)
		p->data[i].col.a = Cookie::Core::Math::Lerp(alphaEnd, p->data[i].colBegin.a, p->data[i].time / p->data[i].timeMax);
}

void ColorOverLife::Update(ParticlesData* p)
{
	for (int i = 0; i < p->countAlive; i++)
	{
		p->data[i].col.x = Cookie::Core::Math::Lerp(colorEnd.x, p->data[i].colBegin.x, p->data[i].time / p->data[i].timeMax);
		p->data[i].col.y = Cookie::Core::Math::Lerp(colorEnd.y, p->data[i].colBegin.y, p->data[i].time / p->data[i].timeMax);
		p->data[i].col.z = Cookie::Core::Math::Lerp(colorEnd.z, p->data[i].colBegin.z, p->data[i].time / p->data[i].timeMax);
		p->data[i].col.a = Cookie::Core::Math::Lerp(colorEnd.a, p->data[i].colBegin.a, p->data[i].time / p->data[i].timeMax);
	}
}

void EnabledGravity::Update(ParticlesData* p)
{
	for (int i = 0; i < p->countAlive; i++)
		p->data[i].vel += Cookie::Core::Math::Vec3(0, gravity * p->data[i].mass, 0) * Cookie::Core::DeltaTime();	
}

void UpdateTime::Update(ParticlesData* p)
{
	for (int i = 0; i < p->countAlive; i++)
	{
		p->data[i].time -= Cookie::Core::DeltaTime();
		if (p->data[i].time < 0)
			p->kill(i);
	}
}

void Loop::Update(ParticlesData* p)
{
	if (p->countAlive < p->countFrame)
	{
		float countAlive = p->countAlive;
		float countFrame = p->countFrame;

		p->wake(countAlive, countFrame);

		for (int i = 0; i < particlesGen->size(); i++)
			(*particlesGen)[i]->generate(p, countAlive, countFrame + 1);
	}
}

void CollisionWithPlane::Update(ParticlesData* p)
{
	for (int i = 0; i < p->countAlive; i++)
	{
		Cookie::Core::Math::Vec3 pos(p->data[i].pos.x, p->data[i].pos.y, p->data[i].pos.z);
		float distance = (pos.Dot(n) + dis) / n.Length();

		if (distance <= Cookie::Core::Math::EPSILON)
		{
			p->countAlive--;
			ParticlesHandler::CreateParticlesWithPrefab(p->data[i].pos, namePrefab);
		}
	}
}

void CreateParticlesFollowing::Update(ParticlesData* p)
{
	for (int i = 0; i < p->countAlive; i++)
	{
		if (data->countAlive < data->data.size())
		{
			data->data[data->countAlive].alive = true;
			data->data[data->countAlive].pos = p->data[i].pos - p->data[i].vel * coeffPos * Cookie::Core::DeltaTime();
			data->data[data->countAlive].time = time;
			data->data[data->countAlive].timeMax = time;
			data->data[data->countAlive].scale = p->data[i].scale * coeffScale;
			data->data[data->countAlive].col = p->data[i].colBegin;
			data->data[data->countAlive].colBegin = Cookie::Core::Math::Vec4(1, 1, 1, 1);
			data->countAlive ++;
		}
	}
}

void Shadow::Update(ParticlesData* p)
{
	for (int i = 0; i < p->countAlive; i++)
	{
		if (i < data->data.size())
		{
			data->data[i].alive = true;
			data->data[i].pos = p->data[i].pos;
			data->data[i].pos.y = 0.90f;
			data->data[i].rot = Cookie::Core::Math::Vec3(Core::Math::PI / 2, 0, 0);
			data->data[i].time = time;
			data->data[i].timeMax = time;
			data->data[i].scale = p->data[i].scale;
			data->data[i].col = Cookie::Core::Math::Vec4(1, 1, 1, 1);
			data->data[i].colBegin = Cookie::Core::Math::Vec4(1, 1, 1, 1);
		}
	}
}

void SpawnEnd::Update(ParticlesData* p)
{
	for (int i = 0; i < p->countAlive; i++)
	{
		if (p->data[i].time <= Cookie::Core::DeltaTime() * 2)
		{
			p->countAlive--;
			ParticlesHandler::CreateParticlesWithPrefab(posSpawn, namePrefab);
		}
	}
}

void StopAt::Update(ParticlesData* p)
{
	for (int i = 0; i < p->countAlive; i++)
	{
		Cookie::Core::Math::Vec3 pos(p->data[i].pos.x, p->data[i].pos.y, p->data[i].pos.z);
		float distance = (posEnd - pos).Length();
		if (abs(distance) <= 0.9f)
			p->countAlive--;
	}
}