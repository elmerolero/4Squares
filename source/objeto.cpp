#include "objeto.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
using namespace std;

// Tamaño de la unidad en pixeles
double Objeto::magnitudUnidad = 0;

const double Objeto::RELACION_ASPECTO = 1.77;
const double Objeto::UNIDAD_PANTALLA = 6.13;

// Actualiza el tamaño de la unidad
void Objeto::actualizarMagnitudUnidad( int pantallaAltura ){
	magnitudUnidad = pantallaAltura / 6.13;
}

double Objeto::leerMagnitudUnidad( void ){
	return magnitudUnidad;
}

// Constructor
Objeto::Objeto(): espacialX( 0.f ), espacialY( 0.f ), espacialAncho( 0.f ), espacialAlto( 0.f ){
	rectTextura  = { 0, 0, 0, 0 };
	rectAbsoluto = { 0, 0, 0, 0 };
}

Objeto::~Objeto(){
	; // Empty
}

// Renderiza
void Objeto::renderizar( void ){
	// Llama a la clase base para renderizar la textura
	Texture::renderTexture( leerDimensionesTextura(), leerDimensionesEspaciales() );
}

void Objeto::leerDimensionesDesdeArchivo( string ubicacion ){
	// File descriptor
	ifstream archivo;
	
	// Intenta abrir el archivo
	archivo.open( ubicacion );
	if( !archivo.is_open() ){
		throw runtime_error( "Error al leer archivo." );
	}
	
	// Rect de relativo y rect de texturas
	SDL_DRect rRect;
	SDL_Rect  tRect; 

	while( !archivo.eof() ){
		// Loads relative coordinates
		archivo >> rRect.x;
		archivo >> rRect.y;
		archivo >> rRect.w;
		archivo >> rRect.h;
	
		// Loads absolute coordinates
		archivo >> tRect.x;
		archivo >> tRect.y;
		archivo >> tRect.w;
		archivo >> tRect.h;
	}
	archivo.close();
	
	// Establece las coordenadas
	escribirDimensionesEspaciales( rRect );
	escribirDimensionesTextura( tRect );
}

// Establece las dimensiones actuales en el espacio
void Objeto::escribirDimensionesEspaciales( SDL_DRect rect )
{
	// Establece las dimensiones relativas
	espacialX = rect.x;
	espacialY = rect.y;
	espacialAncho = rect.w;
	espacialAlto = rect.h;

	// Actualza las dimensiones absolutaas
	actualizarDimensionesAbsolutas();
}

void Objeto::escribirDimensionesEspaciales( double x, double y, double ancho, double alto )
{
	// Establece las dimensiones espaciales
	espacialX = x;
	espacialY = y;
	espacialAncho = ancho;
	espacialAlto = alto;

	// Actualza las dimensiones absolutaas
	actualizarDimensionesAbsolutas();
}

// Establece las dimensiones de la textura
void Objeto::escribirDimensionesTextura( SDL_Rect rect ){
	rectTextura = rect;
}

void Objeto::escribirDimensionesTextura( int x, int y, int ancho, int alto ){
	rectTextura.x = x;
	rectTextura.y = y;
	rectTextura.w = ancho;
	rectTextura.h = alto;
} 

// Permite escribir las dimensiones espaciales de manera individual
void Objeto::escribirEspacialX( double x ){
	espacialX = x;
	rectAbsoluto.x = espacialX * magnitudUnidad;
}

void Objeto::escribirEspacialY( double y ){
	espacialY = y;
	rectAbsoluto.y = espacialY * magnitudUnidad;
}

void Objeto::escribirEspacialAncho( double w ){
	espacialAncho = w;
	rectAbsoluto.w = espacialAncho * magnitudUnidad;
}

void Objeto::escribirEspacialAlto( double h ){
	espacialAlto = h;
	rectAbsoluto.h = espacialAlto * magnitudUnidad;
}

double Objeto::leerEspacialX( void ) const{
	return espacialX;
}

double Objeto::leerEspacialY( void ) const{
	return espacialY;
}

double Objeto::leerEspacialAncho( void ) const{
	return espacialAncho;
}

double Objeto::leerEspacialAlto( void ) const{
	return espacialAlto;
}

// Permite leer las dimensiones absolutas de la textura de manera individual
int Objeto::leerAbsolutoX( void ) const{
	return rectAbsoluto.x;
}

int Objeto::leerAbsolutoY( void ) const{
	return rectAbsoluto.y;
}

int Objeto::leerAbsolutoAncho( void ) const{
	return rectAbsoluto.w;
}

int Objeto::leerAbsolutoAlto( void ) const{
	return rectAbsoluto.h;
}

// Establece la posición X a mostrar en la textura
void Objeto::escribirTexturaX( int x ){
	rectTextura.x = x;
}

// Establece la posición Y a mostrar en la textura
void Objeto::escribirTexturaY( int y ){
	rectTextura.y = y;
}

// Establece la posición W a mostrar en la textura
void Objeto::escribirTexturaW( int w ){
	rectTextura.w = w;
}

// Establece la posición H a mostrar en la textura
void Objeto::escribirTexturaH( int h ){
	rectTextura.h = h;
}

// Obtiene la posición X a mostrar en la textura
int Objeto::leerTexturaX( void ) const{
	return rectTextura.x;
}

// Obtiene la posición Y a mostrar en la textura
int Objeto::leerTexturaY( void ) const{
	return rectTextura.y;
}

// Obtiene la posición W a mostrar en la textura
int Objeto::leerTexturaW( void ) const{
	return rectTextura.w;
}

// Obtiene la posición H a mostrar en la textura
int Objeto::leerTexturaH( void ) const{
	return rectTextura.h;
}

SDL_Rect *Objeto::leerDimensionesTextura( void ){
	return &rectTextura;
}

SDL_Rect *Objeto::leerDimensionesEspaciales( void ){
	return &rectAbsoluto;
}

// Actualiza las dimensiones reales respecto a la pantalla
void Objeto::actualizarDimensionesAbsolutas( void ){
	rectAbsoluto.x = espacialX * magnitudUnidad;
	rectAbsoluto.y = espacialY * magnitudUnidad;
	rectAbsoluto.w = espacialAncho * magnitudUnidad;
	rectAbsoluto.h = espacialAlto * magnitudUnidad;
}
