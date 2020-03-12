#pragma once
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

struct DBMetaData {
	char dbName[30];
	unsigned int dbSizeInBytes = 0;
	unsigned int firstBlock = 0;
	int cantBloques = -1;
	int tBloque = -1;
	int bitmapSize = -1;
	int cantTables = 0;

	DBMetaData(){
		dbSizeInBytes = 0;
	    tBloque = 0;
	    cantBloques = 0;
	}

	// DBMetaData(int tB){
	//     dbSizeInBytes = 0;
	//     tBloque = tB;
	//     cantBloques = 0;
	// }

};

struct BitMap {

	char* bitmap;

	BitMap()
	{

	}
	BitMap(int DBAmount)
	{
		bitmap = new char[DBAmount / 8];
	}


};

struct Block {

	char* data{};
	int nextDataBlockLocation = -1;

};

struct Table {
	char tName[30];
	int totalColumns;
	unsigned int infoStart = 0;
	int byte = 0;
	unsigned int rows = 0;
	bool flag = false;
	int firstBlock = -1;
};

struct Column {
	char cName[30];
	char dataType[8];
};

/*
	Recibo un vector de columnas y un objeto de tabla
	al final se podria ir agregando toda la info, poniendo un ; para marcar que ese es el final de la tabla
*/

//Para agarrar el byte libre seria encontrar la primera posicion que sea igual a -
