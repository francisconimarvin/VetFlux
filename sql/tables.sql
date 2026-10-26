CREATE TABLE public.doctor (
    rut varchar(16) PRIMARY KEY,
    nombre varchar(100) NOT NULL,
    email varchar(150) UNIQUE NOT NULL,
    telefono varchar(10) UNIQUE NOT NULL,
    activo boolean NOT NULL DEFAULT true
);

CREATE TABLE public.tutor (
    rut varchar(16) PRIMARY KEY,
    nombre varchar(100) NOT NULL,
    email varchar(150) UNIQUE NOT NULL,
    telefono varchar(11) UNIQUE NOT NULL,
    domicilio varchar(200),
    doctor_fk varchar(16) NOT NULL REFERENCES public.doctor(rut)
);

CREATE TABLE public.paciente (
    id serial PRIMARY KEY,
    nombre varchar(50) NOT NULL,
    edad integer,
    sexo char(1) CHECK (sexo = 'H' OR sexo = 'M'),
    especie varchar(20) NOT NULL,
    raza varchar(20),
    color varchar(30),
    tutor_fk varchar(16) REFERENCES public.tutor(rut),
    doctor_fk varchar(16) NOT NULL REFERENCES public.doctor(rut)
);

CREATE TABLE public.agenda (
    id serial PRIMARY KEY,
    fecha_hora timestamptz UNIQUE NOT NULL,
    notas text,
    id_paciente_fk integer NOT NULL REFERENCES public.paciente(id)
);

CREATE TABLE public.instrumentos (
    id serial PRIMARY KEY,
    nombre varchar(100)
);

CREATE TABLE public.inventario (
    id serial PRIMARY KEY,
    cantidad integer NOT NULL,
    instrumento_fk integer NOT NULL REFERENCES public.instrumentos(id),
    doctor_fk varchar(16) NOT NULL REFERENCES public.doctor(rut),
    CONSTRAINT inventario_instrumento_doctor_unique UNIQUE (instrumento_fk, doctor_fk)
);

CREATE TABLE public.ficha_medica (
    id serial PRIMARY KEY,
    fecha_ficha timestamptz NOT NULL DEFAULT CURRENT_TIMESTAMP,
    titulo varchar(100) NOT NULL,
    descripcion text NOT NULL,
    id_paciente_fk integer NOT NULL REFERENCES public.paciente(id)
);

CREATE TABLE public.medicamentos (
    id serial PRIMARY KEY,
    nombre varchar(100) NOT NULL
);

CREATE TABLE public.receta (
    id serial PRIMARY KEY,
    dosis varchar(50) NOT NULL,
    paciente_fk integer NOT NULL REFERENCES public.paciente(id),
    medicamento_fk integer NOT NULL REFERENCES public.medicamentos(id)
);

CREATE TABLE public.usuarios (
    id serial PRIMARY KEY,
    fk_doctor varchar(16) REFERENCES public.doctor(rut),
    passwd varchar(258) NOT NULL,
    role varchar(10) NOT NULL
);
