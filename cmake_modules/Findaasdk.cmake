set (AASDK_DIR ~/aasdk)

find_path(AASDK_INCLUDE_DIR
    aasdk/Version.hpp
    PATHS ${AASDK_DIR}
    PATH_SUFFIXES include
)

find_path(AASDK_PROTO_INCLUDE_DIR
    aasdk_proto/AbsoluteInputEventData.pb.h
    PATHS ${AASDK_DIR}
)

find_path(AASDK_LIB_DIR
    libaasdk.so
    PATHS ${AASDK_DIR}
    PATH_SUFFIXES lib
)

if (AASDK_INCLUDE_DIR AND AASDK_PROTO_INCLUDE_DIR AND AASDK_LIB_DIR)
    set(AASDK_FOUND TRUE)
endif()
  
if (AASDK_FOUND)
    if (NOT aasdk_FIND_QUIETLY)
        message(STATUS "Found aasdk:")
        message(STATUS " - Includes: ${AASDK_INCLUDE_DIR}")
        message(STATUS " - Includes: ${AASDK_PROTO_INCLUDE_DIR}")
        message(STATUS " - Libraries: ${AASDK_LIB_DIR}")
    endif()
    add_library(aasdk INTERFACE)
    target_include_directories(aasdk INTERFACE ${AASDK_INCLUDE_DIR} ${AASDK_PROTO_INCLUDE_DIR})
    set_target_properties(aasdk PROPERTIES INTERFACE_LINK_DIRECTORIES ${AASDK_LIB_DIR})
    target_link_libraries(aasdk INTERFACE libaasdk.so libaasdk_proto.so)
else()
    if (aasdk_FIND_REQUIRED)
        if(AASDK_INCLUDE_DIR AND NOT AASDK_PROTO_INCLUDE_DIR)
            message(FATAL_ERROR "aasdk was found but not built.  Perform an in-source build.")
        else()
            message(FATAL_ERROR "Could not find aasdk")
        endif()
    endif()
endif()

mark_as_advanced(AASDK_INCLUDE_DIRS AASDK_LIBRARIES)
