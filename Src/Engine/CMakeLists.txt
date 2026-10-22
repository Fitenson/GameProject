# --------------------------------------------------
# Engine (static library)
# --------------------------------------------------

add_library(Engine STATIC
    Core/Application.cpp
    Core/Application.h
    Core/Random.cpp
    Core/Random.h
    Core.h

    # Camera
    Graphics/Camera/CameraController.cpp
    Graphics/Camera/CameraController.h
    Graphics/Renderer/Renderer.cpp
    Graphics/Renderer/Renderer.h
    Graphics.h

    # Navigation
    Navigation/NavigationStack.cpp
    Navigation/NavigationStack.h
    Navigation/Screen.h
    Navigation.h

    # UI
    UI/Component/Button/Button.h
    UI/Component/Button/Button.cpp
    UI/Component/Label/Label.h
    UI/Component/Label/Label.cpp
    UI/Component/Dialog/Dialog.h
    UI/Component/Dialog/Dialog.cpp
    UI/Component/Layout/Column.h
    UI/Component/Layout/Column.cpp
    UI/Component/Layout/Row.h
    UI/Component/Layout/Row.cpp
    UI/Component/Style/Align.h
    UI/Component/Style/Align.cpp
    UI/Component/Style/Insets.h
    UI/Component/Style/Insets.cpp
    UI/Component/Style/Modifier.h
    UI/Component/Style/Modifier.cpp
    UI.h

    Utils/Iterator.h
    Utils.h
)

target_compile_features(Engine PUBLIC cxx_std_20)

target_include_directories(Engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(Engine
    PUBLIC
        raylib
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    Camera/CameraController.cpp
    Camera/CameraController.h
    Core/Application.cpp
    Core/Application.h
)