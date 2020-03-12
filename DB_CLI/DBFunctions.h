#include "BitMapFunctions.h"
#include <string>
#include <fstream>


using namespace std;

class DBFunctions
{

public:
    int getPos(string dbName, int infoStart, int);
    char* getBitmapFromFile(string dbName);
    void flagTable(string dbName, string tName);
    void setFBlock(string dbName, string tName, int fBlock);
    void setBlockOn_Off(string dbName, int nBlock, bool on);
    int getTableRows(string dbName, string tName);
    void updtInfoStart(unsigned int infoStartBytes, string dbName, string tName, int);
    int getInfoStart(string dbName, string tName);
    void updtTableRows(string dbName, string tName, bool insert, bool all);
    Table getTableByName(string dbName, string tName);
    vector<Column> getColumnsFromFile(string dbName, string tName);
    void updtTotalTables(string);
    void createDB(DBMetaData dbmd);
    void dropDB(string dbName);
    void getLastData(char*);
    DBMetaData connectDB(string dbName);
    void createTable(string dbName, string tName, vector<Column> columns);
    bool dropTable(string dbName, string tName);
    void printDroppedT(string dbName, string tName);
    void insert(string dbName, string tName, vector<string>);
    void deleteRows(string dbName, string tName);
    void deleteRowsCondition(string dbName, string tName, string condition);
    void selectAll(string dbName, string tName, string condition);
    void select(string dbName, string tName, vector<string>, string condition);
    void update(string dbName, string tName, vector<string> set, string condition);
    vector<string> conditionToVector(string condition);
};

int DBFunctions::getPos(string dbName, int infoStart, int reads)
{
    ifstream db(dbName+".reciodb");

    DBMetaData dbmd;
    Block b;

    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));
    
    b.data = new char[dbmd.tBloque - 4];

    int pos = 0;
    db.seekg(infoStart, ios::beg);
    for (int i = 1; i <= dbmd.cantBloques; i++)
    {
        db.read(reinterpret_cast<char*>(b.data), dbmd.tBloque - 4);
        db.read(reinterpret_cast<char*>(&pos), sizeof(int));
        if (i == reads)
        {
            db.close();
            return pos;
        }
    }

    db.close();
    return -2;

}

char* DBFunctions::getBitmapFromFile(string dbName)
{
    ifstream db(dbName+".reciodb", ios::in | ios::binary);

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    BitMap bm;
    bm.bitmap = new char[dbmd.bitmapSize];
    db.read(reinterpret_cast<char*>(bm.bitmap), dbmd.bitmapSize);

    db.close();
    return bm.bitmap;

}

void DBFunctions::flagTable(string dbName, string tName)
{
    fstream db(dbName + ".reciodb", ios::in | ios::out | ios::binary);

    if (!db)
        return;

    const int COL_TABLE_SIZE = sizeof(Column) + sizeof(Table);
    int total = 0;

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    db.seekg(sizeof(DBMetaData) + dbmd.bitmapSize + (dbmd.cantBloques * (dbmd.tBloque)), ios::beg);

    for (int i = 0; i < dbmd.cantTables; i++)
    {
        int pos = (int)db.tellg();
        Table t;
        Column c;
        db.read(reinterpret_cast<char*>(&t), sizeof(Table));
        for (int j = 1; j <= t.totalColumns; j++)
            db.read(reinterpret_cast<char*>(&c), sizeof(Column));

        if (strcmp(tName.c_str(), t.tName) == 0) {
            t.flag = true;
            db.seekp(pos, ios::beg);
            db.write(reinterpret_cast<char*>(&t), sizeof(Table));
            break;
        }

    }

    db.close();
}

void DBFunctions::setFBlock(string dbName, string tName, int fBlock)
{
    fstream db(dbName + ".reciodb", ios::in | ios::out | ios::binary);

    if (!db)
        return;

    const int COL_TABLE_SIZE = sizeof(Column) + sizeof(Table);
    int total = 0;

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    db.seekg(sizeof(DBMetaData) + dbmd.bitmapSize + (dbmd.cantBloques * (dbmd.tBloque)), ios::beg);

    for (int i = 0; i < dbmd.cantTables; i++)
    {
        int pos = (int)db.tellg();
        Table t;
        Column c;
        db.read(reinterpret_cast<char*>(&t), sizeof(Table));
        for (int j = 1; j <= t.totalColumns; j++)
            db.read(reinterpret_cast<char*>(&c), sizeof(Column));

        if (strcmp(tName.c_str(), t.tName) == 0) {
            t.firstBlock = fBlock;
            db.seekp(pos, ios::beg);
            db.write(reinterpret_cast<char*>(&t), sizeof(Table));
            break;
        }

    }

    db.close();
}

vector<string> DBFunctions::conditionToVector(string condition)
{
    //aasdf=asdfaf
    vector<string> rVector;
    string rString = "";
    for (int i = 0; i < condition.length(); i++)
    {
        if (condition[i] == '=')
        {
            rVector.push_back(rString);
            rString = "";
            continue;
        }
        rString += condition[i];
    }
    rVector.push_back(rString);

    return rVector;

}

void DBFunctions::setBlockOn_Off(string dbName, int nBlock, bool on) {
    fstream db(dbName+".reciodb", ios::in | ios::out | ios::binary);

    if (!db) return;

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    BitMap bm;
    bm.bitmap = new char[dbmd.bitmapSize];
    db.read(reinterpret_cast<char*>(bm.bitmap), dbmd.bitmapSize);

    BMFcunctions bFunc(bm);

    if (on)
    {
        //Set on para proceder a actualizar el bitmap
        bFunc.setOn(nBlock);
        db.seekp(sizeof(DBMetaData), ios::beg);
        //Se actualiza el bitmap
        db.write(reinterpret_cast<const char*>(bFunc.bm.bitmap), dbmd.bitmapSize);
        //bFunc.printNewBM(dbmd.bitmapSize);
    }
    else
    {
        //Set off para proceder a actualizar el bitmap
        bFunc.setOff(nBlock);
        db.seekp(sizeof(DBMetaData), ios::beg);
        //Se actualiza el bitmap
        db.write(reinterpret_cast<const char*>(bFunc.bm.bitmap), dbmd.bitmapSize);
        //bFunc.printNewBM(dbmd.bitmapSize);
    }

}

void DBFunctions::updtTableRows(string dbName, string tName, bool insert, bool all) {
    fstream db(dbName+".reciodb", ios::in | ios::out | ios::binary);

    if (!db) return;

    const int COL_TABLE_SIZE = sizeof(Column) + sizeof(Table);
    int total = 0;

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    db.seekg(sizeof(DBMetaData) + dbmd.bitmapSize + (dbmd.cantBloques * (dbmd.tBloque)), ios::beg);

    for (int i = 0; i < dbmd.cantTables; i++)
    {
        int pos = (int)db.tellg();
        Table t;
        Column c;
        db.read(reinterpret_cast<char*>(&t), sizeof(Table));
        for (int j = 1; j <= t.totalColumns; j++)
            db.read(reinterpret_cast<char*>(&c), sizeof(Column));

        if (strcmp(tName.c_str(), t.tName) == 0) {
            if (insert)
            {
                t.rows++;
                db.seekp(pos, ios::beg);
                db.write(reinterpret_cast<char*>(&t), sizeof(Table));
                break;
            }
            else
            {
                if (all)
                {
                    t.rows = 0;
                    db.seekp(pos);
                    db.write(reinterpret_cast<char*>(&t), sizeof(Table));
                    break;
                }
                else
                {
                    t.rows--;
                    db.seekp(pos);
                    db.write(reinterpret_cast<char*>(&t), sizeof(Table));
                    break;
                }
            }
        }

    }


    db.close();

}

int DBFunctions::getTableRows(string dbName, string tName) {
    ifstream db(dbName+".reciodb", ios::in | ios::binary);
    int rRows = 0;

    if (!db) return -1;

    const int COL_TABLE_SIZE = sizeof(Column) + sizeof(Table);
    int total = 0;

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));
    db.seekg(sizeof(DBMetaData) + dbmd.bitmapSize + (dbmd.cantBloques * (dbmd.tBloque)), ios::beg);

    for (int i = 0; i < dbmd.cantTables; i++)
    {
        Table t;
        Column c;
        db.read(reinterpret_cast<char*>(&t), sizeof(Table));
        for (int j = 1; j <= t.totalColumns; j++)
            db.read(reinterpret_cast<char*>(&c), sizeof(Column));

        if (strcmp(tName.c_str(), t.tName) == 0) {
            rRows = t.rows;
            break;
        }

    }

    db.close();
    return rRows;

}

Table DBFunctions::getTableByName(string dbName, string tName)
{
    ifstream db(dbName+".reciodb", ios::in | ios::out | ios::binary);
    Table rTable;

    if (!db) return rTable;

    const int COL_TABLE_SIZE = sizeof(Column) + sizeof(Table);

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    db.seekg(sizeof(DBMetaData) + dbmd.bitmapSize + (dbmd.cantBloques * (dbmd.tBloque)), ios::beg);

    for (int i = 0; i < dbmd.cantTables; i++)
    {
        Column c;
        db.read(reinterpret_cast<char*>(&rTable), sizeof(Table));
        for (int j = 1; j <= rTable.totalColumns; j++)
            db.read(reinterpret_cast<char*>(&c), sizeof(Column));

        if ((strcmp(rTable.tName, tName.c_str()) == 0) && !rTable.flag)
        {
            db.close();
            return rTable;
        }

    }

    db.close();
    return rTable;

}

vector<Column> DBFunctions::getColumnsFromFile(string dbName, string tName) {
    ifstream db(dbName + ".reciodb", ios::in | ios::out | ios::binary);
    vector<Column> rColumns;

    if (!db) return rColumns;

    const int COL_TABLE_SIZE = sizeof(Column) + sizeof(Table);

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    db.seekg(sizeof(DBMetaData) + dbmd.bitmapSize + (dbmd.cantBloques * (dbmd.tBloque)), ios::beg);

    for (int i = 0; i < dbmd.cantTables; i++)
    {
        Table t;
        Column c;
        db.read(reinterpret_cast<char*>(&t), sizeof(Table));

        if ((strcmp(t.tName, tName.c_str())) == 0 && !t.flag)
        {
            for (int j = 1; j <= t.totalColumns; j++) {
                db.read(reinterpret_cast<char*>(&c), sizeof(Column));
                rColumns.push_back(c);
            }

            db.close();
            return rColumns;

        }

        for (int j = 1; j <= t.totalColumns; j++)
            db.read(reinterpret_cast<char*>(&c), sizeof(Column));

    }

    db.close();
    return rColumns;
}

void DBFunctions::updtTotalTables(string dbName) {
    fstream db(dbName + ".reciodb", ios::in | ios::out | ios::binary);
    if (!db) return;

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    dbmd.cantTables++;
    cout << dbmd.cantTables;
    db.seekp(ios::beg);
    db.write(reinterpret_cast<const char*>(&dbmd), sizeof(DBMetaData));
    db.close();
}

void DBFunctions::createDB(DBMetaData dbmd)
{

    string name(dbmd.dbName);
    ofstream newDB(name + ".reciodb", ios::out | ios::binary | ios::app);

    if (!newDB)
    {
        cout << "\nThe DB could not be created!";
        return;
    }

    //Write metadata
    dbmd.firstBlock = sizeof(DBMetaData) + (dbmd.cantBloques/8);
    newDB.write(reinterpret_cast<const char*>(&dbmd), sizeof(DBMetaData));

    //Write bitmap
    cout << dbmd.bitmapSize;
    BMFcunctions bmFunctions(dbmd.bitmapSize);
    bmFunctions.initBitMap(dbmd.cantBloques);
    newDB.write(reinterpret_cast<const char*>(bmFunctions.bm.bitmap), dbmd.bitmapSize);

    //Write blocks
    Block *b = new Block();
    b->data = new char[dbmd.tBloque-4];
    for (int i = 0; i < dbmd.tBloque - 4; i++)
        b->data[i] = '-';

    for (int i = 0; i < dbmd.cantBloques; i++)
    {
        newDB.write(reinterpret_cast<const char*>(b->data), (dbmd.tBloque - 4));
        newDB.write(reinterpret_cast<const char*>(&b->nextDataBlockLocation), sizeof(int));
    }

    newDB.close();

    cout << "\n\tDataBase created successfully!!";
}

void DBFunctions::getLastData(char* n)
{
    /*string name(n);
    ifstream db(name + ".reciodb", ios::in | ios::binary);

    if (!db)
        return;

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(dbmd));

    const auto b = new Block(dbmd.tBloque);
    b->data = new char[dbmd.tBloque];
    db.read(reinterpret_cast<char*>(b), dbmd.tBloque);

    cout << endl
        << "Hola: ";
    cout << b->data[dbmd.tBloque - 1] << endl;
    db.close();*/
}

void DBFunctions::dropDB(string dbName)
{

    if (remove(dbName.c_str()) != 0)
        perror("Error dropping database");
    else
        puts("Database successfully dropped!!");
}

DBMetaData DBFunctions::connectDB(string dbName)
{

    DBMetaData * dbmd = new DBMetaData();
    ifstream newDB(dbName + ".reciodb", ios::in | ios::binary);

    if (!newDB)
    {
        cout << "Database could not be selected";
        return *dbmd;
    }

    newDB.read(reinterpret_cast<char*>(dbmd), sizeof(DBMetaData));

    //cout << dbmd->dbName;
    newDB.close();
    return *dbmd;
}

void DBFunctions::createTable(string dbName, string tName, vector<Column> columns) {

    //fstream db(dbName+".reciodb", ios::out | ios::in | ios::binary);
    ofstream db(dbName + ".reciodb", ios::out | ios::binary | ios::app);

    Table t;
    strcpy_s(t.tName, 30, tName.c_str());
    t.totalColumns = columns.size();

    db.write(reinterpret_cast<const char*>(&t), sizeof(Table));
    for (int i = 0; i < columns.size(); i++)
        db.write(reinterpret_cast<const char*>(&columns[i]), sizeof(Column));

    updtTotalTables(dbName);
    db.close();

    /*
        Metadata
        Bitmap
        Bloques de datos
        Tablas-Columnas
    */
    
    
}

bool DBFunctions::dropTable(string dbName, string tName)
{
    fstream db(dbName+".reciodb", ios::in | ios::out | ios::binary);

    if (!db)
        return false;

    const int COL_TABLE_SIZE = sizeof(Column)+sizeof(Table);

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    db.seekg(sizeof(DBMetaData)+dbmd.bitmapSize+(dbmd.cantBloques*(dbmd.tBloque)), ios::beg);
    cout << db.tellg();
    
    for (int i = 0; i < dbmd.cantTables; i++)
    {
        Table t;
        Column c;
        db.read(reinterpret_cast<char*>(&t), sizeof(Table));
        for (int j = 1; j <= t.totalColumns; j++)
            db.read(reinterpret_cast<char*>(&c), sizeof(Column));

        if (strcmp(t.tName, tName.c_str()) == 0 && !t.flag)
        {
            this->deleteRows(dbName, tName);
            this->flagTable(dbName, tName);
            db.close();
            return true;
        }

    }

    db.close();
    return false;

}

void DBFunctions::printDroppedT(string dbName, string tName)
{
    fstream db(dbName + ".reciodb", ios::in | ios::out | ios::binary);

    if (!db)
        return;

    const int COL_TABLE_SIZE = sizeof(Column) + sizeof(Table);
    int total = 0;

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    db.seekg(sizeof(DBMetaData) + dbmd.bitmapSize + dbmd.cantBloques, ios::beg);

    for (int i = 0; i < dbmd.cantTables; i++)
    {
        Table t;
        Column c;
        db.read(reinterpret_cast<char*>(&t), sizeof(Table));
        for (int j = 1; j <= t.totalColumns; j++)
            db.read(reinterpret_cast<char*>(&c), sizeof(Column));

        if (t.flag == true)
            total++;

    }

    db.close();
    cout << endl << total << endl;

}

void DBFunctions::updtInfoStart(unsigned int infoStartInBytes, string dbName, string tName, int byte)
{
    fstream db(dbName + ".reciodb", ios::in | ios::out | ios::binary);

    if (!db)
        return;

    const int COL_TABLE_SIZE = sizeof(Column) + sizeof(Table);
    int total = 0;

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    db.seekg(sizeof(DBMetaData) + dbmd.bitmapSize + (dbmd.cantBloques * (dbmd.tBloque)), ios::beg);

    for (int i = 0; i < dbmd.cantTables; i++)
    {
        int pos = (int)db.tellg();
        Table t;
        Column c;
        db.read(reinterpret_cast<char*>(&t), sizeof(Table));
        for (int j = 1; j <= t.totalColumns; j++)
            db.read(reinterpret_cast<char*>(&c), sizeof(Column));

        if (strcmp(tName.c_str(), t.tName) == 0) {
            t.infoStart = infoStartInBytes;
            t.byte = byte;
            cout << "\n\t\tInfo start: " << t.infoStart << endl;
            db.seekp(pos, ios::beg);
            db.write(reinterpret_cast<char*>(&t), sizeof(Table));
            break;
        }

    }

    db.close();
}

int DBFunctions::getInfoStart(string dbName, string tName) 
{
    ifstream db(dbName+".reciodb", ios::in | ios::binary);
    int rInfo = 0;

    if (!db) return -1;

    const int COL_TABLE_SIZE = sizeof(Column) + sizeof(Table);
    int total = 0;

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    db.seekg(sizeof(DBMetaData) + dbmd.bitmapSize + (dbmd.cantBloques * (dbmd.tBloque)), ios::beg);

    for (int i = 0; i < dbmd.cantTables; i++)
    {
        int pos = (int)db.tellg();
        Table t;
        Column c;
        db.read(reinterpret_cast<char*>(&t), sizeof(Table));
        for (int j = 1; j <= t.totalColumns; j++)
            db.read(reinterpret_cast<char*>(&c), sizeof(Column));

        if (strcmp(tName.c_str(), t.tName) == 0) {
            rInfo = t.infoStart;
            break;
        }

    }

    db.close();
    return rInfo;

}

void DBFunctions::insert(string dbName, string tName, vector<string> values) {
    //Leer char por char hasta el ultimo si no, se mueve al siguiente bloque
    fstream db(dbName+".reciodb", ios::in | ios::out | ios::binary);

    if (!db) return;

    //Get the table and column data
    vector<Column> c = this->getColumnsFromFile(dbName, tName);
    Table t = this->getTableByName(dbName, tName);
    
    if (t.flag)
    {
        cout << "The table doesn't exist!";
        return;
    }

    //Value count check
    if (values.size() > c.size())
    {
        cout << "Please insert values less or equal then the column count!";
        return;
    }

    bool isChar = false;
    //char* opt;

    //Value data type check
    for (int i = 0; i < values.size(); i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (c[i].dataType[j] == 'c') isChar = true;
        }
        if (c[i].dataType == "int")
        {
            try
            {
                int val = stoi(values[i]);
            }
            catch (const std::exception&)
            {
                cout << endl << "Please insert an int "<< i+1 << "position!";
                return;
            }
        }
        else if (c[i].dataType == "double")
        {
            try
            {
                double val = stod(values[i]);
            }
            catch (const std::exception&)
            {
                cout << endl << "Please insert a double " << i + 1 << "position!";
                return;
            }
        }
        else if(isChar)
        {
            int conv = atoi(c[i].dataType);
            if (values[i].length() > conv)
            {
                cout << endl << "Please check the value type!";
                return;
            }

            string tmp = "";
            string val = values[i];
            for (int j = 0; j < conv; j++)
            {
                if (j < val.length())
                    tmp += val[j];
                else
                    tmp += '_';
            }
            values[i] = tmp;
            
        }
    }

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    BitMap bm;
    bm.bitmap = new char[dbmd.bitmapSize];
    db.read(reinterpret_cast<char*>(bm.bitmap), dbmd.bitmapSize);
    Block b;

    BMFcunctions bFunc(bm);

    if (bFunc.getFreePosBit(dbmd.cantBloques) == -1)
    {
        cout << "The db has no space anymore!";
        return;
    }
    else {
        //Converts the values into a string
        string valueString = "";

        for (size_t i = 0; i < values.size(); i++) {
            valueString += values[i];
            valueString += ',';
        }

        valueString += ';';

        int free = bFunc.getFreePosBit(dbmd.cantBloques);
        Block b;
        b.data = new char[dbmd.tBloque-4];

        unsigned int infoStartInBytes = 0;
        if (t.infoStart != 0)
            db.seekg(t.infoStart, ios::beg);
        else
        {
            db.seekg(sizeof(DBMetaData) + dbmd.bitmapSize + (free * dbmd.tBloque), ios::beg);
            infoStartInBytes = (int)db.tellg();
            setBlockOn_Off(dbName, free, true);
            setFBlock(dbName, tName, free);
            updtInfoStart(infoStartInBytes, dbName, tName, 0);
        }

        db.read(reinterpret_cast<char*>(b.data), dbmd.tBloque-4);

        int remainingSize = valueString.length();
        int j = 0;

        int firstWrite = 0;
        bool fWrite = false;
        bool fBlock = true;
        bool first = false;
        int reads = 0;
        Table actTable = this->getTableByName(dbName, tName);
        for (int i = 0; i < dbmd.tBloque-4; i++)
        {
            if (j == remainingSize)
                break;

            if (b.data[i] == '-') {
                if (!fWrite)
                {
                    firstWrite = i;
                    fWrite = true;

                }
                b.data[i]  = valueString[j];
                j++;
            }
            if ((i == dbmd.tBloque-5))
            {
                reads++;
                if (reads == 2)
                {
                    fBlock = false;
                }
                //Escribo lo que tengo actualmente
                if(fBlock)
                {
                    db.seekp(actTable.infoStart, ios::beg);
                    db.write(reinterpret_cast<const char*>(b.data), dbmd.tBloque - 4);
                }
                else
                {
                    db.seekp(sizeof(DBMetaData) + dbmd.bitmapSize + (free * dbmd.tBloque), ios::beg);
                    db.write(reinterpret_cast<const char*>(b.data), dbmd.tBloque - 4);
                }

                if (!first)
                {
                    first = true;
                    fBlock = false;
                }

                int actPos = getPos(dbName, actTable.infoStart, reads);


                
                if (actPos == -1)
                {
                    BitMap rBM;
                    rBM.bitmap = new char[dbmd.bitmapSize];
                    rBM.bitmap = getBitmapFromFile(dbName);

                    BMFcunctions funcNew(rBM);


                    free = funcNew.getFreePosBit(dbmd.cantBloques);

                    //Pongo el bitmap en setOn y actualizo el apuntador al siguiente
                    setBlockOn_Off(dbName, free, true);
                    //Write the new free position
                    int newPos = free * dbmd.tBloque;
                    db.write(reinterpret_cast<const char*>(&newPos), sizeof(int));
                    //Go to the new block and restart the i
                    db.seekg(sizeof(DBMetaData) + dbmd.bitmapSize + (newPos), ios::beg);
                    db.read(reinterpret_cast<char*>(b.data), dbmd.tBloque - 4);
                    i = -1;
                }
                else
                {
                    db.seekg(sizeof(DBMetaData) + dbmd.bitmapSize + (actPos), ios::beg);
                    db.read(reinterpret_cast<char*>(b.data), dbmd.tBloque - 4);
                    i = -1;
                }
                
            }
        }
        /*
        =============================
            
        =============================
        */
        this->updtTableRows(dbName, tName, true, false);
        //if (t.infoStart == 0)
        //{
        //    
        //    db.seekp(sizeof(DBMetaData) + dbmd.bitmapSize + (free * dbmd.tBloque), ios::beg);
        //    db.write(reinterpret_cast<const char*>(b.data), dbmd.tBloque - 4);
        //}
        if(fBlock)
        {
            db.seekp(actTable.infoStart, ios::beg);
            db.write(reinterpret_cast<const char*>(b.data), dbmd.tBloque - 4);
        }
        else
        {
            db.seekp(sizeof(DBMetaData) + dbmd.bitmapSize + (free * dbmd.tBloque), ios::beg);
            db.write(reinterpret_cast<const char*>(b.data), dbmd.tBloque - 4);
        }

    }

    db.close();

}

void DBFunctions::deleteRows(string dbName, string tName)
{
    //Leer char por char hasta el ultimo si no, se mueve al siguiente bloque
    fstream db(dbName + ".reciodb", ios::in | ios::out | ios::binary);

    if (!db) return;

    //Get the table and column data
    vector<Column> c = this->getColumnsFromFile(dbName, tName);
    Table t = this->getTableByName(dbName, tName);

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    BitMap bm;
    bm.bitmap = new char[dbmd.bitmapSize];
    db.read(reinterpret_cast<char*>(bm.bitmap), dbmd.bitmapSize);

    BMFcunctions bFunc(bm);

    if (t.rows == 0)
    {
        cout << "La tabla no tiene registros!";
        return;
    }

    if (t.flag)
    {
        cout << "La tabla no existe!";
        return;
    }

    if (t.infoStart == 0)
    {
        cout << "The table has no rows!";
        db.close();
        return;
    }
    else {
        Block b;
        b.data = new char[dbmd.tBloque - 4];

        db.seekg(t.infoStart, ios::beg);
        db.read(reinterpret_cast<char*>(b.data), dbmd.tBloque - 4);

        bool begin = false;
        int pc = 0;
        for (int i = 0; i < dbmd.tBloque - 4; i++)
        {
            if (i == t.byte)
                begin = true;
            if (begin)
            {
                if (b.data[i] == ';')
                {
                    pc++;
                }
                b.data[i] = '-';
            }
            if (pc == t.rows) break;
        }
        
        this->updtTableRows(dbName, tName, true, true);
        db.seekp(t.infoStart, ios::beg);
        db.write(reinterpret_cast<const char*>(b.data), dbmd.tBloque - 4);

    }

    setBlockOn_Off(dbName, t.firstBlock, false);
    db.close();
}

void DBFunctions::deleteRowsCondition(string dbName, string tName, string condition)
{
    //Leer char por char hasta el ultimo si no, se mueve al siguiente bloque
    fstream db(dbName + ".reciodb", ios::in | ios::out | ios::binary);

    if (!db) return;

    //Get the table and column data
    vector<Column> c = this->getColumnsFromFile(dbName, tName);
    Table t = this->getTableByName(dbName, tName);

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    BitMap bm;
    bm.bitmap = new char[dbmd.bitmapSize];
    db.read(reinterpret_cast<char*>(bm.bitmap), dbmd.bitmapSize);

    BMFcunctions bFunc(bm);

    vector<string> conditionV = this->conditionToVector(condition);

    if (t.flag)
    {
        cout << "La tabla no existe!";
        return;
    }

    if (t.infoStart == 0)
    {
        cout << "The table has no rows!";
        db.close();
        return;
    }
    else {
        Block b;
        b.data = new char[dbmd.tBloque - 4];

        int columnNumberInArray = 0;
        for (int i = 0; i < c.size(); i++)
        {
            if (strcmp(c[i].cName, conditionV[0].c_str()) == 0) columnNumberInArray = i;
        }

        db.seekg(t.infoStart, ios::beg);
        db.read(reinterpret_cast<char*>(b.data), dbmd.tBloque - 4);

        bool beginRead = true, begin = false, first = false;
        int pc = 0;
        int commas = 0;
        int rowsCovered = 0;
        string buffer = "";
        int rowStart = 0;
        for (int i = 0; i < dbmd.tBloque - 4; i++)
        {
            if (b.data[i] == ';' && begin)
            {
                b.data[i] = '-';
                pc++;
                beginRead = true;
                begin = false;
                rowsCovered++;
                commas = 0;
                buffer = "";
                rowStart = 0;
                first = false;
            }
            if ((columnNumberInArray == commas) && beginRead)
                beginRead = true;
            if (b.data[i] == ',')
                commas++;
            if (beginRead)
            {
                if ((b.data[i] != ',' && b.data[i] != '-') && b.data[i] != ';')
                {
                    if (!first)
                    {
                        rowStart = i;
                        first = true;
                    }
                    buffer += b.data[i];
                }
                
            }
            if ((buffer == conditionV[1])&&(b.data[i] == ','&&!begin))
            {
                beginRead = false;
                begin = true;
                i = rowStart;
                buffer = "";
            }
            else if ((buffer != conditionV[1]) && b.data[i] == ',')
            {
                first = false;
                rowStart = 0;
                buffer = "";
                rowsCovered++;
            }
            if ((begin && !beginRead))
                    b.data[i] = '-';
            if (pc == t.rows) break;
        }

        this->updtTableRows(dbName, tName, true, true);
        db.seekp(sizeof(DBMetaData) + dbmd.bitmapSize + (t.infoStart), ios::beg);
        db.write(reinterpret_cast<const char*>(b.data), dbmd.tBloque - 4);

    }

    db.close();
}

void DBFunctions::selectAll(string dbName, string tName, string condition) {
    //Leer char por char hasta el ultimo si no, se mueve al siguiente bloque
    ifstream db(dbName + ".reciodb", ios::in | ios::binary);

    if (!db) return;

    //Get the table and column data
    vector<Column> c = this->getColumnsFromFile(dbName, tName);
    Table t = this->getTableByName(dbName, tName);

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    BitMap bm;
    bm.bitmap = new char[dbmd.bitmapSize];
    db.read(reinterpret_cast<char*>(bm.bitmap), dbmd.bitmapSize);
    Block b;

    BMFcunctions bFunc(bm);

    if (t.flag)
    {
        cout << "La tabla no existe!";
        return;
    }

    if (t.infoStart == 0)
    {
        cout << "The table has no rows!";
        db.close();
        return;
    }
    else {
        if (condition == "all")
        {
            cout << endl;
            for (int i = 0; i < c.size(); i++)
            {
                cout << c[i].cName;
                if (i != (c.size() - 1))
                    cout << "\t | \t";
            }
            cout << endl;

            Block b;
            b.data = new char[dbmd.tBloque - 4];
            db.seekg(t.infoStart, ios::beg);
            db.read(reinterpret_cast<char*>(b.data), dbmd.tBloque - 4);
            int pc = 0;

            for (int i = 0; i < dbmd.tBloque - 4; i++)
            {
                if (pc == t.rows)
                    break;

                if (b.data[i] == '-')
                    continue;
                if (b.data[i] != ',' && b.data[i] != ';')
                {
                    if(b.data[i] != '_')
                        cout << b.data[i];
                }  
                if (b.data[i] == ',' && b.data[i + 1] != ';')
                    cout << "\t | \t";
                if (b.data[i] == ';')
                    cout << endl;
            }
        }
        else
        {
            vector<string> conditionVector = this->conditionToVector(condition);

            int columnNumberInArray = 0;
            for (int i = 0; i < c.size(); i++)
            {
                if (strcmp(c[i].cName, conditionVector[0].c_str()) == 0) columnNumberInArray = i;
            }

            cout << endl;
            for (int i = 0; i < c.size(); i++)
            {
                cout << c[i].cName;
                if (i != (c.size() - 1))
                    cout << "\t | \t";
            }
            cout << endl;

            Block b;
            b.data = new char[dbmd.tBloque - 4];
            db.seekg(t.infoStart, ios::beg);
            db.read(reinterpret_cast<char*>(b.data), dbmd.tBloque - 4);
            int pc = 0;
            int commas = 0;
            string buffer = "";
            bool equal = false;
            vector<string>* tmp = new vector<string>;
            for (int i = 0; i < dbmd.tBloque - 4; i++)
            {
                if (pc == t.rows)
                    break;

                if (b.data[i] == ',')
                {
                    tmp->push_back(buffer);
                    buffer = "";
                    commas++;
                }
                if (b.data[i] == '-')
                    continue;
                if (b.data[i] != ',' && b.data[i] != ';')
                {
                    if (b.data[i] != '_')
                    {
                        buffer += b.data[i];
                    }
                }
                if ((b.data[i] != ',' && b.data[i] != ';') && commas == (columnNumberInArray))
                {
                    if (buffer == conditionVector[1])
                        equal = true;
                } 
                if (b.data[i] == ',' && b.data[i + 1] == ';')
                {
                    if (equal)
                    {
                        for (int i = 0; i < tmp->size(); i++)
                        {
                            cout << tmp->at(i);
                            cout << "\t | \t";
                        }
                    }
                }
                if (b.data[i] == ';')
                {
                    if (equal)
                        cout << endl;
                    buffer = "";
                    equal = false;
                    commas = 0;
                    tmp = new vector<string>;
                } 
            }
        }

    }

    db.close();
}

void DBFunctions::select(string dbName, string tName, vector<string> columnsToShow, string condition)
{
    ifstream db(dbName + ".reciodb", ios::in | ios::binary);

    if (!db) return;

    //Get the table and column data
    vector<Column> c = this->getColumnsFromFile(dbName, tName);
    Table t = this->getTableByName(dbName, tName);

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    BitMap bm;
    bm.bitmap = new char[dbmd.bitmapSize];
    db.read(reinterpret_cast<char*>(bm.bitmap), dbmd.bitmapSize);
    Block b;

    BMFcunctions bFunc(bm);

    if (t.flag)
    {
        cout << "La tabla no existe!";
        return;
    }

    if (t.infoStart == 0)
    {
        cout << "The table has no rows!";
        db.close();
        return;
    }
    else
    {
        if (condition == "all")
        {
            vector<int> columnNumberInArray;
            for (int i = 0; i < c.size(); i++)
            {
                for (int j = 0;  j < columnsToShow.size();  j++)
                    if (strcmp(c[i].cName, columnsToShow[j].c_str()) == 0) columnNumberInArray.push_back(i);
            }

            cout << endl;
            for (int i = 0; i < columnsToShow.size(); i++)
            {
                cout << columnsToShow[i];
                if (i != (c.size() - 1))
                    cout << "\t | \t";
            }
            cout << endl;

            Block b;
            b.data = new char[dbmd.tBloque - 4];
            db.seekg(t.infoStart, ios::beg);
            db.read(reinterpret_cast<char*>(b.data), dbmd.tBloque - 4);
            int pc = 0;

            vector<string>* tmp = new vector<string>;
            string buffer = "";
            int commas = 0;
            bool equal = false;

            for (int i = 0; i < dbmd.tBloque - 4; i++)
            {
                if (pc == t.rows)
                    break;

                if (b.data[i] == ',')
                {
                    if(buffer != "")
                        tmp->push_back(buffer);

                    buffer = "";
                    commas++;
                }
                if (b.data[i] == '-')
                    continue;
                /*if (b.data[i] != ',' && b.data[i] != ';')
                    buffer += b.data[i];*/
                if ((b.data[i] != ',' && b.data[i] != ';'))
                {
                    for (int j = 0; j < columnNumberInArray.size(); j++)
                    {
                        if (commas == columnNumberInArray[j])
                        {
                            if(b.data[i] != '_')
                                buffer += b.data[i];
                        }
                    }
                }
                if (b.data[i] == ',' && b.data[i + 1] == ';')
                {
                    for (int i = 0; i < tmp->size(); i++)
                    {
                        cout << tmp->at(i);
                        cout << "\t | \t";
                    }
                }
                if (b.data[i] == ';')
                {
                    cout << endl;
                    buffer = "";
                    equal = false;
                    commas = 0;
                    tmp = new vector<string>;
                    pc++;
                }
            }
        }
        else
        {
            vector<string> conditionVector = this->conditionToVector(condition);

            int columnNumberInArrayCondition = 0;
            for (int i = 0; i < c.size(); i++)
            {
                if (strcmp(c[i].cName, conditionVector[0].c_str()) == 0) columnNumberInArrayCondition = i;
            }
            
            vector<int> columnNumberInArray;
            for (int i = 0; i < c.size(); i++)
            {
                for (int j = 0; j < columnsToShow.size(); j++)
                    if (strcmp(c[i].cName, columnsToShow[j].c_str()) == 0) columnNumberInArray.push_back(i);
            }

            cout << endl;
            for (int i = 0; i < columnsToShow.size(); i++)
            {
                cout << columnsToShow[i];
                if (i != (c.size() - 1))
                    cout << "\t | \t";
            }
            cout << endl;

            Block b;
            b.data = new char[dbmd.tBloque - 4];
            db.seekg(t.infoStart, ios::beg);
            db.read(reinterpret_cast<char*>(b.data), dbmd.tBloque - 4);
            int pc = 0;

            vector<string>* tmp = new vector<string>;
            string buffer = "";
            int commas = 0;
            bool equal = false;

            for (int i = 0; i < dbmd.tBloque - 4; i++)
            {
                if (pc == t.rows)
                    break;

                if (b.data[i] == ',')
                {
                    if (equal == true)
                    {
                        if(buffer != "")
                            tmp->push_back(buffer);
                    }

                    buffer = "";
                    commas++;
                }
                if (b.data[i] == '-')
                    continue;
                /*if (b.data[i] != ',' && b.data[i] != ';')
                    buffer += b.data[i];*/
                if ((b.data[i] != ',' && b.data[i] != ';'))
                {
                    for (int j = 0; j < columnNumberInArray.size(); j++)
                    {
                        if (commas == columnNumberInArray[j])
                        {
                            if(b.data[i] != '_')
                                buffer += b.data[i];
                        }
                        if (commas == columnNumberInArrayCondition)
                        {
                            if (b.data[i] != '_')
                                buffer += b.data[i];
                            if (buffer == conditionVector[1])
                            {
                                buffer = "";
                                equal = true;
                            }
                        }
                    }
                }
                if (b.data[i] == ',' && b.data[i + 1] == ';')
                {
                    if (equal)
                    {
                        for (int i = 0; i < tmp->size(); i++)
                        {
                            cout << tmp->at(i);
                            cout << "\t | \t";
                        }
                    }
                }
                if (b.data[i] == ';')
                {
                    if(equal == true)
                        cout << endl;
                    buffer = "";
                    equal = false;
                    commas = 0;
                    tmp = new vector<string>;
                    pc++;
                }
            }
        }
    }
}

void DBFunctions::update(string dbName, string tName, vector<string> set, string condition)
{
    fstream db(dbName+".reciodb", ios::in | ios::out | ios::binary);

    if (!db) return;

    //Get the table and column data
    vector<Column> c = this->getColumnsFromFile(dbName, tName);
    Table t = this->getTableByName(dbName, tName);

    if (t.flag)
    {
        cout << "The table doesn't exist!";
        return;
    }
    
    int columnNumberInArray = 0;
    for (int i = 0; i < c.size(); i++)
    {
        if (strcmp(c[i].cName, set[0].c_str()) == 0) columnNumberInArray = i;
    }

    DBMetaData dbmd;
    db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));

    Block b;
    b.data = new char[dbmd.tBloque-4];
    db.seekg(t.infoStart, ios::beg);

    if (condition == "all")
    {
        int commas = 0;
        string newVal = set[1];
        int j = 0;
        int pc = 0;
        int end = false;
        db.read(reinterpret_cast<char*>(b.data), dbmd.tBloque - 4);
        for (int i = 0; i < dbmd.tBloque - 4; i++)
        {
            if (pc == t.rows)
            {
                b.data[i] = '-';
                end = true;
            }
            if (b.data[i] == ',' && !end)
                commas++;
            if (commas == columnNumberInArray)
            {
                if ((b.data[i] != ',' && b.data[i] != ' ') && !end) {
                    b.data[i] = newVal[j];
                    j++;
                }
            }
            if (b.data[i] == ';' && !end)
            {
                pc++;
                commas = 0;
                j = 0;
            }
        }
    }
    else
    {

        vector<string> conditionVector = this->conditionToVector(condition);

        int columnNumberInArrayCondition = 0;
        for (int i = 0; i < c.size(); i++)
        {
            if (strcmp(c[i].cName, conditionVector[0].c_str()) == 0) columnNumberInArrayCondition = i;
        }

        int commas = 0;
        string newVal = set[1];
        int j = 0;
        int pc = 0;
        bool end = false;
        bool read = true;
        int rowStart = 0;
        bool first = false;
        string buffer = "";
        db.read(reinterpret_cast<char*>(b.data), dbmd.tBloque - 4);
        for (int i = 0; i < dbmd.tBloque - 4; i++)
        {
            if (pc == t.rows)
            {
                b.data[i] = '-';
                end = true;
            }
            if (b.data[i] == ',' && !end)
                commas++;
            if (commas == columnNumberInArray)
            {
                if (!first && read)
                {
                    first = true;
                    rowStart = i-1;
                    pc--;
                }
                if ((read&&!end) && (b.data[i] != ',' && b.data[i] != ' '))
                {
                    buffer += b.data[i];
                }
                if ((b.data[i] != ',' && b.data[i] != ' ') && (!end&&!read)) {
                    b.data[i] = newVal[j];
                    j++;
                }

                if ((buffer == conditionVector[1]) && read)
                {
                    read = false;
                    commas = 0;
                    i = rowStart;
                    continue;
                }

            }
            if (b.data[i] == ';' && !end)
            {
                pc++;
                commas = 0;
                read = true;
                first = false;
                rowStart = 0;
                j = 0;
                buffer = "";
            }
        }
    }

    db.seekp(t.infoStart, ios::beg);
    db.write(reinterpret_cast<const char*>(b.data), dbmd.tBloque - 4);

    db.close();

}

//for (int i = 0; i < dbmd.tBloque - 4; i++)
//{
//    if (pc == t.rows)
//        break;
//
//    if (b.data[i] == ',')
//        commas++;
//    if (b.data[i] == '-')
//        continue;
//    if ((b.data[i] != ',' && b.data[i] != ';') && commas == columnNumberInArray)
//    {
//        buffer += b.data[i];
//        if (buffer == conditionVector[1])
//        {
//            cout << b.data[i];
//        }
//    }
//    if (b.data[i] == ',' && b.data[i + 1] != ';')
//    {
//        cout << "\t | \t";
//    }
//    if (b.data[i] == ';')
//        cout << endl;
//}

//void DBFunctions::oldTable() {
//    //fstream db("nueva.reciodb", ios::in | ios::out | ios::binary);
//
//    //Table t;
//    //strcpy_s(t.tName, "Arturo");
//
//    //if (!db)
//    //{
//    //    cout << "Error opening db!!";
//    //    return;
//    //}
//
//    //DBMetaData dbmd;
//    //db.read(reinterpret_cast<char*>(&dbmd), sizeof(DBMetaData));
//    //cout << dbmd.bitmapSize << endl;
//
//    //BitMap bm;
//    //bm.bitmap = new char[dbmd.bitmapSize];
//    //db.read(reinterpret_cast<char*>(bm.bitmap), dbmd.bitmapSize);//Procede a leer el bitmap con su respectiva estructura
//
//    //BMFcunctions bmFunc(bm);//El constructor iguala el bitmap de la clase al bitmap que se leyo del archivo
//
//    //if (bmFunc.getFreePosBit(dbmd.cantBloques) == -1)
//    //{
//    //    cout << "The db has no space anymore!";
//    //    return;
//    //}
//    //else {
//    //    int free = bmFunc.getFreePosBit(dbmd.cantBloques);
//    //    Block b;
//    //    b.data = new char[dbmd.tBloque];
//
//    //    db.seekg(sizeof(DBMetaData) + dbmd.bitmapSize + (free * dbmd.tBloque), ios::beg);
//    //    db.read(reinterpret_cast<char*>(b.data), dbmd.tBloque);
//
//    //    for (int i = 0; i < dbmd.tBloque - 4; i++) {
//    //        if (i < 30)
//    //            b.data[i] = t.tName[i];
//    //        else
//    //            b.data[i] = '-';
//    //    }
//
//    //    db.seekp(sizeof(DBMetaData) + dbmd.bitmapSize + (free * dbmd.tBloque), ios::beg);
//    //    db.write(reinterpret_cast<const char*>(b.data), dbmd.tBloque - 4);
//
//    //    //--------- BITMAP --------
//    //    //Set on para proceder a actualizar el bitmap
//    //    bmFunc.setOn(free);
//    //    db.seekp(sizeof(DBMetaData), ios::beg);
//    //    //Se actualiza el bitmap
//    //    db.write(reinterpret_cast<const char*>(bmFunc.bm.bitmap), dbmd.bitmapSize);
//    //    bmFunc.printNewBM(dbmd.bitmapSize);
//    //}
//
//    //db.close();
//}


