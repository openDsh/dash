cmake_minimum_required(VERSION 3.7.0)
project(dash CXX)

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(base_directory ${CMAKE_CURRENT_SOURCE_DIR})
set(resources_directory ${base_directory}/assets)
set(sources_directory ${base_directory}/src)
set(include_directory ${base_directory}/include)
set(plugins_directory ${base_directory}/plugins)
include(${base_directory}/cmake_modules/functions.cmake)

option(GST_BUILD "Build with GStreamer support" ON)
option(RPI_BUILD "Build Raspberry Pi-specific code paths" OFF)
set(RPI_MODEL "" CACHE STRING "Raspberry Pi model hint, for example pi4")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules/")
SET(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -rdynamic)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -Wall -pedantic -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")
endif()

add_definitions(-DBOOST_ALL_DYN_LINK)

find_package(Boost REQUIRED COMPONENTS system log OPTIONAL_COMPONENTS unit_test_framework)
find_package(libusb-1.0 REQUIRED)
find_package(Qt5 COMPONENTS Multimedia MultimediaWidgets Bluetooth Qml Quick QuickWidgets SerialBus SerialPort WebSockets DBus Xml Svg REQUIRED)
find_package(KF5BluezQt REQUIRED)
find_package(Protobuf REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(rtaudio REQUIRED)
find_package(taglib REQUIRED)
find_package(X11 REQUIRED)
find_package(aasdk REQUIRED)
find_package(openauto REQUIRED)

if(RPI_BUILD AND NOT GST_BUILD)
    message(FATAL_ERROR "RPI_BUILD requires GST_BUILD in packaged builds")
endif(RPI_BUILD AND NOT GST_BUILD)
if(GST_BUILD)
    find_package(Qt5GStreamer)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GST REQUIRED
        gstreamer-1.0>=1.4
        gstreamer-sdp-1.0>=1.4
        gstreamer-video-1.0>=1.4
        gstreamer-app-1.0>=1.4)
    add_definitions(-DUSE_GST)
    if(RPI_BUILD)
        add_definitions(-DRPI)
        if(RPI_MODEL STREQUAL "pi4")
            message("Raspberry Pi 4 build selected")
            add_definitions(-DPI4)
        endif()
    endif(RPI_BUILD)
endif(GST_BUILD)
message(INFO "${GST_LIBRARIES}")

file(GLOB_RECURSE source_files ${sources_directory}/*.cpp ${include_directory}/*.hpp ${resources_directory}/*.qrc)

add_subdirectory(plugins)

add_executable(dash ${source_files})

include(${base_directory}/cmake_modules/gitversion.cmake)

target_include_directories(dash PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    )

target_include_directories(dash SYSTEM PRIVATE
    ${Qt5Svg_INCLUDE_DIRS}
    ${Qt5Xml_INCLUDE_DIRS}
    ${Qt5WebSockets_INCLUDE_DIRS}
    ${Qt5DBus_INCLUDE_DIRS}
    ${Qt5SerialBus_INCLUDE_DIRS}
    ${Qt5SerialPort_INCLUDE_DIRS}
    ${Qt5Multimedia_INCLUDE_DIRS}
    ${Qt5MultimediaWidgets_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Bluetooth_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${LIBUSB_1_INCLUDE_DIRS}
    ${PROTOBUF_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${RTAUDIO_INCLUDE_DIRS}
    ${AASDK_PROTO_INCLUDE_DIRS}
    ${AASDK_INCLUDE_DIRS}
    ${OPENAUTO_INCLUDE_DIRS}
    ${BCM_HOST_INCLUDE_DIRS}
    ${ILCLIENT_INCLUDE_DIRS}
    ${GST_INCLUDE_DIRS}
    ${QTGSTREAMER_INCLUDE_DIR}
    ${TAGLIB_INCLUDE_DIRS}
    ${include_directory}
    )

target_link_libraries(dash
    aasdk_proto
    aasdk
    openauto
    ${Boost_LIBRARIES}
    ${Qt5Svg_LIBRARIES}
    ${Qt5Xml_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5WebSockets_LIBRARIES}
    ${Qt5SerialBus_LIBRARIES}
    ${Qt5SerialPort_LIBRARIES}
    ${Qt5Multimedia_LIBRARIES}
    ${Qt5MultimediaWidgets_LIBRARIES}
    ${Qt5Bluetooth_LIBRARIES}
    KF5::BluezQt
    ${LIBUSB_1_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
    ${BCM_HOST_LIBRARIES}
    ${ILCLIENT_LIBRARIES}
    ${WINSOCK2_LIBRARIES}
    ${RTAUDIO_LIBRARIES}
    ${GST_LIBRARIES}
    ${QTGLIB_LIBRARY}
    ${QTGLIB_LIBRARIES}
    ${QTGSTREAMER_LIBRARY}
    ${QTGSTREAMER_LIBRARIES}
    ${QTGSTREAMER_UI_LIBRARY}
    ${QTGSTREAMER_UI_LIBRARIES}
    ${Qt5QuickWidgets_LIBRARIES}
    ${QTGSTREAMER_QUICK_LIBRARIES}
    ${QTGSTREAMER_QUICK_LIBRARY}
    ${TAGLIB_LIBRARIES}
    )

install(TARGETS dash RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
