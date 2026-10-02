cmake_minimum_required(VERSION 3.23)

project(VetFlux)

add_executable(VetFlux)
target_sources(VetFlux
    PRIVATE
        src/main.c
    )


add_library(VetLibrary)
find_package(PostgreSQL)
target_link_libraries(VetFlux PRIVATE PostgreSQL::PostgreSQL)
target_sources(VetLibrary
    PRIVATE
        src/db.c
        src/tutor.c

    PUBLIC 
        FILE_SET HEADERS
        TYPE HEADERS
        BASE_DIRS
            include
        FILES
            include/db.h
            include/tutor.h

    )

    target_link_libraries(VetFlux
        PRIVATE
            VetLibrary
        )


