#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <unordered_map>
#include <vector>
#include <string>

extern std::string databaseFile;
extern std::vector< std::unordered_map< std::string, std::string > * >results;

class Database
{
	public:
		// Constructor
		Database();
		
		// Destructor
		~Database();
		
		// Connects with the database
		void open( std::string file );
		
		// Queries the database
		void query( std::string query );
		void query( std::string query, std::string fileName );
		
		// Gets the error found in the data base
		std::string error() const;
		
		// Closes the database
		void close();
	
	private:
		void clear();
		sqlite3 *ptrDatabase;
		char *errorMessage;
};

// Called when a query is made
int buildResults( void * notUsed, int numberColums, char **columns, char **columnNames );

#endif
