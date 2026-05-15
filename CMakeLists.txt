cmake_minimum_required(VERSION 3.29)

#######################################################
project(lysa_tutorial_01)
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.env.cmake")
    message(FATAL_ERROR "Please create a .env.cmake file with the LYSA_ENGINE_PROJECT_DIR and VIREO_RHI_PROJECT_DIR variables")
endif ()
include(.env.cmake)
if (NOT VIREO_RHI_PROJECT_DIR)
    message(FATAL_ERROR "Please set VIREO_RHI_PROJECT_DIR in the .env.cmake file")
endif()
if (NOT LYSA_ENGINE_PROJECT_DIR)
    message(FATAL_ERROR "Please set LYSA_ENGINE_PROJECT_DIR in the .env.cmake file")
endif()
#endif()

#######################################################
set(LUA_BINDING OFF)
set(FORWARD_RENDERER ON)
set(DEFERRED_RENDERER ON)
set(PHYSIC_ENGINE_JOLT ON)
set(PHYSIC_ENGINE_PHYSX OFF)
if(WIN32)
    set(DIRECTX_BACKEND ON)
endif ()

#######################################################
set(LYSA_ENGINE_TARGET "lysa_engine")
set(LYSA_ENGINE_TARGET_SHADERS "${LYSA_ENGINE_TARGET}_shaders")
set(LYSA_ENGINE_SHADERS_BUILD_DIR ${LYSA_ENGINE_PROJECT_DIR}/shaders)
add_subdirectory(${LYSA_ENGINE_PROJECT_DIR} external_lib_build/${LYSA_ENGINE_TARGET})

#######################################################
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SHADERS_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

#######################################################
add_custom_target(shaders_copy
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${LYSA_ENGINE_SHADERS_BUILD_DIR}
        ${SHADERS_BUILD_DIR}
        COMMENT "Copying shaders"
        VERBATIM
)
add_dependencies(shaders_copy ${LYSA_ENGINE_TARGET} ${LYSA_ENGINE_TARGET_SHADERS})

#######################################################
function(build_target TARGET_NAME SRCS MODULES)
    message("Building target ${TARGET_NAME}")
    add_executable(${TARGET_NAME}
            ${SRCS}
    )
    target_sources(${TARGET_NAME}
            PUBLIC
            FILE_SET CXX_MODULES
            FILES
            ${MODULES}
    )
    lysa_compile_options(${TARGET_NAME})
    target_link_libraries(${TARGET_NAME} ${LYSA_ENGINE_TARGET})
    add_dependencies(${TARGET_NAME} shaders_copy)
endfunction()

#######################################################
set(MY_TARGET_SRC
        ${SRC_DIR}/Main.cpp
        ${SRC_DIR}/Camera.cpp
        ${SRC_DIR}/SceneInstance.cpp
        ${SRC_DIR}/SceneTree.cpp
        ${SRC_DIR}/RotatingAssetScene.cpp
)
set(MY_TARGET_MODULES
        ${SRC_DIR}/MainWindow.ixx
        ${SRC_DIR}/Camera.ixx
        ${SRC_DIR}/SceneInstance.ixx
        ${SRC_DIR}/SceneTree.ixx
        ${SRC_DIR}/RotatingAssetScene.ixx
)
build_target(myapp "${MY_TARGET_SRC}" "${MY_TARGET_MODULES}")
