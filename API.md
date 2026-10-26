# API HTTP de VetFlux

El servidor escucha en `http://0.0.0.0:8000` por defecto. Se puede cambiar el puerto con `PORT` y la conexión PostgreSQL se configura con `DATABASE_URL` (formato conninfo de libpq o URI PostgreSQL).

```sh
DATABASE_URL='postgresql://usuario:clave@localhost:5432/VetFlux' PORT=8000 ./build/VetFlux
```

## Sesión

`POST /api/auth/login` recibe JSON `{ "rut": "...", "password": "..." }` y devuelve un token Bearer. Las rutas protegidas requieren `Authorization: Bearer <token>`. `POST /api/auth/logout` invalida ese token.

## Recursos

Todas las rutas devuelven JSON. Las operaciones de actualización y borrado filtran por el veterinario autenticado. Los valores no nulos de PostgreSQL se serializan como cadenas; `null` se conserva como JSON `null`.

| Recurso | Rutas |
| --- | --- |
| Tutores | `/api/tutores`, `/api/tutores/{rut}` |
| Pacientes | `/api/pacientes`, `/api/pacientes/{id}`, `/api/pacientes?tutor_fk={rut}` |
| Agenda | `/api/agenda`, `/api/agenda/{id}`, `/api/agenda?id_paciente={id}` |
| Fichas médicas | `/api/fichas-medicas`, `/api/fichas-medicas/{id}` |
| Recetas | `/api/recetas`, `/api/recetas/{id}`, `/api/recetas?id_paciente={id}` |
| Inventario | `GET /api/inventario`, `PUT /api/inventario/{instrumento_id}` |

Se usan `GET` para consultar, `POST` para crear, `PUT` para actualizar y `DELETE` para borrar cuando el recurso admite esa operación. `PUT` acepta los campos que se quieren modificar. Los cuerpos son objetos JSON y respetan los tamaños de las estructuras C y de las columnas SQL:

- Tutor: `rut`, `nombre`, `email`, `telefono`, `domicilio`.
- Paciente: `nombre`, `edad`, `sexo`, `especie`, `raza`, `color`, `tutor_fk`.
- Agenda: `fecha_hora`, `notas`, `id_paciente`.
- Ficha médica: `titulo`, `descripcion`, `id_paciente`.
- Receta: `dosis`, `id_paciente`, `id_medicamento`.
- Inventario: `cantidad`.

Las operaciones SQL están en sus módulos (`tutor.c`, `paciente.c`, `agenda.c`, `fichaMedica.c`, `receta.c`, `inventario.c`). `api.c` convierte la solicitud HTTP en una llamada a esas operaciones.

## Compilación

```sh
cmake -S . -B build
cmake --build build --parallel
```
