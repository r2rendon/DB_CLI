#include "DBStructs.h"

class BMFcunctions
{
public:
	BitMap bm;
	BMFcunctions();
	BMFcunctions(int bmSize);
	BMFcunctions(BitMap bm);
	void initBitMap(int DBAmount);
	int getFreePosBit(int blockSize);
	void setOn(int nBlock);
	void setOff(int nBlock);
	void printBitMap(int nBlocks);
	void printNewBM(int nBlocks);

};