#include "Physics/PhysicsHandle.hpp"
#include "ECS/ComponentTransform.hpp"
#include "ECS/ComponentPhysics.hpp"

using namespace reactphysics3d;
using namespace Cookie::Core::Math;
using namespace Cookie::ECS;

/*============================ CONSTRUCTORS ============================*/

ComponentPhysics::ComponentPhysics()
{
	/* create the physBody and put pointer to our component */
	physBody = Physics::PhysicsHandle::physSim.worldSim->createRigidBody(physTransform);
	physBody->setUserData(this);
}

ComponentPhysics::~ComponentPhysics()
{
}

/*============================ INIT METHODS ============================*/

void ComponentPhysics::AddSphereCollider(float radius, const Vec3& localPos, const Vec3& eulerAngles)
{
	SphereShape* sphere = Physics::PhysicsHandle().physCom->createSphereShape(radius);

	Transform trs;

	trs.setPosition({ localPos.x,localPos.y,localPos.z });
	Quat quat = Quat::ToQuat(eulerAngles);
	trs.setOrientation({ quat.x,quat.y,quat.z,quat.w });

	physColliders.push_back(physBody->addCollider(sphere, trs));
}

void ComponentPhysics::AddCubeCollider(const Vec3& halfExtent, const Vec3& localPos, const Vec3& eulerAngles)
{
	BoxShape* cube = Physics::PhysicsHandle().physCom->createBoxShape({ halfExtent.x,halfExtent.y,halfExtent.z });
	
	Transform trs;

	trs.setPosition({ localPos.x,localPos.y,localPos.z });
	Quat quat = Quat::ToQuat(eulerAngles);
	trs.setOrientation({ quat.x,quat.y,quat.z,quat.w });

	physColliders.push_back(physBody->addCollider(cube, trs));
}

void ComponentPhysics::AddCapsuleCollider(const Vec2& capsuleInfo, const Vec3& localPos, const Vec3& eulerAngles)
{
	CapsuleShape* capsule = Physics::PhysicsHandle().physCom->createCapsuleShape(capsuleInfo.x, capsuleInfo.y);

	Transform trs;

	trs.setPosition({ localPos.x,localPos.y,localPos.z });
	Core::Math::Quat quat = Core::Math::Quat::ToQuat(eulerAngles);
	trs.setOrientation({ quat.x,quat.y,quat.z,quat.w });

	physColliders.push_back(physBody->addCollider(capsule, trs));
}

void ComponentPhysics::RemoveCollider(::reactphysics3d::Collider* collider)
{
	physBody->removeCollider(collider);
}

/*============================ REALTIME METHODS ============================*/

void ComponentPhysics::Set(const ComponentTransform& trs)
{

	physTransform.setPosition({ trs.pos.x,trs.pos.y,trs.pos.z });
	Core::Math::Quat quat = Core::Math::Quat::ToQuat(trs.rot);
	physTransform.setOrientation({ quat.x,quat.y,quat.z,quat.w });	

	physBody->setTransform(physTransform);
}

void ComponentPhysics::Update()noexcept
{
	/* get the transform then normalize with the time step (initial one is 1/60) */
	physTransform = physBody->getTransform();
	physTransform = reactphysics3d::Transform::interpolateTransforms(oldTransform, physTransform, Physics::PhysicsHandle::physSim.factor);

	oldTransform = physTransform;
}