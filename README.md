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
- PostgreSQL/libpq
- CMake
- Argon2

El proyecto contiene los módulos de acceso a datos y un servidor con Socket TCP configurado para recibir métodos HTTP. **Si tienes alguna consulta respecto a esto, puedes leer el [Beej's Network Programming Guide](https://beej.us/guide/bgnet/) y el manual de POSIX Programmer's Manual disponible en Linux, ten en cuenta que las conexiones del socket son diferentes para Windows y Mac.**

La conexión PostgreSQL usa la configuración estándar de libpq. `Leer la documentación oficial de PostgreSQL 18 si tienes alguna consulta al respecto.`

```sh
cmake -B build
cmake --build build
./build/VetFlux
```

