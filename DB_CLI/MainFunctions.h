#pragma once
#include <string>
using std::string;

class MainFunctions {
public:
	vector<string> getSet(string);
	string getWhere(string);
	string getCommand(string);
	string getCommType(string);
	string getDBName(string);
	string getDBSize(string);
	int getBytes(string);
	char getGigaMega(string);
	int getBlockSize(string);
	vector<Column> getColumns(vector<string>);
	vector<string> getColumnStrings(string);
	vector<string> getValues(string);
	bool hasWhere(string);
	string getTableName(string);
	vector<string> columnsToVector(string);
};

string MainFunctions::getCommand(string iString)
{
	string rCommand = "";

	for (int i = 0; i < iString.length(); i++)
	{
		if (iString[i] != ' ')
			rCommand += iString[i];
		else if (iString[i] == ' ')
			return rCommand;

	}

	return "";

}

string MainFunctions::getCommType(string iString)
{
	string CreatyType = "";
	int spaces = 0;

	for (int i = 0; i < iString.length(); i++)
	{
		if (iString[i] == ' ')
			spaces++;
		else if (spaces == 1)
		{
			CreatyType += iString[i];
		}
		else if (spaces == 2)
			break;

	}

	return CreatyType;

}

string MainFunctions::getDBName(string iString)
{
	string rName = "";
	int spaces = 0;

	for (int i = 0; i < iString.length(); i++)
	{
		if (spaces == 2 && iString[i] == '(')
			break;
		if (iString[i] == ' ')
		{
			spaces++;
		}
		else if (spaces == 2)
		{
			rName += iString[i];
		}

	}

	return rName;
}

string MainFunctions::getDBSize(string iString)
{
	string rName = "";
	int spaces = 0;

	for (int i = 0; i < iString.length(); i++)
	{
		if (iString[i] == ' ')
			spaces++;
		else if (spaces == 3)
			rName += iString[i];

	}

	return rName;
}

int MainFunctions::getBytes(string size)
{
	string rName = "";

	for (int i = 0; i < size.length(); i++)
	{
		if (size[i] != 'M' || size[i] != 'm' || size[i] != 'G' || size[i] != 'g')
			rName += size[i];
		else
			break;
	}

	return stoi(rName);

}

char MainFunctions::getGigaMega(string size)
{
	char rChar = ' ';
	for (int i = 0; i < size.length(); i++)
	{
		if (size[i] == 'M' || size[i] == 'm' || size[i] == 'G' || size[i] == 'g')
		{
			rChar = size[i];
			break;
		}

	}

	return rChar;

}

int MainFunctions::getBlockSize(string sB)
{
	string rSize = "";
	int spaces = 0;

	for (int i = 0; i < sB.length(); i++)
	{
		if (sB[i] == ' ')
			spaces++;
		else if (spaces == 4)
			rSize += sB[i];

	}

	return stoi(rSize);

}

vector<string> MainFunctions::getColumnStrings(string input) {
	vector<string> rVector;
	string col;
	bool parenthesis = false;
	for (int i = 0; i < input.length(); i++)
	{
		if (input[i] == '(') {
			parenthesis = true;
			continue;
		}
		
		if (parenthesis && input[i]!=',')
			if (input[i]!=')')
				col += input[i];

		if (input[i] == ',' || input[i] == ')')
		{
			rVector.push_back(col);
			col = "";
		}
	}

	return rVector;

}

vector<Column> MainFunctions::getColumns(vector<string> iVector) {
	//create table hola(name char(12),id int,salary double)
	vector<Column> rVector;
	bool space = false;
	for (int i = 0; i < iVector.size(); i++)
	{
		string cName = "", cType = "";
		string colString = iVector[i];
		Column col;

		if (colString == "")
			continue;

		for (int i = 0; i < colString.length(); i++)
		{
			if (colString[i] == ' ' && i == 0)
				continue;

			if (colString[i] == ' ' && i != 0)
				space = true;

			if (!space)
			{
				if (colString[i]!=' ')
					cName += colString[i];
				
			}
			else if(space)
			{
				if (colString[i] != ' ')
					cType += colString[i];
			}
		}
		strcpy_s(col.cName, 30, cName.c_str());
		strcpy_s(col.dataType, 8, cType.c_str());
		rVector.push_back(col);
		space = false;
	}

	return rVector;

}

vector<string> MainFunctions::getValues(string input) {
	//Insert into table_name values(values,values)
	vector<string> rVector;
	string value = "";
	bool parenthesis = false;

	for (int i = 0; i < input.length(); i++)
	{
		if (input[i]=='(')
		{
			parenthesis = true;
			continue;
		}

		if ((input[i] == ',' || input[i] == ')') && parenthesis)
		{
			rVector.push_back(value);
			value = "";
		}
		else if(parenthesis)
			value += input[i];

	}

	return rVector;

}

bool MainFunctions::hasWhere(string input) {
	//DELETE FROM table_name WHERE condition
	bool rBool = false;
	bool w = false;
	string rsString = "";

	for (int i = 0; i < input.length(); i++)
	{
		if ((input[i] == 'w' || input [i] == 'W') && !w)
		{
			rsString += input[i];
			w = true;
			continue;
		}
		if (w)
		{
			if (input[i] == ' ') break;
			rsString += input[i];
		}
	}

	if (rsString == "WHERE" || rsString == "where") rBool = true;

	return rBool;

}

string MainFunctions::getTableName(string input)
{
	//Select * FROM tablename
	//SELECT nose, asdfa FROM tablename
	string rString = "";
	bool maybeFrom = false;
	bool isFrom = false;
	bool exit = false;
	for (int i = 0; i < input.length(); i++)
	{
		char wat = input[i];
		if ((input[i] == 'F' || input[i] == 'f') && !isFrom)
			maybeFrom = true;
		if (maybeFrom)
			if (input[i] != ' ')
				rString += input[i];
		if (maybeFrom && input[i] == ' ')
		{
			if (rString == "from" || rString == "FROM")
			{
				isFrom = true;
				maybeFrom = false;
				rString = "";
			}
			else
			{
				rString = "";
				maybeFrom = false;
			}
		}

		if (isFrom) {
			if (exit && input[i] == ' ')
				break;
			if (input[i] != ' ')
			{
				rString += input[i];
				exit = true;
			}
			else
				continue;
		}

	}

	return rString;

}

vector<string> MainFunctions::getSet(string input) {
	//UPDATE table SET newthing WHERE this
	vector<string> rVector;
	string rString = "";
	int spaces = 0;

	for (int i = 0; i < input.length(); i++)
	{
		if (spaces == 4) break;
		if (input[i] == ' ')
		{
			spaces++;
			continue;
		}
		if (spaces == 3) {
			if (input[i]=='=')
			{
				rVector.push_back(rString);
				rString = "";
				continue;
			}
			rString += input[i];
		}
	}
	rVector.push_back(rString);
	return rVector;
}

string MainFunctions::getWhere(string input)
{
	string rString = "";
	bool maybeWhere = false;
	bool isWhere = false;

	for (int i = 0; i < input.length(); i++)
	{
		char wat = input[i];
		if ((input[i] == 'w' || input[i] == 'W') && !isWhere)
			maybeWhere = true;
		if (maybeWhere)
			if (input[i] != ' ')
				rString += input[i];
		if (maybeWhere && input[i] == ' ')
		{
			if (rString == "WHERE" || rString == "where")
			{
				isWhere = true;
				maybeWhere = false;
				rString = "";
			}
			else
			{
				rString = "";
				maybeWhere = false;
			}
		}

		if (isWhere) {
			if (input[i] != ' ')
				rString += input[i];
			else
				continue;
		}

		if (isWhere && input[i] == ' ')
		{
			break;
		}

	}

	return rString;
}

vector<string> MainFunctions::columnsToVector(string input)
{
	vector<string> rVector;
	bool space = false;
	string bufferString = "";

	for (int i = 0; i < input.length(); i++)
	{
		if ((input[i + 1] == 'F' || input[i + 1] == 'f') && (input[i + 2] == 'R' || input[i + 2] == 'r'))
		{
			rVector.push_back(bufferString);
			break;
		}

		if (input[i] == ' ')
		{
			space = true;
			continue;
		}

		if (input[i] == ',' && space)
		{
			rVector.push_back(bufferString);
			bufferString = "";
		}
		else if(input[i] != ',' && space)
			bufferString += input[i];

	}

	return rVector;

}
