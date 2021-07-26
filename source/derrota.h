#ifndef DERROTA_H
#define DERROTA_H

#include <string>
#include "estadojuego.h"
#include "temporizador.h"
#include "objeto.h"
#include "juego.h"
#include "componentes.h"
 
class Derrota : public EstadoJuego
{
    public:
        Derrota();
        void estadoEntrada();
        void estadoEventos( SDL_Event &gGameEvent );
        void estadoLogica();
        void estadoRenderizado();
        void actualizarViewport();
};

void actualizarTamanioTexto( std::string texto, Objeto &objeto, TTF_Font *fuente, int tamanioBase, int anchoTextura );

extern bool ocultarElementos;

extern int lineaSombreada;
extern std::string datosPartida;
extern TTF_Font *fuenteInformacion;

extern int incremento;
extern double incrementoRelativo;
extern int anchoActual;
extern double anchoRelativoActual;
extern Objeto estadistico;

extern Objeto tableroEstadistico;

extern Temporizador tiempoEspera;
extern Objeto seAcabo;

extern Objeto continuar;

extern Objeto textoContinuar;

struct Opciones{
    int numero;
    int seleccionada;
};

extern Opciones finOpciones;

void Opciones_Dibujar( Opciones &informacionOpciones, const char *opciones[], Objeto &opcionObjeto, Objeto &opcionTextoObjeto );
std::string comparativo( int puntaje, int nivel, int lineas, int combo, Uint32 tiempo );

#endif