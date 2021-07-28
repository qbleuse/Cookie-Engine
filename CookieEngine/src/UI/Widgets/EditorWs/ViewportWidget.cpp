#include <GLFW/glfw3.h>
#include "ToolbarWidget.hpp"
#include "Editor.hpp"
#include "ViewportWidget.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

using namespace ImGui;
using namespace Cookie::UIwidget;
using namespace Cookie::ECS;
using namespace Cookie::Core::Math;


void Viewport::WindowDisplay()
{
	TryBeginWindow()
	{
		toolbar->WindowDisplay();
	
		viewportDrawspace.posx = GetWindowPos().x + GetCursorPosX();
		viewportDrawspace.posy = GetWindowPos().y + GetCursorPosY();

		if ((viewportDrawspace.width != GetContentRegionAvail().x) || (viewportDrawspace.height != GetContentRegionAvail().y))
		{
			viewportDrawspace.width  = GetContentRegionAvail().x;
			viewportDrawspace.height = GetContentRegionAvail().y;

			camera->SetProj(camera->fov, viewportDrawspace.width, viewportDrawspace.height, camera->camNear, camera->camFar);
		}

		ImGui::Image(static_cast<ImTextureID>(frameBuffer.shaderResource), GetContentRegionAvail());
		camera->windowOffset = { {viewportDrawspace.posx, viewportDrawspace.posy} };
		

		ImGuiIO& io = GetIO();

		if (io.MouseDown[ImGuiMouseButton_Right] && IsWindowHovered())
		{
			if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				io.WantCaptureKeyboard = false;
				camera->Activate();
				camera->ResetPreviousMousePos();
			}
		}
		else if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED && !io.MouseDown[ImGuiMouseButton_Right])
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			io.WantCaptureKeyboard = true;
			camera->Deactivate();
		}
		
		if (selectedEntity.focusedEntity && selectedEntity.componentHandler == coordinator.componentHandler)
			GizmoManipulator();
	}


	ImGui::End();
}

void Viewport::GizmoManipulator()
{
	ComponentTransform& trsf = coordinator.componentHandler->GetComponentTransform(selectedEntity.focusedEntity->id);
	Mat4 trsfTMat = trsf.TRS.Transpose();

	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(viewportDrawspace.posx, viewportDrawspace.posy, viewportDrawspace.width, viewportDrawspace.height);

	static ImGuizmo::OPERATION transformTool;

	switch (toolbar->CurrentTrsfTool())
	{
	case TransformTool::Translate:	transformTool = ImGuizmo::OPERATION::TRANSLATE; break;
	case TransformTool::Rotate:		transformTool = ImGuizmo::OPERATION::ROTATE;	break;
	case TransformTool::Scale:		transformTool = ImGuizmo::OPERATION::SCALE;		break;
	}


	if (ImGuizmo::Manipulate(camera->GetView().Transpose().e, camera->GetProj().Transpose().e, transformTool, ImGuizmo::MODE::LOCAL, trsfTMat.e))
	{
		trsfTMat.Transpose();
		ImGuizmo::DecomposeMatrixToComponents(trsfTMat.e, trsf.pos.e, trsf.rot.e, trsf.scale.e);
	}

	trsf.trsHasChanged = true;
}