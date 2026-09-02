cmake_minimum_required(VERSION 4.4.2)
project(VetFlux)
add_executable(VetFlux)

find_package(PostgreSQL 18.4)

target_sources(VetFlux
    PRIVATE
       src/main.c
       src/db.c
)

target_include_directories(VetFlux PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(VetFlux PRIVATE PostgreSQL::PostgreSQL)

