cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

set(SRC_FILES
    DeviceIOTest.cpp
    bt_test.cpp
    rk_ble_app.c
    rk_wifi_test.c
)

add_executable(deviceio_test ${SRC_FILES})
target_include_directories(deviceio_test PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/../DeviceIO/include" )
target_link_libraries(deviceio_test pthread DeviceIo asound)

install(TARGETS deviceio_test DESTINATION bin)
