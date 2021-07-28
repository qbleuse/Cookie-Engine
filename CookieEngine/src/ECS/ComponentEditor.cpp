#include "Physics/PhysicsHandle.hpp"
#include "ECS/ComponentTransform.hpp"
#include "ECS/ComponentEditor.hpp"

using namespace Cookie::ECS;


/*============================ CONSTRUCTORS ============================*/
ComponentEditor::ComponentEditor() 
{
}

ComponentEditor::~ComponentEditor()
{
	//if (body)

}


/*============================ INIT METHODS ============================*/

void ComponentEditor::InitComponent(const ECS::ComponentTransform& trs)
{
	editTrs = &trs;
	reactphysics3d::Transform newTrs;
	newTrs.setPosition({ trs.pos.x,trs.pos.y,trs.pos.z });
	Core::Math::Quat quat = Core::Math::Quat::ToQuat(trs.rot);
	newTrs.setOrientation({ quat.x,quat.y,quat.z,quat.w });

	body = Physics::PhysicsHandle::editWorld->createCollisionBody(newTrs);
}

/*============================ REALTIME METHODS ============================*/

void ComponentEditor::Update()
{
	reactphysics3d::Transform newTrs;
	newTrs.setPosition({ editTrs->pos.x,editTrs->pos.y,editTrs->pos.z });
	Core::Math::Quat quat = Core::Math::Quat::ToQuat(editTrs->rot);
	newTrs.setOrientation({ quat.x,quat.y,quat.z,quat.w });

	if (body)
		body->setTransform(newTrs);
}

void ComponentEditor::MakeCollider()
{
	if (body)
	{
		if (collider)
		{
			body->removeCollider(collider);
			collider = nullptr;
		}

		Core::Math::Vec3 center = (AABBMin + AABBMax) * 0.5f;
		Core::Math::Vec3 AABBhalf = { AABBMax.x - center.x,AABBMax.y - center.y,AABBMax.z - center.z };

		rp3d::BoxShape* box = Physics::PhysicsHandle::physCom->createBoxShape({ std::abs(AABBhalf.x * editTrs->scale.x) + EDIT_OFFSET,std::abs(AABBhalf.y * editTrs->scale.y) + EDIT_OFFSET ,std::abs(AABBhalf.z * editTrs->scale.z) + EDIT_OFFSET });
		collider = body->addCollider(box, rp3d::Transform({ center.x * editTrs->scale.x,center.y * editTrs->scale.y,center.z * editTrs->scale.z }, rp3d::Quaternion::identity()));
	}
}