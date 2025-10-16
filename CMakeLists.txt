cmake_minimum_required(VERSION 3.19)

set(MBED_APP_JSON_PATH mbed_app.json5)

include(mbed-os/tools/cmake/mbed_toolchain_setup.cmake)
project(MyMbedApp LANGUAGES C CXX ASM)

include(mbed_project_setup)

add_subdirectory(mbed-os)

add_executable(firmware_onboarding_project main.cpp)
target_link_libraries(firmware_onboarding_project mbed-os)
mbed_set_post_build(firmware_onboarding_project)


