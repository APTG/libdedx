cmake_minimum_required(VERSION 3.21)

project(dedx_install_consumer LANGUAGES C)

find_package(dedx CONFIG REQUIRED)

add_executable(install_consumer ../install_consumer.c)
target_link_libraries(install_consumer PRIVATE dedx::dedx)
