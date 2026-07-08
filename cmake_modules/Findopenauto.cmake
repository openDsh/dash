set(OPENAUTO_DIR "" CACHE PATH "OpenAuto install prefix")

find_path(OPENAUTO_INCLUDE_DIR
    NAMES OpenautoLog.hpp
    PATHS ${OPENAUTO_DIR}
    PATH_SUFFIXES include
)

find_library(OPENAUTO_LIBRARY
    NAMES openauto
    PATHS ${OPENAUTO_DIR}
    PATH_SUFFIXES lib
)

if (OPENAUTO_INCLUDE_DIR AND OPENAUTO_LIBRARY)
    set(OPENAUTO_FOUND TRUE)
endif()
  
if (OPENAUTO_FOUND)
    if (NOT openauto_FIND_QUIETLY)
        message(STATUS "Found openauto:")
        message(STATUS " - Includes: ${OPENAUTO_INCLUDE_DIR}")
        message(STATUS " - Libraries: ${OPENAUTO_LIBRARY}")
    endif()
    if(NOT TARGET openauto)
        add_library(openauto SHARED IMPORTED)
        set_target_properties(openauto PROPERTIES
            IMPORTED_LOCATION ${OPENAUTO_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES "${OPENAUTO_INCLUDE_DIR}"
        )
    endif()
else()
    if (openauto_FIND_REQUIRED)
               message(FATAL_ERROR "Could not find openauto")
    endif()
endif()

set(OPENAUTO_INCLUDE_DIRS ${OPENAUTO_INCLUDE_DIR})
set(OPENAUTO_LIBRARIES ${OPENAUTO_LIBRARY})

mark_as_advanced(OPENAUTO_INCLUDE_DIRS OPENAUTO_LIBRARIES OPENAUTO_LIBRARY)
