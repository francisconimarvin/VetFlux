-- Initialize the inventory rows with 0 on new doctors.
CREATE OR REPLACE FUNCTION public.inicializar_inventario()
RETURNS TRIGGER
LANGUAGE plpgsql
AS $$
BEGIN
    INSERT INTO public.inventario (cantidad, instrumento_fk, doctor_fk)
    SELECT 0, i.id, NEW.rut
    FROM public.instrumentos i
    ON CONFLICT (instrumento_fk, doctor_fk) DO NOTHING;

    RETURN NEW;
END;
$$;
