#ifndef TEMPORIZADOR_H
#define TEMPORIZADOR_H
#include <SDL2/SDL.h>
#include <sstream>
#include <iomanip>

class Temporizador
{
    public:
        Temporizador();

        void iniciar();
        void pausar();
        void reanudar();
        void reiniciar();
        bool estaEnPausa();
        Uint32 obtenerTicks();

        int obtenerLargo();
        int obtenerAlto();

        void renderizar( int, int );

    private:
        Uint32 tiempoInicio;
        Uint32 tiempoPausa;
        std::stringstream tiempo;

        //Es verdadero cuando está pausado
        bool pausa;
};

#endif // TEMPORIZADOR_H
