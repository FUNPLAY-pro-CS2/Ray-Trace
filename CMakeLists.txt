cmake_minimum_required(VERSION 3.18)
project(TemplatePlugin C CXX ASM)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(FUNCHOOK_BUILD_TESTS OFF CACHE BOOL "Disable building tests for funchook." FORCE)
set(FMT_WERROR OFF CACHE BOOL "" FORCE)

if(DEFINED ENV{HL2SDKCS2})
    set(SOURCESDK_DIR $ENV{HL2SDKCS2})
else()
    message(FATAL_ERROR "Environment variable HL2SDKCS2 is not set!")
endif()

if(DEFINED ENV{MMSOURCE_DEV})
    set(METAMOD_DIR $ENV{MMSOURCE_DEV})
else()
    message(FATAL_ERROR "Environment variable MMSOURCE_DEV is not set!")
endif()

if(DEFINED ENV{CSGO_PROTO})
    set(CSGO_PROTO_DIR $ENV{CSGO_PROTO})
else()
    message(FATAL_ERROR "Environment variable CSGO_PROTO is not set!")
endif()

include(makefiles/shared.cmake)
include(makefiles/protobuf.cmake)

add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/funchook)

file(GLOB_RECURSE SOURCE_FILES
    src/*.cpp
    src/*.h
    src/*.hpp
)

list(APPEND SOURCE_FILES
    ${SOURCESDK_DIR}/public/tier0/memoverride.cpp
    ${SOURCESDK_DIR}/tier1/generichash.cpp
    ${SOURCESDK_DIR}/tier1/keyvalues3.cpp
    ${SOURCESDK_DIR}/tier1/convar.cpp
    ${SOURCESDK_DIR}/entity2/entityidentity.cpp
    ${SOURCESDK_DIR}/entity2/entitysystem.cpp
    ${SOURCESDK_DIR}/entity2/entitykeyvalues.cpp
    ${METAMOD_DIR}/core/sourcehook/sourcehook.cpp
    ${METAMOD_DIR}/core/sourcehook/sourcehook_impl_chookidman.cpp
    ${METAMOD_DIR}/core/sourcehook/sourcehook_impl_chookmaninfo.cpp
    ${METAMOD_DIR}/core/sourcehook/sourcehook_impl_cvfnptr.cpp
    ${METAMOD_DIR}/core/sourcehook/sourcehook_impl_cproto.cpp
    vendor/dynlibutils/module.cpp
    vendor/dynlibutils/module.h
)

foreach(f ${SOURCE_FILES})
    message(STATUS "Zdrojový soubor: ${f}")
endforeach()

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_compile_definitions(${PROJECT_NAME}
    PUBLIC PROJECT_NAMESPACE=${PROJECT_NAME}
    PUBLIC PROJECT_NAMESPACE_STR=${PROJECT_NAME}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cs2_sdk
)

find_package(Git QUIET)
if (GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_SHA
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
else()
    set(GIT_SHA "nogit")
endif()

string(TIMESTAMP BUILD_TIME_UTC "%Y%m%d%H%M%S" UTC)
set(_seed_input "${GIT_SHA}-${BUILD_TIME_UTC}")
string(SHA1 _sha1 "${_seed_input}")
string(SUBSTRING ${_sha1} 0 16 _seed16)
set(BUILD_SEED_HEX "0x${_seed16}")
message(STATUS "Obfuscation BUILD_SEED: ${BUILD_SEED_HEX} (from ${_seed_input})")
add_compile_definitions(BUILD_SEED=${BUILD_SEED_HEX})

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -fvisibility=hidden -fdata-sections -ffunction-sections)
    target_link_options(${PROJECT_NAME} PRIVATE -Wl,--gc-sections)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    find_program(STRIP_EXECUTABLE strip)
    if (STRIP_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${STRIP_EXECUTABLE} --strip-unneeded $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Stripping symbols from $<TARGET_FILE:${PROJECT_NAME}>"
        )
    endif()
endif()

if (LINUX)
    include(${CMAKE_SOURCE_DIR}/makefiles/linux.base.cmake)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/TemplatePlugin/bin/linuxsteamrt64"
    )
elseif (WIN32)
    include(${CMAKE_SOURCE_DIR}/makefiles/windows.base.cmake)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/TemplatePlugin/bin/win64"
    )
endif()

target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})

add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/configs ${CMAKE_BINARY_DIR}
)
