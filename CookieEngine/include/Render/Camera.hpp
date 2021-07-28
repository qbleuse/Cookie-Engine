#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "Core/Math/Mat4.hpp"

namespace Cookie
{
	namespace Render
	{
		#define CAM_MOUSE_SPEED 50.f
		#define CAM_MOUSE_SPEED_UP_SCALE 10.f
		#define CAM_MOUSE_SENSITIVITY_X 0.0007f
		#define CAM_MOUSE_SENSITIVITY_Y 0.0007f

		#define CAMERA_INITIAL_NEAR 0.01f
		#define CAMERA_INITIAL_FAR  1000.f

		class Camera
		{
			private:
				Core::Math::Mat4 projMat = Core::Math::Mat4::Identity();

			protected:
				Core::Math::Mat4 rotMat = Core::Math::Mat4::Identity();
				Core::Math::Mat4 viewMat = Core::Math::Mat4::Identity();

				bool activated = true;

				float previousMouseX{ 0.0 };
				float previousMouseY{ 0.0 };

			public:
				//because of windows "max" def
				#undef max 
				Core::Math::Vec2 mapClampX = {{ -std::numeric_limits<float>().max(),std::numeric_limits<float>().max() }};
				Core::Math::Vec2 mapClampZ = {{ -std::numeric_limits<float>().max(),std::numeric_limits<float>().max() }};
				Core::Math::Vec2 ZoomClamp = { { -std::numeric_limits<float>().max(),std::numeric_limits<float>().max() } };


				float camNear	= 0.0f;
				float camFar	= 0.0f;
				float fov		= 0.0f;
				float aspectRatio = 0.0f;

				float width = 0.0f;
				float height = 0.0f;

				Core::Math::Vec2 windowOffset = { {0.0f,0.0f} };

				Core::Math::Vec3 pos = {0.0f,0.0f,0.0f};
				Core::Math::Vec3 rot = {0.0f,0.0f,0.0f};
				
			public:
				Camera() {}
				virtual ~Camera() {}

				inline const Core::Math::Mat4& GetView() const	{ return viewMat;			}
				inline const Core::Math::Mat4& GetProj() const	{ return projMat;			}
				inline Core::Math::Mat4 GetViewProj() const		{ return viewMat * projMat;	}
				
				inline void SetProj(float yFov, float _width, float _height, float n, float f) { fov = yFov; width = _width; height = _height; camFar = f; camNear = n;  aspectRatio = width / height; projMat = Core::Math::Mat4::Perspective(Core::Math::ToRadians(yFov), width / height, n, f); }
				inline void SetOrthoProj(float left, float right, float bottom, float top, float n, float f) { width = right - left; height = top - bottom; camFar = f; camNear = n;  aspectRatio = width / height; projMat = Core::Math::Mat4::Ortho(left,right,bottom, top, n, f); }
				inline void LookAt(const Core::Math::Vec3& toLook) { viewMat = Core::Math::Mat4::LookAt(pos, toLook, { 0.0f,1.0f,0.0f });}

				inline virtual void Update() = 0;
				inline void ForceUpdate();

				inline void Activate()noexcept { activated = true; }
				inline void Deactivate()noexcept { activated = false; }

				inline virtual void ResetPreviousMousePos();

				inline Core::Math::Vec3 MouseToWorldDir()const;
				inline Core::Math::Vec3 MouseToWorldDirClamp()const;

				/* a screen point between -1.0 and 1.0 on x and y coordinates */
				inline Core::Math::Vec3 ScreenPointToWorldDir(const Core::Math::Vec2& point)const;
		};

		class FreeFlyCam : public Camera
		{
			private:


			public:
				FreeFlyCam() {}
				virtual ~FreeFlyCam() {}

			inline void UpdateFreeFlyPos();
			inline void UpdateFreeFlyRot();
			inline void Update() override;

		};

		#define GAME_CAM_LOWER_EPSILON -0.98f
		#define GAME_CAM_HIGHER_EPSILON 0.98f

		class GameCam : public Camera
		{

		public:

			GameCam() {}
			virtual ~GameCam() {}

			inline void UpdateGamePos();
			inline void UpdateZoom();
			inline void Update() override;
		};
	}
}

#include "Camera.inl"

#endif // __CAMERA_HPP__
