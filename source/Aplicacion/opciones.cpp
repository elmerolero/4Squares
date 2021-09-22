#include "opciones.h"
#include "texture.h"
#include "object.h"

void Opciones_Dibujar( Opciones &informacionOpciones, const char *opciones[], Texture &opcionTextura, Objeto &opcionObjeto, Texture &textoTextura ){
    // Obtiene la mitad de la pantalla
    double mediaY = gameViewport.h / 2;

    // Dibuja el las opciones disponibles
    for( int i = 0; i < opciones.numero; i++ ){
        double posicionY = ( opcionObjeto.getRelativeH() + .2 ) * ( i ) + mediaY;
        opcionObjeto.setRelativeY( posicionY );
        opcionTextura.renderTexture( opcionObjeto.getSrcRect(), opcionObjeto.getDestRect() );

        // ¿Es la opción seleccionada?
        if( opciones.seleccionada == i ){
            opcionObjeto.establecerTexturaX( opcionTextura.getHeight() / 2 );
            opcionPausaTextura.renderTexture( opcioObjeto.getSrcRect(), opcionPausaObjeto.getDestRect() );
            opcionObjeto.establecerTexturaX( 0 );
        }

        if( textoTextura.crearTexturaDesdeTextoSolido( opciones[ i ], COLOR_BLANCO, fuenteArg ) ){
            SDL_DRect rrect = { opcionObjeto.getRelativeX() + 0.1, posicionY + 0.07, (float)textoTextura.getWidth() / gameUnitSize, (float)textoTextura.getHeight() / gameUnitSize };
            opcionTextoObjeto.setTextureCoords( obtenerRectTextura( opcionTextoTextura ) );
            opcionTextoObjeto.setRelativeCoords( rrect );
            opcionTextoTextura.renderTexture( opcionTextoObjeto.getSrcRect(), opcionTextoObjeto.getDestRect() );
        }
    }
}