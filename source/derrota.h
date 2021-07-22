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

bool ocultarElementos = false;

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

extern Object textoContinuarObjeto;
extern Texture textoContinuarTextura;

struct Opciones{
    int numero;
    int seleccionada;
};

Opciones finOpciones = { 2, 1 };
const char *finListaOpciones[] = { "Volver a jugar", "Salir" };

void Opciones_Dibujar( Opciones &informacionOpciones, const char *opciones[], Texture &opcionTextura, Object &opcionObjeto, Texture &textoTextura, Object &opcionTextoObjeto );

#endif