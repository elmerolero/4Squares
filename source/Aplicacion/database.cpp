#include "database.h"
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <fstream>
using namespace std;

string databaseFile = "../recursos/datos/guardado.dat";
vector< unordered_map< string, string > * > results;
vector< string > nameColumns;

// Constructor
Database::Database():
	ptrDatabase( nullptr ), errorMessage( nullptr )
{
	// Nothing to do
}

// Destructor
Database::~Database()
{
	close();
}

// Connects with the database
void Database::open( string file )
{
	int status = sqlite3_open( file.c_str(), &ptrDatabase );
	if( status ){
		throw runtime_error( "Error trying to open database \'" + file + "\'."  );
	}
}

// Queries the database
void Database::query( string query )
{
	// Deletes the last results
	clear();

    // Makes the query
	int status = sqlite3_exec( ptrDatabase, query.c_str(), buildResults, nullptr, &errorMessage );
	if( status != SQLITE_OK ){
		throw runtime_error( "Error querying. Error: " + error() );
	}
}

// Queries the database and saves the results into a CSV file
void Database::query( string query, string fileName )
{
	// File where file will be saved
	ofstream outputFile;

	// Opens the file
	outputFile.open( fileName, ios_base::out );
	if( !outputFile ){
		throw runtime_error( "Error creating CSV file." );
	}

	// Adds BOM character for UTF-8 files
	const char *bom = "\xef\xbb\xbf";
    outputFile << bom;

	// Queries the database
	this -> query( query );
	if( results.size() > 0 ){
		// Gets the headers
		auto result = results.at( 0 );
		for( auto nameColumn : nameColumns ){
			outputFile << nameColumn << ", ";
		}
		outputFile << endl;

		// For each row
		for( auto result : results ){
			// For each column
			for( auto column : nameColumns ){
				outputFile << (* result)[ column ] << ", ";
			}
			outputFile << endl;
		}
	}

	// Closes the file
	outputFile.close();
}

// Gets the error
string Database::error() const
{
	string error = ( errorMessage != nullptr ? errorMessage : "Unknown error" );
	
	return error;
}

// Closes the database
void Database::close()
{
	// If is really open a database
	if( ptrDatabase != nullptr ){
		sqlite3_close( ptrDatabase );
	}
	
	ptrDatabase = nullptr;
}

// Deletes the last results
void Database::clear()
{
    // For each result from results...
	for( unordered_map< string, string > *result : results ){
		result -> erase( result -> begin() );
        delete result;
	}

	// Clear name columns
	nameColumns.clear();
	
	// Clean results
	results.clear();
}

// Called when a query is made
int buildResults( void * notUsed, int numberColumns, char **columns, char **columnNames )
{
	// Creates a new Row
	unordered_map< string, string > *result = new unordered_map< string, string >();
	
	// Adds the columns names if (first time)
	if( results.empty() ){
		for( int count = 0; count < numberColumns; ++count ){
			nameColumns.push_back( columnNames[ count ] );
		}
	}

	// Adds the columns found
	for( int count = 0; count < numberColumns; ++count ){
		result -> insert( make_pair( columnNames[ count ], columns[ count ] ) );
	}

	// Adds he row to vector of rows
	results.push_back( result );
	
	return 0;
}

