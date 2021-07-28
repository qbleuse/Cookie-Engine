# CMakeList.txt : projet CMake pour CookieEngine, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.8)

set (RENDER_SRC_FILES

    "${SRC_DIR}/Render/Renderer.cpp"
    "${SRC_DIR}/Render/DebugRenderer.cpp"
    "${SRC_DIR}/Render/RendererRemote.cpp"
    "${SRC_DIR}/Render/DrawDataHandler.cpp"

    "${SRC_DIR}/Render/FrameBuffer.cpp"
    "${SRC_DIR}/Render/ShadowBuffer.cpp" 

    "${SRC_DIR}/Render/Drawers/ModelDrawer.cpp"
    "${SRC_DIR}/Render/Drawers/DirLightDrawer.cpp"
    "${SRC_DIR}/Render/Drawers/PointLightDrawer.cpp"
    "${SRC_DIR}/Render/Drawers/FBODrawer.cpp"
    "${SRC_DIR}/Render/Drawers/MapDrawer.cpp"
    "${SRC_DIR}/Render/Drawers/SkyBox.cpp"
    "${SRC_DIR}/Render/Drawers/PlayerDrawer.cpp"
    "${SRC_DIR}/Render/Drawers/SelectionDrawer.cpp"
    "${SRC_DIR}/Render/Drawers/MiniMapDrawer.cpp"
    "${SRC_DIR}/Render/Drawers/MiniModelDrawer.cpp"

    "${SRC_DIR}/Render/RenderPass/GeometryPass.cpp"
    "${SRC_DIR}/Render/RenderPass/LightPass.cpp"
    "${SRC_DIR}/Render/RenderPass/ShadowPass.cpp"
    "${SRC_DIR}/Render/RenderPass/ComposePass.cpp"
    "${SRC_DIR}/Render/RenderPass/GameplayPass.cpp"
    "${SRC_DIR}/Render/RenderPass/MiniMapPass.cpp"
    "${SRC_DIR}/Render/RenderPass/ParticlesPass.cpp")


target_sources(CookieEngine PUBLIC ${RENDER_SRC_FILES})