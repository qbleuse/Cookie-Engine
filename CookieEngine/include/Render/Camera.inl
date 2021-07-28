#ifndef __CAMERA_INL__
#define __CAMERA_INL__

#include "Core/Time.hpp"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <Debug.hpp>

namespace Cookie
{
	namespace Render
	{
		/*======================= CAMERA =======================*/

		inline void Camera::Update()
		{
			rotMat = Core::Math::Mat4::RotateY(rot.y) * Core::Math::Mat4::RotateX(rot.x);
			viewMat = Core::Math::Mat4::Translate(-pos) * rotMat;
		}

		inline void Camera::ForceUpdate()
		{
			rotMat = Core::Math::Mat4::RotateY(rot.y) * Core::Math::Mat4::RotateX(rot.x);
			viewMat = Core::Math::Mat4::Translate(-pos) * rotMat;
		}

		inline void Camera::ResetPreviousMousePos()
		{
			previousMouseX = ImGui::GetIO().MousePos.x;
			previousMouseY = ImGui::GetIO().MousePos.y;
		}					 

		inline Core::Math::Vec3 Camera::MouseToWorldDir()const
		{
			Core::Math::Vec2 mousePos = { { ImGui::GetIO().MousePos.x - windowOffset.x,ImGui::GetIO().MousePos.y - windowOffset.y} };

			Core::Math::Vec2 ratio = { { (mousePos.x / (width * 0.5f)) - 1.0f,  (-mousePos.y / (height * 0.5f)) + 1.0f} };

			Core::Math::Vec4 r = Core::Math::Mat4::Inverse(rotMat * projMat) * Core::Math::Vec4(ratio.x, ratio.y, 1.0f, 1.0f);

			return Core::Math::Vec3({ r.x / r.a,r.y / r.a,r.z / r.a }).Normalize();
		}

		inline Core::Math::Vec3 Camera::MouseToWorldDirClamp()const
		{
			Core::Math::Vec2 mousePos = { { ImGui::GetIO().MousePos.x - windowOffset.x,ImGui::GetIO().MousePos.y - windowOffset.y} };

			Core::Math::Vec2 ratio = { { (mousePos.x / (width * 0.5f)) - 1.0f,  (-mousePos.y / (height * 0.5f)) + 1.0f} };

			if (ratio.x > 1.0f || ratio.x < -1.0f || ratio.y > 1.0f || ratio.y < -1.0f)
				return Core::Math::Vec3();

			Core::Math::Vec4 r = Core::Math::Mat4::Inverse(rotMat * projMat) * Core::Math::Vec4(ratio.x, ratio.y, 1.0f, 1.0f);

			return Core::Math::Vec3({ r.x / r.a,r.y / r.a,r.z / r.a }).Normalize();
		}

		inline Core::Math::Vec3 Camera::ScreenPointToWorldDir(const Core::Math::Vec2& point)const
		{
			Core::Math::Vec4 r = Core::Math::Mat4::Inverse(rotMat * projMat) * Core::Math::Vec4(point.x, point.y, 10.0f, 1.0f);

			return Core::Math::Vec3({ r.x / r.a,r.y / r.a,r.z / r.a }).Normalize();
		}

		/*======================= FREE FLY CAMERA =======================*/


		inline void FreeFlyCam::Update()
		{
			if (activated)
			{
				UpdateFreeFlyPos();
				UpdateFreeFlyRot();
				Camera::Update();
			}
		}

		inline void FreeFlyCam::UpdateFreeFlyPos()
		{
			// Compute speed
			float speed = (CAM_MOUSE_SPEED * Core::UnscaledDeltaTime());
			if (ImGui::GetIO().KeyShift)
				speed *= CAM_MOUSE_SPEED_UP_SCALE;

			// Move forward/backward
			float forwardVelocity = 0.f;
			if (ImGui::GetIO().KeysDown[GLFW_KEY_W]) forwardVelocity -= speed;
			else if (ImGui::GetIO().KeysDown[GLFW_KEY_S]) forwardVelocity += speed;

			// Strafe left/right
			float strafeVelocity = 0.f;
			if (ImGui::GetIO().KeysDown[GLFW_KEY_A])  strafeVelocity -= speed;
			else if (ImGui::GetIO().KeysDown[GLFW_KEY_D]) strafeVelocity += speed;

			pos += Core::Math::Vec3({ viewMat.c[2].x, viewMat.c[2].y, viewMat.c[2].z }) * forwardVelocity;
			pos += Core::Math::Vec3({ viewMat.c[0].x, viewMat.c[0].y, viewMat.c[0].z }) * strafeVelocity;

			// Up movement
			if (ImGui::GetIO().KeysDown[GLFW_KEY_SPACE]) pos.y += speed;
			if (ImGui::GetIO().KeysDown[GLFW_KEY_LEFT_CONTROL]) pos.y -= speed;
		}

		inline void FreeFlyCam::UpdateFreeFlyRot()
		{
			//Calculate DeltaMousePos, later on will put in inputs
			float tempMouseX = ImGui::GetIO().MousePos.x;
			float tempMouseY = ImGui::GetIO().MousePos.y;

			float deltaMouseX = tempMouseX - previousMouseX;
			float deltaMouseY = tempMouseY - previousMouseY;

			previousMouseX = tempMouseX;
			previousMouseY = tempMouseY;

			// Pitch
			rot.x += deltaMouseY * CAM_MOUSE_SENSITIVITY_Y;
			rot.x = std::clamp(rot.x, -Core::Math::TAU / 4.f, Core::Math::TAU / 4.f); // Limit rotation to -90,90 range

			// Yaw
			rot.y += deltaMouseX * CAM_MOUSE_SENSITIVITY_X;
			rot.y = std::fmod(rot.y + Core::Math::TAU + Core::Math::PI, Core::Math::TAU) - Core::Math::PI; // Loop around -180,180
		}


		/* Game Camera */

		inline void GameCam::UpdateGamePos()
		{
			//Calculate DeltaMousePos, later on will put in inputs
			float tempMouseX = ImGui::GetIO().MousePos.x - windowOffset.x;
			float tempMouseY = ImGui::GetIO().MousePos.y - windowOffset.y;
			
			Core::Math::Vec2 ratio = { { (tempMouseX / (width * 0.5f)) - 1.0f,  (-tempMouseY / (height * 0.5f)) + 1.0f} };

			previousMouseX = tempMouseX;
			previousMouseY = tempMouseY;
			
			float speed = (Core::DeltaTime() * CAM_MOUSE_SPEED);
			
			if ((ratio.x >= GAME_CAM_HIGHER_EPSILON || ratio.x <= GAME_CAM_LOWER_EPSILON) 
				&& (ratio.x < 1.0f || ratio.x > 1.0f))
				pos.x += ratio.x * speed;
			if ((ratio.y >= GAME_CAM_HIGHER_EPSILON || ratio.y <= GAME_CAM_LOWER_EPSILON)
				&& (ratio.y < 1.0f || ratio.y > 1.0f))
				pos.z -= ratio.y * speed;

			pos.x = std::clamp(pos.x, mapClampX.x, mapClampX.y);
			pos.z = std::clamp(pos.z, mapClampZ.x, mapClampZ.y);
		}

		inline void GameCam::UpdateZoom()
		{
			Core::Math::Vec3 dir = { viewMat.c[2].x, viewMat.c[2].y, viewMat.c[2].z };

			pos -= dir * ImGui::GetIO().MouseWheel * (CAM_MOUSE_SPEED * Core::DeltaTime());

			pos.y = std::clamp(pos.y, ZoomClamp.x, ZoomClamp.y);
		}


		inline void GameCam::Update()
		{
			if (activated)
			{
				UpdateGamePos();
				UpdateZoom();
				Camera::Update();
			}

			ResetPreviousMousePos();
		}

	}
}

#endif
