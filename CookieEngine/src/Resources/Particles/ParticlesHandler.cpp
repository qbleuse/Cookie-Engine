#include "ParticlesHandler.hpp"
#include "Camera.hpp"
#include "DrawDataHandler.hpp"
#include "ResourcesManager.hpp"

using namespace Cookie::Resources::Particles;

std::list<ParticlesSystem> ParticlesHandler::particlesSystems;
unsigned int ParticlesHandler::living;
std::unordered_map<std::string, std::unique_ptr<ParticlesPrefab>>* ParticlesHandler::particlesPrefab;
Cookie::Render::ParticlesPass ParticlesHandler::shader;

constexpr float cullEpsilon = -3.0f;

bool ParticlesHandler::TestFrustrum(Render::Frustrum& frustrum, Cookie::Core::Math::Vec4& pos)
{
	for (int j = 0; j < frustrum.planes.size(); j++)
	{
		if ((frustrum.planes[j].Dot(pos) + frustrum.planes[j].w) < cullEpsilon)
			return true;
	}
	return false;
}

void ParticlesHandler::Update()
{
	for (std::list<ParticlesSystem>::iterator particles = particlesSystems.begin(); particles != particlesSystems.end(); particles++)
	{
		(*particles).Update();
		if ((*particles).needToBeRemoved)
		{
			particles = particlesSystems.erase(particles);
			particlesSystems.push_back(ParticlesSystem());
			living--;
		}
	}	
}

void ParticlesHandler::Draw(const Render::Camera& cam)
{
	frustrum.MakeFrustrum(cam);
	for (std::list<ParticlesSystem>::iterator particles = particlesSystems.begin(); particles != particlesSystems.end(); particles++)
		(*particles).Draw(cam, frustrum);
}

void ParticlesHandler::CreateParticlesWithPrefab(const Cookie::Core::Math::Vec3& pos, const std::string& namePref, const Cookie::Core::Math::Vec3& posSpawnEnd)
{
	CreateParticlesWithPrefab(pos, (*particlesPrefab)[namePref].get(), posSpawnEnd);
}

void ParticlesHandler::CreateParticlesWithPrefab(const Cookie::Core::Math::Vec3& pos, ParticlesPrefab* particlesPrefab, const Cookie::Core::Math::Vec3& posSpawnEnd)
{
	if (living + 1 >= particlesSystems.size() || !particlesPrefab)
		return;
	ParticlesSystem& particles = *std::next(particlesSystems.begin(), living);

	ParticlesPrefab* prefab = particlesPrefab;

	particles.name = prefab->name;
	particles.data.resize(prefab->data.size());
	particles.trs = Cookie::Core::Math::Mat4::Translate(pos);
	for (int i = 0; i < prefab->data.size(); i++)
	{
		particles.data[i].generate(prefab->data[i].size, prefab->data[i].countFrame);
		particles.data[i].wake(0, prefab->data[i].countAlive);
		particles.data[i].mesh = prefab->data[i].mesh;
		particles.data[i].texture = prefab->data[i].texture;
		particles.data[i].countFrame = prefab->data[i].countFrame;
		particles.data[i].countAlive = prefab->data[i].countAlive;
		particles.data[i].SetIsBIllboard(prefab->data[i].isBillboard);	
	}

	particles.particlesEmiter = prefab->emitter;

	std::string name;
	for (int i = 0; i < prefab->emit.size(); i++)
	{
		for (int j = 0; j < prefab->emit[i].size(); j++)
		{
			name = prefab->emit[i][j].name;
			if (name == "PointPositionGen")
			{
				std::shared_ptr<PointPositionGenerate> point = std::make_unique<PointPositionGenerate>();
				point.get()->pos = prefab->emit[i][j].data[0];
				point.get()->trs = &particles.trs;
				particles.particlesEmiter[i].generators.push_back(std::move(point));
				free(point.get());
			}
			else if (name == "Loop")
			{
				std::shared_ptr<Loop> loop = std::make_unique<Loop>(particles.particlesEmiter[i].generators);
				particles.particlesEmiter[i].updates.push_back(std::move(loop));
				free(loop.get());
			}
			else if (name == "BoxPositionGen")
			{
				std::shared_ptr<BoxPositionGenerate> box = std::make_unique<BoxPositionGenerate>();
				box.get()->pos = prefab->emit[i][j].data[0];
				box.get()->sizeBox = prefab->emit[i][j].data[1];
				box.get()->trs = &particles.trs;
				particles.particlesEmiter[i].generators.push_back(std::move(box));
				free(box.get());
			}
			else if (name == "CirclePositionGen")
			{
				std::shared_ptr<SpherePositionGenerate> sphere = std::make_unique<SpherePositionGenerate>();
				sphere.get()->pos = prefab->emit[i][j].data[0];
				sphere.get()->radius = prefab->emit[i][j].data[1].x;
				sphere.get()->trs = &particles.trs;
				particles.particlesEmiter[i].generators.push_back(std::move(sphere));
				free(sphere.get());
			}
			else if (name == "CreateParticles")
			{
				if (particles.data.size() > prefab->emit[i][j].data[0].x && prefab->emit[i][j].data[0].x >= 0)
				{
					std::shared_ptr<CreateParticlesFollowing> create = std::make_unique<CreateParticlesFollowing>(particles.data[prefab->emit[i][j].data[0].x]);
					particles.data[prefab->emit[i][j].data[0].x].canRemoved = false;
					create.get()->coeffScale = prefab->emit[i][j].data[1].x;
					create.get()->coeffPos = prefab->emit[i][j].data[1].y;
					create.get()->time = prefab->emit[i][j].data[1].z;
					particles.particlesEmiter[i].updates.push_back(std::move(create));
					free(create.get());
				}
			}
			else if (name == "Shadow")
			{
				if (particles.data.size() > prefab->emit[i][j].data[0].x && prefab->emit[i][j].data[0].x >= 0)
				{
					std::shared_ptr<Shadow> shadow = std::make_unique<Shadow>(particles.data[prefab->emit[i][j].data[0].x]);
					particles.data[prefab->emit[i][j].data[0].x].canRemoved = false;
					shadow.get()->time = prefab->emit[i][j].data[0].y;
					particles.particlesEmiter[i].updates.push_back(std::move(shadow));
					free(shadow.get());
				}
			}
			else if (name == "SpawnEnd")
			{
				std::shared_ptr<SpawnEnd> plane = std::make_unique<SpawnEnd>();
				plane.get()->namePrefab = prefab->emit[i][j].nameData;
				plane.get()->posSpawn = posSpawnEnd;
				particles.particlesEmiter[i].updates.push_back(std::move(plane));
				free(plane.get());
			}
			else if (name == "InitVelWithPoint")
			{
				std::shared_ptr<InitVelocityWithPoint> vel = std::make_unique<InitVelocityWithPoint>();
				vel.get()->endPoint = posSpawnEnd;
				particles.particlesEmiter[i].generators.push_back(std::move(vel));
				free(vel.get());
			}
			else if (name == "StopAt")
			{
				std::shared_ptr<StopAt> stop = std::make_unique<StopAt>();
				stop.get()->posEnd = posSpawnEnd;
				particles.particlesEmiter[i].updates.push_back(std::move(stop));
				free(stop.get());
			}
		}
	}

	particles.needToBeRemoved = false;
	particles.generate();
	living++;
}