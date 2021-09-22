import requests
import json
import os
import time
import threading
import hashlib
import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk

# Ruta donde se aloja el programa
programPath = "http://192.168.1.74/programs/scb/"

# Path where the updater interface should be
interfacePath = "resources/interfaces/Actualizador.glade"

# Checks if program's interface exists
if( not os.path.exists( interfacePath ) ):
    interface = requests.get( programPath + interfacePath, allow_redirects=True )
    if( interface.status_code != 200 ):
        print( "An error was ocurred. Error ", request.status_code )
        Gtk.main_quit()
        exit()  
    # Creates the directory
    if( not os.path.exists( "resources/interfaces/" ) ):
        os.makedirs( "resources/interfaces/" )
    # Creates the file
    open( interfacePath, 'wb' ).write( interface.content )

# Loads the interface
builder = Gtk.Builder()
builder.add_from_file( interfacePath )

window = builder.get_object( "VentanaPrincipal" )
window.connect( "destroy", Gtk.main_quit )
window.show_all()

progressBar = builder.get_object( "BarraProgreso" )

def updater():
    # Get json version checks versions
    versionFile = programPath + "program_version"

    # Makes the request for the json file
    request = requests.get( versionFile )

    # Result was not correct?
    if( request.status_code != 200 ):
        print( "An error was ocurred. Error ", request.status_code )
        Gtk.main_quit()
        return 

    # Decodes content to utf-8 and gets its json content
    request.encoding = 'utf-8'
    result = json.loads( request.content )

    # Gets program's local file to compare versions and gets the json
    localFile = open( "program_version.json" )
    local = json.loads( localFile.read() )

    # Compares version from online to local version to check if they are different
    if( result[ "version" ] <= local[ "version" ] ):
        print( "Local version is updated." )
        Gtk.main_quit()
        return

    # Determines the total size
    total = 0
    for file in result[ "archivos" ]:
        total += file[ "tamaño" ]

    # Gets the new files
    progress = 0.0
    for file in result[ "archivos" ]:
        # Gets the file
        program = requests.get( programPath + file[ "ubicacion" ], allow_redirects=True )

        # Make sure source file and destination file have the same file
        print( file[ "nombre" ] )
        receivedFile = hashlib.sha256( program.content )
        if( receivedFile.hexdigest() != file[ "hash" ] ):
            print( "Error hashing files." )
            Gtk.main_quit()
            return

        # Destination path doesn't exists?
        if( not os.path.exists( file[ "ubicacion" ] ) ):
            # Creates directory
            os.makedirs( file[ "ubicacion" ] )
        
        # Creates the file
        open( file[ "ubicacion" ] + file[ "nombre" ], 'wb').write( program.content )

        #Updates the progress bar
        progress += file[ "tamaño" ] / total
        progressBar.set_fraction( progress )
    
    Gtk.main_quit()

t = threading.Thread( target=updater )
t.start()
Gtk.main()

# Ends
print( "Success" )
