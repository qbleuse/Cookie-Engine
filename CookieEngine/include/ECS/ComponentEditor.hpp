#ifndef __COMPONENT_EDITOR_HPP__
#define __COMPONENT_EDITOR_HPP__

namespace reactphysics3d
{
	class CollisionBody;
	class Collider;
}

namespace Cookie
{
	namespace ECS
	{
		#define EDIT_OFFSET 0.0001f 

		class ComponentTransform;

		class ComponentEditor
		{
			public:
			ComponentEditor();
			~ComponentEditor();
			

			const ComponentTransform*			editTrs { nullptr };
			reactphysics3d::CollisionBody*	body	{ nullptr };
			reactphysics3d::Collider*		collider{ nullptr };
			Core::Math::Vec3				AABBMin	{ 0.0f,0.0f,0.0f };
			Core::Math::Vec3				AABBMax { 0.0f,0.0f,0.0f };

			void InitComponent(const ComponentTransform& trs);

			void Update();

			void MakeCollider();
		};
	}
}

#endif // !__COMPONENT_EDITOR_HPP__
