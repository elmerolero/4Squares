#include "objeto.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
using namespace std;

// Tamaño de la unidad en pixeles
double Objeto::magnitudUnidad = 0;

const double Objeto::RELACION_ASPECTO = 1.77;
const double Objeto::UNIDAD_PANTALLA = 10;

// Actualiza el tamaño de la unidad
void Objeto::actualizarMagnitudUnidad( int pantallaAltura ){
	magnitudUnidad = pantallaAltura / UNIDAD_PANTALLA;
}

double Objeto::leerMagnitudUnidad( void ){
	return magnitudUnidad;
}

// Constructor
Objeto::Objeto(): espacioX( 0.f ), espacioY( 0.f ), espacioAncho( 0.f ), espacioAlto( 0.f ), desplazamientoX( 0 ), desplazamientoY( 0 ){
	desplazamientoX = 0;
	desplazamientoY = 0;
	rectTextura  = { 0, 0, 0, 0 };
	rectAbsoluto = { 0, 0, 0, 0 };
}

Objeto::~Objeto(){
	; // Empty
}

// Renderiza
void Objeto::renderizar( void ){
	// Llama a la clase base para renderizar la textura
	renderTexture( leerDimensionesTextura(), leerDimensionesEspacio() );
}

// Lee carga una textura y establece las opciones de objeto
void Objeto::leerObjetoDesdeArchivo( string ubicacion ){
	// Carga la textura
	loadFileTexture( ubicacion.c_str() );
	
	// Establece las coordenadas
	escribirDesplazamientoX( 0 );
	escribirDesplazamientoY( 0 );
	escribirDimensionesTextura( 0, 0, getWidth(), getHeight() );
	escribirDimensionesEspacio( 0, 0, ( (float)getWidth() * UNIDAD_PANTALLA ) / 1080, ( (float)getHeight() * UNIDAD_PANTALLA ) / 1080 );
}

// Establece las dimensiones actuales en el espacio
void Objeto::escribirDimensionesEspacio( SDL_DRect rect )
{
	// Establece las dimensiones relativas
	espacioX = rect.x;
	espacioY = rect.y;
	espacioAncho = rect.w;
	espacioAlto = rect.h;

	// Actualza las dimensiones absolutaas
	actualizarDimensionesAbsolutas();
}

void Objeto::escribirDimensionesEspacio( double x, double y, double ancho, double alto )
{
	// Establece las dimensiones espacioAes
	espacioX = x;
	espacioY = y;
	espacioAncho = ancho;
	espacioAlto = alto;

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

	escribirDimensionesEspacio( ( (float)x * UNIDAD_PANTALLA ) / 1080, ( (float)y * UNIDAD_PANTALLA ) / 1080, ( (float)ancho * UNIDAD_PANTALLA ) / 1080, ( (float)alto * UNIDAD_PANTALLA ) / 1080 );
} 

// Permite escribir las dimensiones espacioAes de manera individual
void Objeto::escribirEspacioX( double x ){
	espacioX = x;
	rectAbsoluto.x = espacioX * magnitudUnidad;
}

void Objeto::escribirEspacioY( double y ){
	espacioY = y;
	rectAbsoluto.y = espacioY * magnitudUnidad;
}

void Objeto::escribirEspacioAncho( double w ){
	espacioAncho = w;
	rectAbsoluto.w = espacioAncho * magnitudUnidad;
}

void Objeto::escribirEspacioAlto( double h ){
	espacioAlto = h;
	rectAbsoluto.h = espacioAlto * magnitudUnidad;
}

double Objeto::leerEspacioX( void ) const{
	return espacioX;
}

double Objeto::leerEspacioY( void ) const{
	return espacioY;
}

double Objeto::leerEspacioAncho( void ) const{
	return espacioAncho;
}

double Objeto::leerEspacioAlto( void ) const{
	return espacioAlto;
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
	// Establece la textura seleccionada se acuerdo al desplazamiento
	rectTextura.x = desplazamientoX * rectTextura.w;
	rectTextura.y = desplazamientoY * rectTextura.h;

	return &rectTextura;
}

SDL_Rect *Objeto::leerDimensionesEspacio( void ){
	return &rectAbsoluto;
}

// Actualiza las dimensiones reales respecto a la pantalla
void Objeto::actualizarDimensionesAbsolutas( void ){
	rectAbsoluto.x = espacioX * magnitudUnidad;
	rectAbsoluto.y = espacioY * magnitudUnidad;
	rectAbsoluto.w = espacioAncho * magnitudUnidad;
	rectAbsoluto.h = espacioAlto * magnitudUnidad;
}

void Objeto::escribirDesplazamientoX( int x ){
	this -> desplazamientoX = x;
}
	
void Objeto::escribirDesplazamientoY( int y ){
	this -> desplazamientoY = y;	
}

int Objeto::leerDesplazamientoX() const{
	return desplazamientoX;
}

int Objeto::leerDesplazamientoY() const{
	return desplazamientoY;
}

void Objeto::escribirFotogramaAncho( int ancho ){
	this -> rectTextura.w = ancho;
}

int Objeto::leerFotogramaAncho( void ) const{
	return rectTextura.w;
}

void Objeto::escribirFotogramaAlto( int alto ){
	this -> rectTextura.h = alto;
}

int Objeto::leerFotogramaAlto( void ) const{
	return rectTextura.h;
}
