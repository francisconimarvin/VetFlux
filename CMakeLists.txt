cmake_minimum_required(VERSION 3.23)

project(VetFlux LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

add_executable(VetFlux)
target_sources(VetFlux
    PRIVATE
        src/main.c
    )


add_library(VetLibrary)

find_package(PostgreSQL REQUIRED)

target_link_libraries(VetFlux PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(VetLibrary PUBLIC argon2 PostgreSQL::PostgreSQL)

target_sources(VetLibrary
    PRIVATE
        src/db.c
        src/tutor.c
        src/paciente.c
        src/menu.c
        src/utils.c
        src/login.c
        src/agenda.c
        src/inventario.c
        src/fichaMedica.c
        src/receta.c
        src/mongoose.c
        src/api.c

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
            include/agenda.h
            include/inventario.h
            include/fichaMedica.h
            include/receta.h
            include/mongoose.h
            include/api.h
    )

    target_link_libraries(VetFlux
        PRIVATE
            VetLibrary
        )
