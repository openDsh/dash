set(H264_DIR "" CACHE PATH "h264bitstream install prefix")

find_path(H264_INCLUDE_DIR
    NAMES h264_stream.h
    PATHS ${H264_DIR}
    PATH_SUFFIXES include include/h264bitstream
)

find_library(H264_LIBRARY
    NAMES h264bitstream
    PATHS ${H264_DIR}
    PATH_SUFFIXES lib
)

if (H264_INCLUDE_DIR AND H264_LIBRARY)
    set(H264_FOUND TRUE)
endif()

if (H264_FOUND)
    if (NOT h264_FIND_QUIETLY)
        message(STATUS "Found h264bitstream:")
        message(STATUS " - Includes: ${H264_INCLUDE_DIR}")
        message(STATUS " - Libraries: ${H264_LIBRARY}")
    endif()
    if(NOT TARGET h264)
        add_library(h264 UNKNOWN IMPORTED)
        set_target_properties(h264 PROPERTIES
            IMPORTED_LOCATION ${H264_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES "${H264_INCLUDE_DIR}"
        )
    endif()
else()
    if (h264_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find h264bitstream")
    endif()
endif()

set(H264_INCLUDE_DIRS ${H264_INCLUDE_DIR})
set(H264_LIBRARIES ${H264_LIBRARY})

mark_as_advanced(H264_INCLUDE_DIRS H264_LIBRARIES H264_INCLUDE_DIR H264_LIBRARY)
