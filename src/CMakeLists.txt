add_executable(${PROJECT_NAME} set49mode.cpp)
target_link_libraries(${PROJECT_NAME} usb-1.0)
install(TARGETS set49mode DESTINATION bin)