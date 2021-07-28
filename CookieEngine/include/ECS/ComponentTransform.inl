#ifndef __COMPONENT_TRANSFORM_INL__
#define __COMPONENT_TRANSFORM_INL__


namespace Cookie
{
	namespace ECS
	{
		inline ComponentTransform::ComponentTransform() {}
		inline ComponentTransform::ComponentTransform(const Core::Math::Vec3& _translation, const Core::Math::Vec3& _rotation, const Core::Math::Vec3& _scale): pos{ _translation }, rot{ _rotation }, scale{ _scale } {}

		inline void ComponentTransform::ToDefault()noexcept
		{
			radius = 0;

			pos = { 0 ,0 ,0 };
			rot = { 0 ,0 ,0 };
			scale = { 1, 1, 1 };

			TRS = Core::Math::Mat4::Identity();
		}
		inline void ComponentTransform::ComputeRadius()
		{
			if (modelptr && modelptr->mesh)
				radius = Core::Math::Vec2{ { scale.x * std::abs(modelptr->mesh->AABBMin.x), scale.z * std::abs(modelptr->mesh->AABBMin.z) } }.Length();
		}
		inline void ComponentTransform::ComputeTRS()
		{
			TRS = Core::Math::Mat4::TRS(pos, { Core::Math::ToRadians(rot.x),Core::Math::ToRadians(rot.y) ,Core::Math::ToRadians(rot.z) }, scale);
		
			//should be done only when scale or mesh change
			ComputeRadius();
		}

	}
}

#endif