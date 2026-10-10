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
target_link_libraries(VetLibrary PUBLIC argon2)
target_sources(VetLibrary
    PRIVATE
        src/db.c
        src/tutor.c
        src/paciente.c
        src/menu.c
        src/utils.c
        src/login.c

    PUBLIC 
        FILE_SET HEADERS
        TYPE HEADERS
        BASE_DIRS
            include
        FILES
            include/db.h
            include/tutor.h
            include/paciente.h
            include/menu.h
            include/utils.h
            include/login.h
    )

    target_link_libraries(VetFlux
        PRIVATE
            VetLibrary
        )
