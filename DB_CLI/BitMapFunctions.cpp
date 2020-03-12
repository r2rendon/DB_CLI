#include "BitMapFunctions.h"

BMFcunctions::BMFcunctions() {

}

BMFcunctions::BMFcunctions(int bmSize) {
	this->bm.bitmap = new char[bmSize];
}

BMFcunctions::BMFcunctions(BitMap bm) {
	this->bm = bm;
}

void BMFcunctions::initBitMap(int DBAmount) {
	for (int i = 0; i < DBAmount / 8; i++)
		this->bm.bitmap[i] = 0;
}

int BMFcunctions::getFreePosBit(int nBlocks) {
	const int SHIFT = 8 * sizeof(char) - 1;
	char MASK = 1 << SHIFT;

	for (int i = 0; i < nBlocks; i++)
	{
		char value;
		value = this->bm.bitmap[i];
		for (int j = 1; j <= SHIFT; j++)
		{
			int bit = (value & MASK ? 1 : 0);
			if (bit == 0)
			{
				int freePos = (j - 1) + (i * 8);
				if (freePos > nBlocks)
				{
					cout << "No queda espacio ..." << endl;
					return 1;
				}
				return freePos;
			}
			else
				MASK = 1 << SHIFT - j;
		}
	}

	cout << "No queda espacio en disco ..." << endl;
	return -1;
}

void BMFcunctions::setOn(int nBlock) {
	int positionByte = nBlock / 8;
	int iniPosition = (nBlock / 8) * 8;

	for (int i = iniPosition, x = 7; i < (positionByte * 8) + 8; i++, x--)
	{
		if (i == nBlock)
		{
			this->bm.bitmap[positionByte] |= 1 << x;
			break;
		}

	}
}

void BMFcunctions::setOff(int nBlock) {
	int positionByte = nBlock / 8;
	int iniPosition = (nBlock / 8) * 8;

	for (int i = iniPosition, x = 7; i < (positionByte * 8) + 8; i++, x--)
	{
		if (i == nBlock)
		{
			this->bm.bitmap[positionByte] &= ~(1 << x);
			break;
		}
	}
}

void BMFcunctions::printBitMap(int nBlocks) {
	const int SHIFT = 8 * sizeof(char) - 1;
	const char MASK = 1 << SHIFT;

	for (int i = 0; i < nBlocks / 8; i++)
	{
		char value;
		value = this->bm.bitmap[i];
		for (int c = 0; c <= nBlocks / 8; c++)
		{
			cout << (value & MASK ? '1' : '0');
			value <<= 1;

			if (c % 8 == 0)
				cout << ' ';
		}

	}
	cout << endl;
}

void BMFcunctions::printNewBM(int nBlocks) {
	const int SHIFT = 8 * sizeof(char) - 1;
	const char MASK = 1 << SHIFT;
	int fullcounter = 0;

	for (int i = 0; i < nBlocks / 8; i++)
	{
		char value;
		value = this->bm.bitmap[i];

		for (int c = 1; c <= SHIFT + 1; c++)
		{
			cout << (value & MASK ? '1' : '0');
			value <<= 1;
			fullcounter++;
			if (c % 8 == 0)
				cout << ' ';

		}
	}
	cout << fullcounter << endl;
}

