#ifndef __COMPONENT_PHYSICS_HPP__
#define __COMPONENT_PHYSICS_HPP__

#include "Physics/PhysicsHandle.hpp"

namespace reactphysics3d
{
	class RigidBody;
	class Collider;
	//class Transform;
}

namespace Cookie
{
	namespace Core::Math
	{
		union Vec2;
		union Vec3;
	}

    namespace ECS
    {

		class ComponentTransform;


		/* Component Of the ECS, serve as a wrapper to the method of reactphysics3D.
		 * reactphysics3D also have its own ECS, so we make at creation a RigidBody 
		 * by component we have and desactivate them.
		 */
		class ComponentPhysics
		{
			public:
				/* The RigidBody of the component, it also countains a pointer to this component */
				::reactphysics3d::RigidBody*				physBody = nullptr;
				/* The colliders, only cube, sphere and capsule are supported */
				std::vector<::reactphysics3d::Collider*>	physColliders;

				/* physTransform is the transform in which we recover the transform computed by reactPhysics3D ... */
				::reactphysics3d::Transform					physTransform;

				/* ... and this one is to interpolate to have a smooth movement 
				 * that will work the same on every hardware */
				::reactphysics3d::Transform					oldTransform;

			public:
				ComponentPhysics();
				~ComponentPhysics();


			void AddSphereCollider	(float radius,							const Core::Math::Vec3& localPos, const Core::Math::Vec3& eulerAngles);
			void AddCubeCollider	(const Core::Math::Vec3& halfExtent,	const Core::Math::Vec3& localPos, const Core::Math::Vec3& eulerAngles);
			void AddCapsuleCollider	(const Core::Math::Vec2& capsuleInfo,	const Core::Math::Vec3& localPos, const Core::Math::Vec3& eulerAngles);
			void RemoveCollider		(::reactphysics3d::Collider* collider);

			void Update()noexcept;
			void Set(const ComponentTransform& trs);

			inline void ToDefault()
			{
				/* remove all collliders */
				for (int i = 0; i < physColliders.size(); i++)
				{
					RemoveCollider(physColliders[i]);
				}

				/* actually does not reset settings, just desactivate it */
				physBody->setIsActive(false);

				std::vector<reactphysics3d::Collider*>().swap(physColliders);
			}
		};

    }
}

#endif // !__COMPONENT_PHYSICS_HPP__
