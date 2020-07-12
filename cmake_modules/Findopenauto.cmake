set (OPENAUTO_DIR ~/openauto)

find_path(OPENAUTO_INCLUDE_DIR
    OpenautoLog.hpp
    PATHS ${OPENAUTO_DIR}
    PATH_SUFFIXES include
)

find_path(OPENAUTO_LIB_DIR
    libopenauto.so
    PATHS ${OPENAUTO_DIR}
    PATH_SUFFIXES lib
)

if (OPENAUTO_INCLUDE_DIR AND OPENAUTO_LIB_DIR)
    set(OPENAUTO_FOUND TRUE)
endif()
  
if (OPENAUTO_FOUND)
    if (NOT openauto_FIND_QUIETLY)
        message(STATUS "Found openauto:")
        message(STATUS " - Includes: ${OPENAUTO_INCLUDE_DIR}")
        message(STATUS " - Libraries: ${OPENAUTO_LIB_DIR}")
    endif()
    add_library(openauto INTERFACE)
    target_include_directories(openauto INTERFACE ${OPENAUTO_INCLUDE_DIR}) 
    set_target_properties(openauto PROPERTIES INTERFACE_LINK_DIRECTORIES ${OPENAUTO_LIB_DIR})
    target_link_libraries(openauto INTERFACE libopenauto.so )
else()
    if (openauto_FIND_REQUIRED)
               message(FATAL_ERROR "Could not find openauto")
    endif()
endif()

mark_as_advanced(OPENAUTO_INCLUDE_DIRS OPENAUTO_LIBRARIES)
