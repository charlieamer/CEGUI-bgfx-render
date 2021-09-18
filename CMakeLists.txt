#TODO FINISH SETTING UP CEGUI
cmake_minimum_required (VERSION 3.8)
project(CeguiBgfxRenderer)
set (CMAKE_CXX_STANDARD 11)

function(replace_source_dir_in_file filename)
	message("Replacing in ${filename}")
	file(READ ${filename} filedata)
	string(REGEX REPLACE "(CMAKE_SOURCE_DIR)" "CeguiBgfxRenderer_CEGUI_SOURCE_DIR" filedata "${filedata}")
	file(WRITE ${filename} "${filedata}")
endfunction()

set(CeguiBgfxRenderer_MAJOR 0)
set(CeguiBgfxRenderer_MINOR 2)
set(CeguiBgfxRenderer_PATCH 0)
set(CeguiBgfxRenderer "${CeguiBgfxRenderer_MAJOR}.${CeguiBgfxRenderer_MINOR}")

set(CeguiBgfxRenderer_CEGUI_INCLUDE_PATH "" CACHE PATH "Path to CEGUI include. Essentially we need {{THIS_PATH}}/CEGUI/CEGUI.h")
if(NOT EXISTS "${CeguiBgfxRenderer_CEGUI_INCLUDE_PATH}/CEGUI/CEGUI.h")
	message(SEND_ERROR "Please set CeguiBgfxRenderer_CEGUI_INCLUDE_PATH to correct value. ${CeguiBgfxRenderer_CEGUI_INCLUDE_PATH}/CEGUI/CEGUI.h was not found")
endif()

set(CeguiBgfxRenderer_BGFX_INCLUDE_PATH "" CACHE PATH "Path to BGFX include. Essentially we need {{THIS_PATH}}/bgfx/bgfx.h")
if(NOT EXISTS "${CeguiBgfxRenderer_BGFX_INCLUDE_PATH}/bgfx/bgfx.h")
	message(SEND_ERROR "Please set CeguiBgfxRenderer_BGFX_INCLUDE_PATH to correct value. ${CeguiBgfxRenderer_BGFX_INCLUDE_PATH}/bgfx/bgfx.h was not found")
endif()

set(CeguiBgfxRenderer_BX_INCLUDE_PATH "" CACHE PATH "Path to BGFX include. Essentially we need {{THIS_PATH}}/bgfx/bgfx.h")
if(NOT EXISTS "${CeguiBgfxRenderer_BX_INCLUDE_PATH}/bx/bx.h")
	message(SEND_ERROR "Please set CeguiBgfxRenderer_BX_INCLUDE_PATH to correct value. ${CeguiBgfxRenderer_BX_INCLUDE_PATH}/bgfx/bgfx.h was not found")
endif()

set(CeguiBgfxRenderer_BIMG_INCLUDE_PATH "" CACHE PATH "Path to BGFX include. Essentially we need {{THIS_PATH}}/bgfx/bgfx.h")
if(NOT EXISTS "${CeguiBgfxRenderer_BIMG_INCLUDE_PATH}/bimg/bimg.h")
	message(SEND_ERROR "Please set CeguiBgfxRenderer_BIMG_INCLUDE_PATH to correct value. ${CeguiBgfxRenderer_BIMG_INCLUDE_PATH}/bgfx/bgfx.h was not found")
endif()

set(CeguiBgfxRenderer_BUILD_SAMPLES OFF CACHE BOOL "Build CEGUI samples. This requres that you set CeguiBgfxRenderer_CEGUI_INCLUDE_PATH to directory containing full CEGUI source")

if(NOT CeguiBgfxRenderer_BUILD_SAMPLES)
	set(CeguiBgfxRenderer_CEGUI_CONFIG_INCLUDE_PATH "" CACHE PATH "Path to include with cegui config. Essentially we need {{THIS_PATH}}/CEGUI/Config.h")
	if(NOT EXISTS "${CeguiBgfxRenderer_CEGUI_CONFIG_INCLUDE_PATH}/CEGUI/Config.h")
		message(SEND_ERROR "Please set CeguiBgfxRenderer_CEGUI_CONFIG_INCLUDE_PATH to correct value. ${CeguiBgfxRenderer_CEGUI_CONFIG_INCLUDE_PATH}/CEGUI/Config.h was not found")
	endif()

	set(CeguiBgfxRenderer_BUILD_SHARED OFF CACHE BOOL "Build dynamic library")
endif()

# SAMPLES
if(CeguiBgfxRenderer_BUILD_SAMPLES)
	set(CeguiBgfxRenderer_CEGUI_ORIGINAL_SOURCE_DIR "${CeguiBgfxRenderer_CEGUI_INCLUDE_PATH}/../..")
	set(CeguiBgfxRenderer_CEGUI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/cegui")
	set(CeguiBgfxRenderer_BUILD_SHARED ON)

	# Samples dependencies
	find_package(glfw3 REQUIRED)
	find_package(glew REQUIRED)
	find_package(glm REQUIRED)
	find_package(freetype REQUIRED)

	# Add CEGUI
	if(NOT EXISTS "${CeguiBgfxRenderer_CEGUI_ORIGINAL_SOURCE_DIR}/CMakeLists.txt")
		message(SEND_FATAL "Please set CeguiBgfxRenderer_CEGUI_INCLUDE_PATH to SOURCE CODE of CEGUI (not built version). Essentially we need {{THIS_PATH}}/../../CMakeLists.txt. ${CeguiBgfxRenderer_CEGUI_INCLUDE_PATH}/CMakeLists.txt NOT FOUND")
	endif()

	# Copy CEGUI source if it doesn't exist
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/cegui")
		file(COPY "${CeguiBgfxRenderer_CEGUI_ORIGINAL_SOURCE_DIR}" DESTINATION "${CeguiBgfxRenderer_CEGUI_SOURCE_DIR}/cegui")
		FILE(GLOB_RECURSE CMAKE_LISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/cegui/CMakeLists.txt")
		FILE(GLOB_RECURSE CMAKE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/external/cegui/*.cmake")
	endif()
	# Replace all CMAKE_SOURCE_DIR with different variable in CEGUI's cmake files
	FOREACH(fpath ${CMAKE_LISTS})
		replace_source_dir_in_file(${fpath})
	ENDFOREACH()
	FOREACH(fpath ${CMAKE_FILES})
		replace_source_dir_in_file(${fpath})
	ENDFOREACH()

	# CEGUI configuration for samples
	set(CEGUI_BUILD_RENDERER_DIRECT3D9 OFF CACHE BOOL "OVERRIDEN" FORCE)
	set(CEGUI_BUILD_RENDERER_DIRECT3D10 OFF CACHE BOOL "OVERRIDEN" FORCE)
	set(CEGUI_BUILD_RENDERER_DIRECT3D11 OFF CACHE BOOL "OVERRIDEN" FORCE)
	set(CEGUI_BUILD_RENDERER_DIRECTFB OFF CACHE BOOL "OVERRIDEN" FORCE)
	set(CEGUI_BUILD_RENDERER_IRRLICHT OFF CACHE BOOL "OVERRIDEN" FORCE)
	set(CEGUI_BUILD_RENDERER_NULL OFF CACHE BOOL "OVERRIDEN" FORCE)
	set(CEGUI_BUILD_RENDERER_OGRE OFF CACHE BOOL "OVERRIDEN" FORCE)
	set(CEGUI_BUILD_RENDERER_OPENGL OFF CACHE BOOL "OVERRIDEN" FORCE)
	set(CEGUI_BUILD_RENDERER_OPENGL3 ON CACHE BOOL "OVERRIDEN" FORCE) # We leave this to on because it makes stuff much much easier ...
	set(CEGUI_BUILD_RENDERER_OPENGL OFF CACHE BOOL "OVERRIDEN" FORCE)
	set(CEGUI_BUILD_XMLPARSER_TINYXML2 ON CACHE BOOL "OVERRIDEN" FORCE)
	set(CEGUI_SAMPLES_ENABLED ON CACHE BOOL "OVERRIDEN" FORCE)
	set(CEGUI_OPTION_DEFAULT_IMAGECODEC "BimgImageCodec" CACHE STRING "OVERRIDEN" FORCE)
	set(CEGUI_HAS_FREETYPE ON CACHE BOOL "OVERRIDEN" FORCE)
	set(CEGUI_HAS_STD11_REGEX ON CACHE BOOL "OVERRIDEN" FORCE)
	include_directories(${CMAKE_CURRENT_BINARY_DIR}/external/cegui/samples/common/include)
	add_definitions(-w) # Don't print any warnings. CEGUI generates a lot of them.
	add_compile_definitions(CEGUI_USE_GLEW)
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	set(CEGUI_USE_GLEW ON CACHE BOOL "OVERRIDEN" FORCE)
	set(CEGUI_GLFW_VER "3" CACHE STRING "OVERRIDEN" FORCE)
	include_directories(
		${glew_DIR}/../../../include
		${glm_DIR}/../../../include
		${glfw_DIR}/../../../include
	)

	add_subdirectory(external/cegui)

	remove_definitions(-w) # Print all warnings
	target_sources(CEGUISampleFramework-0.8 PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/CeguiBgfxRendererSamples/CeguiBgfxRendererSampleApp.cpp
	)
	target_include_directories(CEGUISampleFramework-0.8 PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
	)
	cmake_policy(SET CMP0079 NEW)
	target_link_libraries(CEGUIOpenGLRenderer-0 GL)
	target_link_directories(CEGUISampleFramework-0.8 PRIVATE "${glew_DIR}/../..")
	# TODO: Automatically find if GLEW has "d" suffix (i.e. on my computer is GLEWd but by default it is GLEW)
	target_link_libraries(CEGUISampleFramework-0.8 GLEWd pthread z)
	set_target_properties(CEGUITinyXML2Parser PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

	set(CeguiBgfxRenderer_CEGUI_CONFIG_INCLUDE_PATH "${CMAKE_CURRENT_BINARY_DIR}/external/cegui/cegui/include" CACHE STRING "OVERRIDEN" FORCE)
	# set_source_files_properties(
	# 	external/cegui/samples_framework/src/CEGuiOpenGL3BaseApplication.cpp
	# 	PROPERTIES COMPILE_FLAGS
	# 	-DOpenGL3Renderer=hepek
	# )
endif()

set(RENDERER_SOURCES
	src/CeguiBgfxRenderer/CeguiBgfxGeometry.cpp
	src/CeguiBgfxRenderer/CeguiBgfxRenderer.cpp
	src/CeguiBgfxRenderer/CeguiBgfxTexture.cpp
	src/CeguiBgfxRenderer/CeguiBgfxTextureTarget.cpp
	src/CeguiBgfxRenderer/CeguiBgfxViewportTarget.cpp
)

add_library(CEGUIBgfxRenderer_Static STATIC ${RENDERER_SOURCES})
if(CeguiBgfxRenderer_BUILD_SHARED)
	add_library(CEGUIBgfxRenderer SHARED ${RENDERER_SOURCES}
	)
endif()

set(IMAGE_CODEC_SOURCES
	src/CeguiBimgImageCodec/CeguiBimgImageCodec.cpp
	src/CeguiBimgImageCodec/CeguiBimgImageCodecModule.cpp
)

add_library(CEGUIBimgImageCodec_Static STATIC ${IMAGE_CODEC_SOURCES})

if(CeguiBgfxRenderer_BUILD_SHARED)
	add_library(CEGUIBimgImageCodec SHARED ${IMAGE_CODEC_SOURCES}
		${CeguiBgfxRenderer_BIMG_INCLUDE_PATH}/../src/image_decode.cpp
		${CeguiBgfxRenderer_BIMG_INCLUDE_PATH}/../src/image_gnf.cpp
		${CeguiBgfxRenderer_BIMG_INCLUDE_PATH}/../src/image.cpp
		${CeguiBgfxRenderer_BX_INCLUDE_PATH}/../src/string.cpp
		${CeguiBgfxRenderer_BX_INCLUDE_PATH}/../src/bx.cpp
		${CeguiBgfxRenderer_BX_INCLUDE_PATH}/../src/allocator.cpp
	)
endif()

set(CeguiBgfxRenderer_PUBLIC_INCLUDE_DIRS
	${CMAKE_CURRENT_SOURCE_DIR}/include
)
set(CeguiBgfxRenderer_PRIVATE_INCLUDE_DIRS 
	${CeguiBgfxRenderer_CEGUI_INCLUDE_PATH}
	${CeguiBgfxRenderer_CEGUI_CONFIG_INCLUDE_PATH}
	${CeguiBgfxRenderer_BGFX_INCLUDE_PATH}
	${CeguiBgfxRenderer_BX_INCLUDE_PATH}
	${CeguiBgfxRenderer_BIMG_INCLUDE_PATH}
)

target_include_directories(CEGUIBgfxRenderer_Static PUBLIC ${CeguiBgfxRenderer_PUBLIC_INCLUDE_DIRS})
target_include_directories(CEGUIBimgImageCodec_Static PUBLIC ${CeguiBgfxRenderer_PUBLIC_INCLUDE_DIRS})
target_include_directories(CEGUIBgfxRenderer_Static PRIVATE ${CeguiBgfxRenderer_PRIVATE_INCLUDE_DIRS})
target_include_directories(CEGUIBimgImageCodec_Static PRIVATE ${CeguiBgfxRenderer_PRIVATE_INCLUDE_DIRS})
if(CeguiBgfxRenderer_BUILD_SHARED)
	target_include_directories(CEGUIBgfxRenderer PUBLIC ${CeguiBgfxRenderer_PUBLIC_INCLUDE_DIRS})
	target_include_directories(CEGUIBimgImageCodec PUBLIC ${CeguiBgfxRenderer_PUBLIC_INCLUDE_DIRS})
	target_include_directories(CEGUIBgfxRenderer PRIVATE ${CeguiBgfxRenderer_PRIVATE_INCLUDE_DIRS})
	target_include_directories(CEGUIBimgImageCodec PRIVATE ${CeguiBgfxRenderer_PRIVATE_INCLUDE_DIRS}
		${CeguiBgfxRenderer_BIMG_INCLUDE_PATH}/../3rdparty
		${CeguiBgfxRenderer_BIMG_INCLUDE_PATH}/../3rdparty/iqa/include
		${CeguiBgfxRenderer_BIMG_INCLUDE_PATH}/../3rdparty/astc-codec/include
		${CeguiBgfxRenderer_BIMG_INCLUDE_PATH}/../3rdparty/tinyexr/deps/miniz
	)
endif()
