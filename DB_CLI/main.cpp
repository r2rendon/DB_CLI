#include "DBFunctions.h"
#include <string.h>
#include "MainFunctions.h"

using namespace std;

int main()
{
	bool exit = false;
	string db = "", input;
	DBFunctions functions;
	MainFunctions mf;

	while (!exit)
	{
		string comm = "", name = "";

		if (db == "")
		{
			cout << "\n\tNO DB IS SELECTED\n";
		}

		cout << "\n<" + db + "> ";
		getline(cin, input);

		comm = mf.getCommand(input);
		for (int i = 0; i < comm.length(); i++)
			comm[i] = toupper(comm[i]);

		name = mf.getDBName(input);

		if (comm == "s" || comm == "S")
			exit = true;
		else if (db == "")
		{
			if (comm == "CONNECT" && (mf.getCommType(input) == "DB" || mf.getCommType(input) == "db" || mf.getCommType(input) == "Db"))
			{
				cout << "wat";
				DBMetaData rMD = functions.connectDB(name);
				cout << rMD.dbName;
				db = rMD.dbName;
			}
			else if (comm == "CREATE")
			{
				//cout << getCreateType(input);
				if (mf.getCommType(input) == "DATABASE" || mf.getCommType(input) == "database" || mf.getCommType(input) == "Database")
				{
					char gorb = mf.getGigaMega(mf.getDBSize(input));
					if (mf.getBlockSize(input) <= 2048 && mf.getBlockSize(input) >= 512)
					{
						if (gorb == 'M' || gorb == 'm')
						{
							DBMetaData dbmd;
							strcpy_s(dbmd.dbName, name.c_str());
							dbmd.dbSizeInBytes = (mf.getBytes(mf.getDBSize(input))*1024)*1024;
							dbmd.tBloque = mf.getBlockSize(input);
							dbmd.cantBloques = dbmd.dbSizeInBytes / dbmd.tBloque;
							dbmd.bitmapSize = dbmd.cantBloques/8;

							functions.createDB(dbmd);
							//functions.getLastData(dbmd.dbName);
						}
						else if (gorb == 'G' || gorb == 'g')
						{
							DBMetaData dbmd;
							strcpy_s(dbmd.dbName, name.c_str());
							dbmd.dbSizeInBytes = ((mf.getBytes(mf.getDBSize(input)) * 1024) * 1024)*1024;
							dbmd.tBloque = mf.getBlockSize(input);
							dbmd.cantBloques = dbmd.dbSizeInBytes / dbmd.tBloque;
							dbmd.bitmapSize = dbmd.cantBloques / 8;

							functions.createDB(dbmd);
						}
						else
						{
							cout << "\t\nSize not supported in database";
						}
					}
					else
						cout << "\t\nBLOCK SIZE NOT SUPPORTED!\n";
				}
			}
			else if (comm == "DROP")
			{
				if (mf.getCommType(input) == "DATABASE" || mf.getCommType(input) == "database" || mf.getCommType(input) == "Database")
					functions.dropDB(name + ".reciodb");
			}
		}
		else {
			if (comm == "DROP" && (mf.getCommType(input) == "TABLE" || mf.getCommType(input) == "Table" || mf.getCommType(input) == "table")) {
				bool rs = functions.dropTable(db, name);
				(rs ? cout << "\nTable Dropped!\n" : cout << "\nTable Could not be dropped\n");
			}
			else if (comm == "DISCONNECT")
				db = "";
			else if (comm == "CREATE" && (mf.getCommType(input) == "TABLE" || mf.getCommType(input) == "Table" || mf.getCommType(input) == "table")) {
				vector<string> columnStrings = mf.getColumnStrings(input);
				vector<Column> columns = mf.getColumns(columnStrings);
				functions.createTable(db, name, columns);
			}
			else if (comm == "INSERT" && (mf.getCommType(input) == "INTO" || mf.getCommType(input) == "into" || mf.getCommType(input) == "Into")) {
				vector<string> values = mf.getValues(input);
				functions.insert(db, name, values);
			}
			else if (comm == "DELETE" && !mf.hasWhere(input))
			{
				functions.deleteRows(db, name);
			}
			else if (comm == "DELETE" && mf.hasWhere(input))
			{
				functions.deleteRowsCondition(db, name, mf.getWhere(input));
			}
			else if (comm == "SELECT" && mf.getCommType(input) == "*")
			{
				if (mf.hasWhere(input))
				{
					string tName = mf.getTableName(input);
					if (tName == "")
						cout << "Please write correctly the FROM";
					else
						functions.selectAll(db, tName, mf.getWhere(input));
				}
				else {
					string tName = mf.getTableName(input);
					if (tName == "")
						cout << "Please write correctly the FROM";
					else
						functions.selectAll(db, tName, "all");
				}
			}
			else if (comm == "SELECT" && mf.getCommType(input) != "*")
			{
				string tName = mf.getTableName(input);
				if (!mf.hasWhere(input))
					functions.select(db, tName, mf.columnsToVector(input), "all");
				else
					functions.select(db, tName, mf.columnsToVector(input), mf.getWhere(input));
			}
			else if (comm == "UPDATE" && (name == "set" || name == "SET"))
			{
				//Este sera la tabla: mf.getCommType(input);
				if (mf.hasWhere(input))
				{
					functions.update(db, mf.getCommType(input), mf.getSet(input), mf.getWhere(input));
				}
				else
					functions.update(db, mf.getCommType(input), mf.getSet(input), "all");
			}
			else
				cout << "\n\tCommand not found" << endl;
		}

	}
}