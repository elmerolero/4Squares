#ifndef DERROTA_H
#define DERROTA_H

#include <string>
#include "estadojuego.h"
#include "temporizador.h"
#include "objeto.h"
#include "juego.h"
 
class Derrota : public EstadoJuego
{
    public:
        Derrota();
        void estadoEntrada();
        void estadoEventos( SDL_Event &gGameEvent );
        void estadoLogica();
        void estadoRenderizado();
        void actualizarViewport();

        void escribirNombre( std::string nombre );
        void mostrarNombre( void );

    private:
        std::string nombre;
};

extern bool ocultarElementos;

extern int lineaSombreada;
extern std::string datosPartida;


extern int incremento;
extern double incrementoRelativo;
extern int anchoActual;
extern double anchoRelativoActual;

extern Temporizador tiempoEspera;

extern Objeto continuar;

extern Objeto textoContinuar;

struct Opciones{
    int numero;
    int seleccionada;
};

extern Opciones finOpciones;

void Opciones_Dibujar( Opciones &informacionOpciones, const char *opciones[], Objeto &opcionObjeto, Objeto &opcionTextoObjeto );


#endif