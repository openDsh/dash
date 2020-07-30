cmake_minimum_required(VERSION 3.5.1)
project(ia CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(base_directory ${CMAKE_CURRENT_SOURCE_DIR})
set(resources_directory ${base_directory}/assets)
set(sources_directory ${base_directory}/src)
set(include_directory ${base_directory}/include)
set(plugins_directory ${base_directory}/plugins)
include(${base_directory}/cmake_modules/functions.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${base_directory}/bin)
set(EXECUTABLE_OUTPUT_PATH ${base_directory}/bin)

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
find_package(Qt5 COMPONENTS Multimedia MultimediaWidgets Bluetooth Qml Quick QuickWidgets SerialBus REQUIRED)
find_package(KF5BluezQt REQUIRED)
find_package(Protobuf REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(rtaudio REQUIRED)
find_package(taglib REQUIRED)
find_package(X11 REQUIRED)
find_package(aasdk REQUIRED)
find_package(openauto REQUIRED)

if(RPI_BUILD AND NOT GST_BUILD)
    add_definitions(-DUSE_OMX -DOMX_SKIP64BIT)
    set(BCM_HOST_LIBRARIES "/opt/vc/lib/libbcm_host.so")
    set(BCM_HOST_INCLUDE_DIRS "/opt/vc/include")
    set(ILCLIENT_INCLUDE_DIRS "/opt/vc/src/hello_pi/libs/ilclient")
    set(ILCLIENT_LIBRARIES "/opt/vc/src/hello_pi/libs/ilclient/libilclient.a;/opt/vc/lib/libvcos.so;/opt/vc/lib/libvcilcs.a;/opt/vc/lib/libvchiq_arm.so")
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
        findRpiRevision( RPI_REVISION )
        set(RPI_4_REVISIONS a03111 b03111 b03112 c03111 c03112 d03114)
        if(RPI_REVISION IN_LIST RPI_4_REVISIONS)
            message("Raspberry Pi 4 Found")
            add_definitions(-DPI4)
        endif(RPI_REVISION IN_LIST RPI_4_REVISIONS)
    endif(RPI_BUILD)
endif(GST_BUILD)
message(INFO "${GST_LIBRARIES}")

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

file(GLOB_RECURSE source_files ${sources_directory}/*.cpp ${include_directory}/*.hpp ${resources_directory}/*.qrc)

file(GLOB_RECURSE plugin_source_files ${plugins_directory}/infiniti_g37.cpp ${include_directory}/*.hpp)

add_library(InfinitiG37 SHARED ${plugin_source_files})

target_include_directories(InfinitiG37 SYSTEM PRIVATE
    ${Qt5SerialBus_INCLUDE_DIRS}
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
    ${X11_INCLUDE_DIR}
    ${include_directory}
)
target_link_libraries(InfinitiG37
    aasdk_proto
    aasdk
    openauto
    ${Boost_LIBRARIES}
    ${Qt5SerialBus_LIBRARIES}
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
    ${X11_LIBRARIES}
    )


add_executable(ia ${source_files})


target_include_directories(ia PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    )

target_include_directories(ia SYSTEM PRIVATE
    ${Qt5SerialBus_INCLUDE_DIRS}
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
    ${X11_INCLUDE_DIR}
    ${include_directory}
    )

target_link_libraries(ia
    aasdk_proto
    aasdk
    openauto
    ${Boost_LIBRARIES}
    ${Qt5SerialBus_LIBRARIES}
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
    ${X11_LIBRARIES}
    )


