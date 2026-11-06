# CMakeList.txt : CMake project for GameProject, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("GameProject")

# --------------------------------------------------
# Dependencies
# --------------------------------------------------

include(FetchContent)

# raylib
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

# HFSM2
FetchContent_Declare(
    hfsm2
    GIT_REPOSITORY https://github.com/andrew-gresyk/HFSM2.git
    GIT_TAG master
)

FetchContent_MakeAvailable(raylib hfsm2)

# --------------------------------------------------
# Engine library
# --------------------------------------------------

add_subdirectory(Src/Engine)

# --------------------------------------------------
# Game executable
# --------------------------------------------------

add_executable(GameProject

    # Application
    GameProject.cpp
    GameProject.h

    # Game
    Src/Features/Game/Game.cpp
    Src/Features/Game/Game.h
    Src/Features/Game/GameContext.h
    Src/Features/Game/Entity.cpp
    Src/Features/Game/Entity.h
    Src/Features/Game/State/State.h
    Src/Features/Game/State/MainMenu.cpp
    Src/Features/Game/State/MainMenu.h
    Src/Features/Game/State/ArmoryMenu.cpp
    Src/Features/Game/State/ArmoryMenu.h
    Src/Features/Game/State/Loading.cpp
    Src/Features/Game/State/Loading.h
    Src/Features/Game/State/Paused.cpp
    Src/Features/Game/State/Paused.h

    Src/Features/Game/State/GameOver.cpp
    Src/Features/Game/State/GameOver.h
    Src/Features/Game/State/StageComplete.cpp
    Src/Features/Game/State/StageComplete.h
    Src/Features/Game/State/Playing.cpp
    Src/Features/Game/State/Playing.h
    Src/Features/Game/State/StateHelper.cpp
    Src/Features/Game/State/StateHelper.h
    Src/Features/Game/GameStates.h


    # World
    Src/Features/World/World.cpp
    Src/Features/World/World.h
    Src/Features/World/Map/Map.cpp
    Src/Features/World/Map/Map.h
    Src/Features/World/Map/Chunk.cpp
    Src/Features/World/Map/Chunk.h
    Src/Features/World/Map/Cell.cpp
    Src/Features/World/Map/Cell.h

    # Menu
    Src/Features/Menu/MainMenu.cpp
    Src/Features/Menu/MainMenu.h
    Src/Features/Menu/AvatarMenu.cpp
    Src/Features/Menu/AvatarMenu.h
    Src/Features/Menu/ArmoryMenu.cpp
    Src/Features/Menu/ArmoryMenu.h

    # Character
    Src/Features/Character/Character.cpp
    Src/Features/Character/Character.h
    Src/Features/Character/Stats.h
    Src/Features/Character/Item.h

    Src/Features/Character/Weapon/Weapon.cpp
    Src/Features/Character/Weapon/Weapon.h
    Src/Features/Character/Weapon/WeaponFactory.cpp
    Src/Features/Character/Weapon/WeaponFactory.h
    Src/Features/Character/Weapon/AmmoInfo.h

    Src/Features/Character/Weapon/Gun/Gun.cpp
    Src/Features/Character/Weapon/Gun/Gun.h
    Src/Features/Character/Weapon/Gun/Bullet.cpp
    Src/Features/Character/Weapon/Gun/Bullet.h
    Src/Features/Character/Weapon/Gun/GunMount.cpp
    Src/Features/Character/Weapon/Gun/GunMount.h
    Src/Features/Character/Weapon/Gun/GunStats.h
    Src/Features/Character/Weapon/Gun/Attachment/GunPart.h
    Src/Features/Character/Weapon/Gun/Attachment/Barrel.cpp
    Src/Features/Character/Weapon/Gun/Attachment/Barrel.h
    Src/Features/Character/Weapon/Gun/Attachment/HeavyBarrel.cpp
    Src/Features/Character/Weapon/Gun/Attachment/HeavyBarrel.h
    Src/Features/Character/Weapon/Gun/Attachment/HeavyReceiver.cpp
    Src/Features/Character/Weapon/Gun/Attachment/HeavyReceiver.h
    Src/Features/Character/Weapon/Gun/Attachment/Magazine.cpp
    Src/Features/Character/Weapon/Gun/Attachment/Magazine.h
    Src/Features/Character/Weapon/Gun/Attachment/Compensator.cpp
    Src/Features/Character/Weapon/Gun/Attachment/Compensator.h
    Src/Features/Character/Weapon/Gun/Attachment/Receiver.cpp
    Src/Features/Character/Weapon/Gun/Attachment/Receiver.h
    Src/Features/Character/Weapon/Gun/Attachment/Scope.cpp
    Src/Features/Character/Weapon/Gun/Attachment/Scope.h
    Src/Features/Character/Weapon/Gun/Attachment/Suppressor.cpp
    Src/Features/Character/Weapon/Gun/Attachment/Suppressor.h

    Src/Features/Character/Weapon/Sword/Sword.cpp
    Src/Features/Character/Weapon/Sword/Sword.h
    Src/Features/Character/Weapon/Sword/SwordMount.cpp
    Src/Features/Character/Weapon/Sword/SwordMount.h
    Src/Features/Character/Weapon/Sword/SwordStats.h

    Src/Features/Character/Weapon/Sword/Attachment/SwordPart.h
    Src/Features/Character/Weapon/Sword/SwordAttachment.h
    Src/Features/Character/Weapon/Sword/Attachment/Pommel.cpp
    Src/Features/Character/Weapon/Sword/Attachment/Pommel.h
    Src/Features/Character/Weapon/Sword/Attachment/Grip.cpp
    Src/Features/Character/Weapon/Sword/Attachment/Grip.h
    Src/Features/Character/Weapon/Sword/Attachment/Guard.cpp
    Src/Features/Character/Weapon/Sword/Attachment/Guard.h
    Src/Features/Character/Weapon/Sword/Attachment/Blade.cpp
    Src/Features/Character/Weapon/Sword/Attachment/Blade.h
    Src/Features/Character/Weapon/Sword/Attachment/Fuller.cpp
    Src/Features/Character/Weapon/Sword/Attachment/Fuller.h
    Src/Features/Character/Weapon/Sword/Attachment/Crescent.cpp
    Src/Features/Character/Weapon/Sword/Attachment/Crescent.h
    Src/Features/Character/Weapon/Sword/Attachment/Serrations.cpp
    Src/Features/Character/Weapon/Sword/Attachment/Serrations.h

    Src/Features/Character/Weapon/Gun/HeavyGun.cpp
    Src/Features/Character/Weapon/Gun/HeavyGun.h
    Src/Features/Character/Weapon/Shield/Shield.cpp
    Src/Features/Character/Weapon/Shield/Shield.h

    Src/Features/Character/Combat/AttackContext.h
    Src/Features/Character/Combat/HandPose.h

    # Avatar
    Src/Features/Character/Avatar/Avatar.cpp
    Src/Features/Character/Avatar/Avatar.h
    Src/Features/Character/Avatar/AvatarFactory.cpp
    Src/Features/Character/Avatar/AvatarFactory.h
    Src/Features/Character/Avatar/Soldier.cpp
    Src/Features/Character/Avatar/Soldier.h
    Src/Features/Character/Avatar/Assasin.cpp
    Src/Features/Character/Avatar/Assasin.h
    Src/Features/Character/Avatar/Tanker.cpp
    Src/Features/Character/Avatar/Tanker.h
    Src/Features/Character/Avatar/AvatarLook.h
    Src/Features/Character/Avatar/AvatarPainter.cpp
    Src/Features/Character/Avatar/AvatarPainter.h
    Src/Features/Character/Avatar/AvatarPose.h

    # NPC
    Src/Features/Character/NPC/Trooper.cpp
    Src/Features/Character/NPC/Trooper.h
    Src/Features/Character/NPC/Ally.cpp
    Src/Features/Character/NPC/Ally.h

    # Player
    Src/Features/Player/Player.cpp
    Src/Features/Player/Player.h

    # Monsters
    Src/Features/Monster/Zombie.cpp
    Src/Features/Monster/Zombie.h

    # Spawner
    Src/Features/Spawner/EnemySpawner.cpp
    Src/Features/Spawner/EnemySpawner.h
    Src/Features/Spawner/ItemSpawner.cpp
    Src/Features/Spawner/ItemSpawner.h

    # Stage
    Src/Features/World/Map/StageConfig.cpp
    Src/Features/World/Map/StageConfig.h
    Src/Features/World/Map/Stage.cpp
    Src/Features/World/Map/Stage.h

    # Puzzle
    Src/Features/World/Puzzle/Puzzle.h
    Src/Features/World/Puzzle/PuzzleRenderer.cpp
    Src/Features/World/Puzzle/PuzzleRenderer.h

    # Map (new files next to Map.cpp)
    Src/Features/World/Map/MapGenerator.cpp
    Src/Features/World/Map/MapGenerator.h

    # Menu
    Src/Features/Menu/StageScreen.cpp
    Src/Features/Menu/StageScreen.h
)

target_compile_features(GameProject PRIVATE cxx_std_20)

target_include_directories(GameProject
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/Src
        ${hfsm2_SOURCE_DIR}/include
)

target_link_libraries(GameProject
    PRIVATE
        Engine   # brings raylib along transitively
)

# TODO: Add tests and install targets if needed.