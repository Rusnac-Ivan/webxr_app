cmake_minimum_required(VERSION 3.12)

project(gl_test C CXX)

# Set standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Include cmake modules
if(NOT DEFINED EMSCRIPTEN)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
endif()

# Set src and include root path
set(SRC_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/")

# Set src and include root path
set(LIBS_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/")

# set(RESOURCES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/resources")
# message(STATUS "RESOURCES_PATH : ${RESOURCES_PATH}")
file(GLOB_RECURSE RESOURCES_ABSOLUTE_PATH LIST_DIRECTORIES FALSE ${CMAKE_CURRENT_SOURCE_DIR}/resources/*)
message(STATUS "RESOURCES_ABSOLUTE_PATH : ${RESOURCES_ABSOLUTE_PATH}")

foreach(absolute_file_path ${RESOURCES_ABSOLUTE_PATH})
    # message(STATUS "absolute_file_path : ${absolute_file_path}")
    # file(RELATIVE_PATH FILE_RELATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${absolute_file_path})
    set(RESOURCES_RELATIVE_PATH "${RESOURCES_RELATIVE_PATH} ${absolute_file_path} ")
endforeach()

message(STATUS "RESOURCES_RELATIVE_PATH : ${RESOURCES_RELATIVE_PATH}")

# file(RELATIVE_PATH RESOURCES_RELATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${RESOURCES_ABSOLUTE_PATH})
# message(STATUS "RESOURCES_RELATIVE_PATH : ${RESOURCES_RELATIVE_PATH}")
file(
    GLOB_RECURSE SOURCE_FILES
    LIST_DIRECTORIES false
    "${SRC_ROOT_PATH}/*.c"
    "${SRC_ROOT_PATH}/*.cpp"
    "${SRC_ROOT_PATH}/*.h"
    "${SRC_ROOT_PATH}/*.hpp"
)

file(
    GLOB_RECURSE LIBS_FILES
    LIST_DIRECTORIES false
    "${LIBS_ROOT_PATH}/*.c"
    "${LIBS_ROOT_PATH}/*.cpp"
    "${LIBS_ROOT_PATH}/*.h"
    "${LIBS_ROOT_PATH}/*.hpp"
)

foreach(_source IN ITEMS ${SOURCE_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${SRC_ROOT_PATH}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${LIBS_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${LIBS_ROOT_PATH}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("source\\${_group_path}" FILES "${_source}")
endforeach()

set(GLAD_INCLUDE_DIR "${LIBS_ROOT_PATH}/glad-0.1.34/include")

if(DEFINED EMSCRIPTEN)
    if(CMAKE_BUILD_TYPE MATCHES Debug) # -lnodefs.js
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s NO_EXIT_RUNTIME=1 -s USE_WEBGL2=1 -s USE_GLFW=3 -s DEMANGLE_SUPPORT=1 -s ASSERTIONS=2 --bind -s WASM=1 -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s INITIAL_MEMORY=512MB  -s MAXIMUM_MEMORY=1GB -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY ")
    else()
        add_definitions(-DNDEBUG) # -s ASYNCIFY --preload-file ${RESOURCES_RELATIVE_PATH}
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s NO_EXIT_RUNTIME=1 -s USE_WEBGL2=1 -s USE_GLFW=3 --bind -s WASM=1 -s INITIAL_MEMORY=256MB -s MAXIMUM_MEMORY=1GB -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY -O3 ")
    endif()
endif()

if(NOT DEFINED EMSCRIPTEN)
    add_executable(${PROJECT_NAME} ${INCLUDE_LIST} ${SOURCE_FILES} ${LIBS_FILES})
else()
    add_executable(${PROJECT_NAME} ${INCLUDE_LIST} ${SOURCE_FILES})
endif()

# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ ${GLAD_INCLUDE_DIR})
include_directories(${SRC_ROOT_PATH}/engine)

target_include_directories(${PROJECT_NAME} PRIVATE ${LIBS_ROOT_PATH}/stb)
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_ROOT_PATH})

if(NOT DEFINED EMSCRIPTEN)
    target_include_directories(${PROJECT_NAME} PRIVATE ${GLAD_INCLUDE_DIR})

    find_package(OpenGL REQUIRED)

    find_package(GLFW3 REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW3_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW3_LIBRARY})
    message(STATUS "GLFW3_INCLUDE_DIR: ${GLFW3_INCLUDE_DIR}")
    message(STATUS "GLFW3_LIBRARY: ${GLFW3_LIBRARY}")
endif()

if(IMGUI_PATH)
    file(GLOB IMGUI_SRC ${IMGUI_PATH}/imconfig.h)
    file(GLOB IMGUI_SRC ${IMGUI_SRC} ${IMGUI_PATH}/imgui.cpp)
    file(GLOB IMGUI_SRC ${IMGUI_SRC} ${IMGUI_PATH}/imgui.h)
    file(GLOB IMGUI_SRC ${IMGUI_SRC} ${IMGUI_PATH}/imgui_demo.cpp)
    file(GLOB IMGUI_SRC ${IMGUI_SRC} ${IMGUI_PATH}/imgui_draw.cpp)
    file(GLOB IMGUI_SRC ${IMGUI_SRC} ${IMGUI_PATH}/imgui_internal.h)
    file(GLOB IMGUI_SRC ${IMGUI_SRC} ${IMGUI_PATH}/imgui_tables.cpp)
    file(GLOB IMGUI_SRC ${IMGUI_SRC} ${IMGUI_PATH}/imgui_widgets.cpp)
    file(GLOB IMGUI_SRC ${IMGUI_SRC} ${IMGUI_PATH}/imstb_rectpack.h)
    file(GLOB IMGUI_SRC ${IMGUI_SRC} ${IMGUI_PATH}/imstb_textedit.h)
    file(GLOB IMGUI_SRC ${IMGUI_SRC} ${IMGUI_PATH}/imstb_truetype.h)
    file(GLOB IMGUI_SRC ${IMGUI_SRC} ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp)
    file(GLOB IMGUI_SRC ${IMGUI_SRC} ${IMGUI_PATH}/backends/imgui_impl_opengl3.h)
    file(GLOB IMGUI_SRC ${IMGUI_SRC} ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp)
    file(GLOB IMGUI_SRC ${IMGUI_SRC} ${IMGUI_PATH}/backends/imgui_impl_glfw.h)

    foreach(_source IN ITEMS ${IMGUI_SRC})
        get_filename_component(_source_path "${_source}" PATH)
        file(RELATIVE_PATH _source_path_rel "${IMGUI_PATH}" "${_source_path}")
        string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
        source_group("source\\imgui\\${_group_path}" FILES "${_source}")
    endforeach()

    message(STATUS "IMGUI_SRC: ${IMGUI_SRC}")
    target_sources(${PROJECT_NAME} PRIVATE ${IMGUI_SRC})
    target_include_directories(${PROJECT_NAME} PRIVATE ${IMGUI_PATH})
else()
    message(FATAL_ERROR "IMGUI_PATH is not set!")
endif()

if(IMGUIZMO_PATH)
    set(IMGUIZMO_SRC
        ${IMGUIZMO_PATH}/GraphEditor.h
        ${IMGUIZMO_PATH}/GraphEditor.cpp
        ${IMGUIZMO_PATH}/ImCurveEdit.h
        ${IMGUIZMO_PATH}/ImCurveEdit.cpp
        ${IMGUIZMO_PATH}/ImGradient.h
        ${IMGUIZMO_PATH}/ImGradient.cpp
        ${IMGUIZMO_PATH}/ImGuizmo.h
        ${IMGUIZMO_PATH}/ImGuizmo.cpp
        ${IMGUIZMO_PATH}/ImSequencer.h
        ${IMGUIZMO_PATH}/ImSequencer.cpp
        ${IMGUIZMO_PATH}/ImZoomSlider.h
    )

    foreach(_source IN ITEMS ${IMGUIZMO_SRC})
        get_filename_component(_source_path "${_source}" PATH)
        file(RELATIVE_PATH _source_path_rel "${IMGUIZMO_PATH}" "${_source_path}")
        string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
        source_group("source\\imguizmo\\${_group_path}" FILES "${_source}")
    endforeach()

    target_sources(${PROJECT_NAME} PRIVATE ${IMGUIZMO_SRC})
    target_include_directories(${PROJECT_NAME} PRIVATE ${IMGUIZMO_PATH})
else()
    message(FATAL_ERROR "IMGUIZMO_PATH is not set!")
endif()

if(TINYGLTF_PATH)
    message(STATUS "TINYGLTF_PATH: ${TINYGLTF_PATH}")
    set(TINYGLTF_SRC
        ${TINYGLTF_PATH}/json.hpp
        ${TINYGLTF_PATH}/stb_image.h
        ${TINYGLTF_PATH}/stb_image_write.h
        ${TINYGLTF_PATH}/tiny_gltf.h

        # ${TINYGLTF_PATH}/tiny_gltf.cc
    )
    target_sources(${PROJECT_NAME} PRIVATE ${TINYGLTF_SRC})
    target_include_directories(${PROJECT_NAME} PRIVATE ${TINYGLTF_PATH})
else()
    message(FATAL_ERROR TINYGLTF_PATH "TINYGLTF_PATH is not set!")
endif()

find_package(glm REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
