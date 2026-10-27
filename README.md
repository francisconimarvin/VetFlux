# VetFlux: Sistema individual para médicos veterinarios.
### Creado por: Fluxware ®, 2026.
### Autor: Marvin Francisconi
### Versión: 1.0.0
### Licencia: [GNU General Public License v3.0](LICENSE)
### Propósito:
> Crear un sistema para que un/a médico veterinario pueda hacer agentamiento de citas, manejo de inventarios y fichas médicas. Está destinado para que el médico trabaje de forma individual con sus recursos.  

### Disclaimer:
- Este **NO** es un sistema para una clínica veterinaria.

# Stack:
- C Language 202311L (23) 
- GCC Compiler 16.2.1
- PostgreSQL
- CMake
- Argon2
- libpq

El proyecto contiene los módulos de acceso a datos y autenticación como una
biblioteca C. No incluye una interfaz de terminal ni un servidor HTTP.

La conexión PostgreSQL usa la configuración estándar de libpq. Puedes definir
el host, usuario y base de datos mediante las variables `PGHOST`, `PGUSER` y
`PGDATABASE`; libpq lee la contraseña desde `~/.pgpass` cuando corresponde.

```sh
cmake -S . -B build
cmake --build build --parallel
```

