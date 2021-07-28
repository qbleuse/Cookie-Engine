# CMakeList.txt : projet CMake pour CookieEngine, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.8)

set (UI_SRC_FILES

    "${SRC_DIR}/UI/UIcore.cpp"

    "${SRC_DIR}/UI/UImenu.cpp"
    "${SRC_DIR}/UI/UIscene.cpp"
    "${SRC_DIR}/UI/CustomImWidget.cpp"
    "${SRC_DIR}/UI/Widgets/UIwidgetBases.cpp"
    "${SRC_DIR}/UI/Widgets/UIwidgets.cpp" 

    "${SRC_DIR}/UI/Widgets/GameWs/ActionPanelWidget.cpp"
    "${SRC_DIR}/UI/Widgets/GameWs/GamespectorWidget.cpp"
    "${SRC_DIR}/UI/Widgets/GameWs/MinimapWidget.cpp"
    "${SRC_DIR}/UI/Widgets/GameWs/ResourcePanelWidgets.cpp")


IF(NOT(GAME_ONLY))
set (UI_SRC_FILES ${UI_SRC_FILES}

    "${SRC_DIR}/UI/UIeditor.cpp"

    "${SRC_DIR}/UI/Widgets/EditorWs/AIBehaviorEditorWidget.cpp"
    "${SRC_DIR}/UI/Widgets/EditorWs/ConsoleWidget.cpp"
    "${SRC_DIR}/UI/Widgets/EditorWs/FileExplorerWidget.cpp"
    "${SRC_DIR}/UI/Widgets/EditorWs/GameportWidget.cpp"
    "${SRC_DIR}/UI/Widgets/EditorWs/GameUIeditorWidget.cpp"
    "${SRC_DIR}/UI/Widgets/EditorWs/HierarchyWidget.cpp"
    "${SRC_DIR}/UI/Widgets/EditorWs/InspectorWidget.cpp"
    "${SRC_DIR}/UI/Widgets/EditorWs/ParticleEditorWidget.cpp"
    "${SRC_DIR}/UI/Widgets/EditorWs/SoundOrchestratorWidget.cpp"
    "${SRC_DIR}/UI/Widgets/EditorWs/TextureEditorWidget.cpp"
    "${SRC_DIR}/UI/Widgets/EditorWs/ToolbarWidget.cpp"
    "${SRC_DIR}/UI/Widgets/EditorWs/ViewportWidget.cpp"
    "${SRC_DIR}/UI/Widgets/EditorWs/WorldSettingsWidget.cpp")

endif()


target_sources(CookieEngine PUBLIC ${UI_SRC_FILES})