#ifndef DERROTA_H
#define DERROTA_H

#include <string>
#include "gamestate.h"
#include "temporizador.h"
#include "object.h"
#include "game.h"
 
class Derrota : public EstadoJuego
{
    public:
        Derrota();
        void estadoEntrada();
        void estadoEventos();
        void estadoLogica();
        void estadoRenderizado();
        void actualizarViewport();
};

void actualizarTamanioTexto( std::string texto, Texture &textura, Object &objeto, TTF_Font *fuente, int tamanioBase, int anchoTextura );

extern int lineaSombreada;
extern std::string datosPartida;
extern TTF_Font *fuenteInformacion;
extern SDL_Rect fondoDerrota;

extern int incremento;
extern double incrementoRelativo;
extern int anchoActual;
extern double anchoRelativoActual;
extern Object estadisticoObjeto;
extern Texture estadisticoTextura;

extern Object tableroEstadisticoObjeto;
extern Texture tableroEstadistico;

extern Temporizador tiempoEspera;
extern Object seAcaboObjeto;
extern Texture seAcaboTextura;

extern Object continuarObjeto;
extern Texture continuarTextura;

#endif