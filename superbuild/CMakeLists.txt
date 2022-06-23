#
# === CMake lists for the gl_test ===
#

# === CMake options ===

# Set CMake version
cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

#
# Configure CMake environment
#

# Declare project
project(gl_test-superbuild LANGUAGES C CXX)

# Set standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include(ExternalProject)

if(NOT DEFINED LIBRARIES_PREFIX)
    set(LIBRARIES_PREFIX ${CMAKE_BINARY_DIR})
endif()

if(NOT DEFINED LIBRARIES_INSTALL_PREFIX)
    set(LIBRARIES_INSTALL_PREFIX ${LIBRARIES_PREFIX}/installed/${CMAKE_SYSTEM_NAME})
endif()

if(NOT DEFINED EMSCRIPTEN)
    ExternalProject_Add(glfw
        PREFIX ${LIBRARIES_PREFIX}/glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.4
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${LIBRARIES_INSTALL_PREFIX}/glfw
        CMAKE_CACHE_ARGS -DGLFW_BUILD_DOCS:BOOL=OFF
        -DGLFW_BUILD_TESTS:BOOL=OFF
        -DGLFW_BUILD_EXAMPLES:BOOL=OFF

        UPDATE_DISCONNECTED TRUE
        UPDATE_COMMAND ""
        BUILD_ALWAYS FALSE
    )
endif()

ExternalProject_Add(imgui
    PREFIX ${LIBRARIES_PREFIX}/imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.84
    UPDATE_DISCONNECTED TRUE
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Add_Step(imgui replace_imgui_impl_opengl3
    DEPENDEES download
    DEPENDERS configure
    COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/patches/imgui/imgui_impl_opengl3.cpp" "${LIBRARIES_PREFIX}/imgui/src/imgui/backends/imgui_impl_opengl3.cpp"
)
ExternalProject_Get_property(imgui SOURCE_DIR)
set(IMGUI_SOURCE_DIR ${SOURCE_DIR})

ExternalProject_Add(imguizmo
    PREFIX ${LIBRARIES_PREFIX}/imguizmo
    DEPENDS imgui
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG 1.83
    UPDATE_DISCONNECTED TRUE
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Add_Step(imguizmo replace_imguizmo_cpp
    DEPENDEES download
    DEPENDERS configure
    COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/patches/imguizmo/ImGuizmo.cpp" "${LIBRARIES_PREFIX}/imguizmo/src/imguizmo/ImGuizmo.cpp"
)
ExternalProject_Get_property(imguizmo SOURCE_DIR)
set(IMGUIZMO_SOURCE_DIR ${SOURCE_DIR})

ExternalProject_Add(glm
    PREFIX ${LIBRARIES_PREFIX}/glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG master
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${LIBRARIES_INSTALL_PREFIX}/glm
    -DCMAKE_DEBUG_POSTFIX=$<$<CONFIG:Debug>:d>
    -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_TOOLCHAIN_ARGS:STRING=${CMAKE_TOOLCHAIN_ARGS}
    CMAKE_CACHE_ARGS -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DBUILD_TESTING:BOOL=OFF

    UPDATE_DISCONNECTED TRUE
    UPDATE_COMMAND ""
    BUILD_ALWAYS FALSE
)

ExternalProject_Add(tinygltf
    PREFIX ${LIBRARIES_PREFIX}/tinygltf
    GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
    GIT_TAG master
    UPDATE_DISCONNECTED TRUE
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Get_property(tinygltf SOURCE_DIR)
set(TINYGLTF_SOURCE_DIR ${SOURCE_DIR})

set(INSTALLATION_PREFIXES_LIST "${LIBRARIES_INSTALL_PREFIX}/glm")

ExternalProject_Add(gl_test
    PREFIX ${LIBRARIES_PREFIX}/gl_test
    DEPENDS glm imguizmo
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/../
    CMAKE_ARGS
    CMAKE_CACHE_ARGS -DGLFW3_ROOT:PATH=${LIBRARIES_INSTALL_PREFIX}/glfw
    -DIMGUI_PATH:PATH=${IMGUI_SOURCE_DIR}
    -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_TOOLCHAIN_ARGS:STRING=${CMAKE_TOOLCHAIN_ARGS}
    -Dglm_DIR:PATH=${LIBRARIES_INSTALL_PREFIX}/glm/lib/cmake/glm
    -DCMAKE_PREFIX_PATH:STRING=${INSTALLATION_PREFIXES_LIST}
    -DTINYGLTF_PATH:PATH=${TINYGLTF_SOURCE_DIR}
    -DIMGUIZMO_PATH:PATH=${IMGUIZMO_SOURCE_DIR}
    BUILD_ALWAYS TRUE
    INSTALL_COMMAND ""
)
