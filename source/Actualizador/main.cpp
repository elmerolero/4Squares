#include <iostream>
#include <fstream>
#include <string>
#include <zip.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <openssl/md5.h>
#pragma comment(lib, "urlmon.lib")
using namespace std;

string hashArchivo;

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

/*size_t memory_data(void *ptr, size_t size, size_t nmemb, void *stream ) {
    size_t largo = size * nmemb;

    hashArchivo.append( ptr, largo );

    return largo;
}*/

int main()
{
    CURL *curl;
    CURLcode result;
    FILE *fp;

    string descargaURL = "http://micuarto.000webhostapp.com/recursos/aplicaciones/4Squares.zip";
    string rutaArchivoSalida = "4Squares.zip";
    string hashURL = "http://micuarto.000webhostapp.com/recursos/hash/4Squares.php";

    // Abre el archivo para guardarlo
    fp = fopen( rutaArchivoSalida.c_str(), "wb" );
    if( fp == NULL ){
        cout << "Ocurrió un error al crear el archivo descargado." << endl;
        return 0;
    }

    // Descarga el archivo
    curl = curl_easy_init();
    if( curl ){
        // Realiza la conexion
        curl_easy_setopt( curl, CURLOPT_URL, descargaURL.c_str() );
        curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_data );
        curl_easy_setopt( curl, CURLOPT_WRITEDATA, fp );
        result = curl_easy_perform( curl );

        // Liberamos espacio
        curl_easy_cleanup( curl );
        fclose( fp );
    }

    // Hashea el archivo
    fp = fopen ( rutaArchivoSalida.c_str(), "rb" );
    if( fp == NULL ){
        cout << "Ocurrió un error al abrir el archivo descargado." << endl;
        return 0;
    }

    unsigned char c[ MD5_DIGEST_LENGTH ];
    int i;
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[ 1024 ];

    MD5_Init ( &mdContext );
    while( ( bytes = fread (data, 1, 1024, fp ) ) != 0 ){
        MD5_Update ( &mdContext, data, bytes );
    }

    MD5_Final ( c, &mdContext );
    for(i = 0; i < MD5_DIGEST_LENGTH; i++){
        printf("%02x", c[ i ] );
    }
    printf (" %s\n", rutaArchivoSalida.c_str() );
    fclose( fp );

    

    // 
    char line[ 256 ] = gets( contents );
    while( line != NULL ) {
        struct zip_stat zst;
        zip_stat_init( &zst );
        zip_stat( z, line, 0, &zst);

        cout << line;
        if( zst.name == nullptr ){
            cout << "no encontrado" << endl;
        }

        char *contenidoArchivo = new char[ zst.size ];

        //Read the compressed file
        /*zip_file *g = zip_fopen( z, line, 0 );
        zip_fread( g, contenidoArchivo, zst.size );
        zip_fclose( g );

        cout << "llegue aqui" << endl;
        struct stat sst = {0};

        if (stat(line, &sst) == -1) {
            mkdir( line );
        }*/

        // Guarda el archivo
        /**/

        // Elimina el char creado
        delete[] contenidoArchivo;

        line  = strtok(NULL, "\n");
    }

    //And close the archive
    zip_close( z );
    
    // Libera memoria
    delete[] contents;

    return 0;
}

void guardarArchivoDesdeZip( string nombreZip, string nombreArchivo ){
    //Open the ZIP archive
    int err = 0;
    zip *z = zip_open( "4Squares.zip", 0, &err );

    //Search for the file of given name
    string nombre = "content.dat";
    struct zip_stat st;
    zip_stat_init( &st );
    zip_stat( z, nombre.c_str(), 0, &st);

    //Alloc memory for its uncompressed contents
    char *contents = new char[ st.size ];

    //Read the compressed file
    zip_file *f = zip_fopen( z, nombre.c_str(), 0 );
    zip_fread( f, contents, st.size );
    zip_fclose( f );

    ofstream archivo;

    archivo.open( line, ofstream::binary );
    if( !archivo ){
        cout << "chingo a su madre el america." << endl;
    }

    archivo.write( contenidoArchivo, zst.size );
    archivo.close();
}