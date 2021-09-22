#ifndef INICIO_H
#define INICIO_H

#include "estadojuego.h"
#include "globales.h"

class Inicio : public EstadoJuego
{
    public:
        Inicio();
        void estadoEntrada();
        void estadoEventos( SDL_Event &gGameEvent );
        void estadoLogica();
        void estadoRenderizado();
        void actualizarViewport();
        void escribirNombre( std::string nombre );
        void mostrarNombre( void );

    private:
        std::string nombre;
        bool startPresionado;
        Uint8 transparencia;
        Temporizador tiempoAtenuacion;
};

#endif