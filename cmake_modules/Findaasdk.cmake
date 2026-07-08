set(AASDK_DIR "" CACHE PATH "AASDK install prefix")

find_path(AASDK_INCLUDE_DIR
    NAMES aasdk/Version.hpp
    PATHS ${AASDK_DIR}
    PATH_SUFFIXES include
)

find_path(AASDK_PROTO_INCLUDE_DIR
    NAMES aasdk_proto/AbsoluteInputEventData.pb.h
    PATHS ${AASDK_DIR}
    PATH_SUFFIXES include
)

find_library(AASDK_LIBRARY
    NAMES aasdk
    PATHS ${AASDK_DIR}
    PATH_SUFFIXES lib
)

find_library(AASDK_PROTO_LIBRARY
    NAMES aasdk_proto
    PATHS ${AASDK_DIR}
    PATH_SUFFIXES lib
)

if (AASDK_INCLUDE_DIR AND AASDK_PROTO_INCLUDE_DIR AND AASDK_LIBRARY AND AASDK_PROTO_LIBRARY)
    set(AASDK_FOUND TRUE)
endif()
  
if (AASDK_FOUND)
    if (NOT aasdk_FIND_QUIETLY)
        message(STATUS "Found aasdk:")
        message(STATUS " - Includes: ${AASDK_INCLUDE_DIR}")
        message(STATUS " - Includes: ${AASDK_PROTO_INCLUDE_DIR}")
        message(STATUS " - Libraries: ${AASDK_LIBRARY};${AASDK_PROTO_LIBRARY}")
    endif()
    if(NOT TARGET aasdk_proto)
        add_library(aasdk_proto SHARED IMPORTED)
        set_target_properties(aasdk_proto PROPERTIES
            IMPORTED_LOCATION ${AASDK_PROTO_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES "${AASDK_PROTO_INCLUDE_DIR}"
        )
    endif()
    if(NOT TARGET aasdk)
        add_library(aasdk SHARED IMPORTED)
        set_target_properties(aasdk PROPERTIES
            IMPORTED_LOCATION ${AASDK_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES "${AASDK_INCLUDE_DIR};${AASDK_PROTO_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES aasdk_proto
        )
    endif()
else()
    if (aasdk_FIND_REQUIRED)
        if(AASDK_INCLUDE_DIR AND NOT AASDK_PROTO_INCLUDE_DIR)
            message(FATAL_ERROR "aasdk was found but not built.  Perform an in-source build.")
        else()
            message(FATAL_ERROR "Could not find aasdk")
        endif()
    endif()
endif()

set(AASDK_INCLUDE_DIRS ${AASDK_INCLUDE_DIR} ${AASDK_PROTO_INCLUDE_DIR})
set(AASDK_LIBRARIES ${AASDK_LIBRARY} ${AASDK_PROTO_LIBRARY})

mark_as_advanced(AASDK_INCLUDE_DIRS AASDK_LIBRARIES AASDK_LIBRARY AASDK_PROTO_LIBRARY)
