#include "temporizador.h"

Temporizador::Temporizador():
    tiempoInicio(0),
    tiempoPausa(0),
    pausa(true)
{
    ;
}

void Temporizador::iniciar()
{
    tiempoInicio = SDL_GetTicks();

    pausa = false;
}

void Temporizador::pausar()
{
    if( pausa == false )
    {
        tiempoPausa = SDL_GetTicks() - tiempoInicio;

        pausa = true;
    }
}

void Temporizador::reanudar()
{
    if( pausa == true)
    {
        pausa = false;
        tiempoInicio = SDL_GetTicks() - tiempoPausa;

        tiempoPausa = 0;
    }
}

void Temporizador::reiniciar()
{
    tiempoPausa = 0;
    tiempoInicio = SDL_GetTicks();
}

Uint32 Temporizador::obtenerTicks()
{
    Uint32 tiempo;

    if( pausa == true )
    {
        tiempo = tiempoPausa;
    }
    else
    {
        tiempo = SDL_GetTicks() - tiempoInicio;
    }

    return tiempo;
}

bool Temporizador::estaEnPausa()
{
    return pausa;
}
/*
void Temporizador::renderizar( int x, int y )
{
    //Color en que se renderiza
    SDL_Color color = {255, 255, 255};

    //Reinicia el texto del stream de strings
    tiempo.str("");

    //Se inserta en tiempo el tiempo transcurrido
    tiempo << (obtenerTicks() / 60000) % 60 << ":" << std::setfill('0') << std::setw(2) << (obtenerTicks() / 1000) % 60 << ":" << (obtenerTicks() % 1000) / 10;

    //Crea la textura
    tiempoGrafico.crearTexturaDeTexto(tiempo.str().c_str(), color, fuente_aaarg);

    //Renderiza
    tiempoGrafico.renderizar(x, y);
}

int Temporizador::obtenerLargo()
{
    return tiempoGrafico.obtenerLargo();
}

int Temporizador::obtenerAlto()
{
    return tiempoGrafico.obtenerAlto();
}*/
