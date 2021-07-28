# CMakeList.txt : projet CMake pour CookieEngine, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.8)

set (DEPS_SRC_FILES
    "${DEPS_SRC_DIR}/DirectXTex/DDSTextureLoader11.cpp"
    "${DEPS_SRC_DIR}/DirectXTex/WICTextureLoader11.cpp"

    "${DEPS_SRC_DIR}/ImGui/imgui.cpp"
    "${DEPS_SRC_DIR}/ImGui/imgui_demo.cpp"
    "${DEPS_SRC_DIR}/ImGui/imgui_draw.cpp"
    "${DEPS_SRC_DIR}/ImGui/imgui_impl_dx11.cpp"
    "${DEPS_SRC_DIR}/ImGui/imgui_impl_glfw.cpp"
    "${DEPS_SRC_DIR}/ImGui/imgui_tables.cpp"
    "${DEPS_SRC_DIR}/ImGui/imgui_widgets.cpp"
    "${DEPS_SRC_DIR}/ImGui/imgui_stdlib.cpp")

IF(NOT(GAME_ONLY))
set (DEPS_SRC_FILES ${DEPS_SRC_FILES}
    "${DEPS_SRC_DIR}/ImGuizmo/ImCurveEdit.cpp"
    "${DEPS_SRC_DIR}/ImGuizmo/ImGradient.cpp"
    "${DEPS_SRC_DIR}/ImGuizmo/ImGuizmo.cpp"
    "${DEPS_SRC_DIR}/ImGuizmo/ImSequencer.cpp")
endif()


target_sources(CookieEngine PUBLIC ${DEPS_SRC_FILES})