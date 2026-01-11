# Credit for this protobuf generation cmake file goes to Poggicek.
# Based on their work at https://github.com/Poggicek/StickerInspect

set(PROTO_TARGETS
        ${CSGO_PROTO_DIR}/network_connection.proto
        ${CSGO_PROTO_DIR}/networkbasetypes.proto
        ${CSGO_PROTO_DIR}/cs_gameevents.proto
        ${CSGO_PROTO_DIR}/cs_usercmd.proto
        ${CSGO_PROTO_DIR}/base_gcmessages.proto
        ${CSGO_PROTO_DIR}/econ_gcmessages.proto
        ${CSGO_PROTO_DIR}/engine_gcmessages.proto
        ${CSGO_PROTO_DIR}/gcsdk_gcmessages.proto
        ${CSGO_PROTO_DIR}/gcsystemmsgs.proto
        ${CSGO_PROTO_DIR}/cstrike15_gcmessages.proto
        ${CSGO_PROTO_DIR}/cstrike15_usermessages.proto
        ${CSGO_PROTO_DIR}/netmessages.proto
        ${CSGO_PROTO_DIR}/steammessages.proto
        ${CSGO_PROTO_DIR}/usercmd.proto
        ${CSGO_PROTO_DIR}/usermessages.proto
        ${CSGO_PROTO_DIR}/gameevents.proto
        ${CSGO_PROTO_DIR}/clientmessages.proto
        ${CSGO_PROTO_DIR}/te.proto
)

if(UNIX)
    set(PROTOC_EXECUTABLE ${SOURCESDK_DIR}/devtools/bin/linux/protoc)
elseif(WIN32)
    set(PROTOC_EXECUTABLE ${SOURCESDK_DIR}/devtools/bin/protoc.exe)
endif()

foreach(PROTO_TARGET ${PROTO_TARGETS})
    get_filename_component(PROTO_FILENAME ${PROTO_TARGET} NAME_WLE)
    list(APPEND PROTO_OUTPUT ${PROTO_FILENAME}.pb.cc ${PROTO_FILENAME}.pb.h)
    list(APPEND PROTO_INPUT ${PROTO_FILENAME}.proto)
    get_filename_component(PROTO_PATH ${PROTO_TARGET} DIRECTORY)
    list(APPEND PROTO_PATHS "--proto_path=${PROTO_PATH}")
endforeach()

list(REMOVE_DUPLICATES PROTO_PATHS)
list(TRANSFORM PROTO_OUTPUT PREPEND ${CMAKE_CURRENT_BINARY_DIR}/protobufcompiler/)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/protobufcompiler)

add_custom_command(
        OUTPUT ${PROTO_OUTPUT}
        COMMAND "${PROTOC_EXECUTABLE}" -I ${SOURCESDK_DIR}/thirdparty/protobuf-3.21.8/src --proto_path=${CSGO_PROTO_DIR} ${PROTO_PATHS} --cpp_out=${CMAKE_CURRENT_BINARY_DIR}/protobufcompiler ${PROTO_INPUT}
        COMMENT "Generating protobuf file"
)

add_library(Protobufs STATIC
        ${PROTO_OUTPUT}
)

target_include_directories(Protobufs
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/protobufcompiler
        PUBLIC ${SOURCESDK_DIR}/thirdparty/protobuf-3.21.8/src
)

if(WIN32)
    target_link_libraries(Protobufs PUBLIC ${SOURCESDK_DIR}/lib/public/win64/2015/libprotobuf.lib)
elseif(UNIX)
    target_link_libraries(Protobufs PUBLIC ${SOURCESDK_DIR}/lib/linux64/release/libprotobuf.a)
endif()
set_target_properties(Protobufs PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(Protobufs PROPERTIES FOLDER SDK)