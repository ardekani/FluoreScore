#include "CsvEntry.h"
#include <vector>

using namespace std;

class Csv{
public:
	Csv();
	Csv(FILE *file, int numOfValues); //int numOfValues added by Reza
	void squeezeData(int flyNum, int flySize);
	void printCsv(ofstream &out, vector<char>channels, int nBins); // channels and nBins is added by Reza
	//void printCsv(ofstream &out);

	void printPos(ofstream &pos);
	Csv* combineCsv(int numOfValues);
	Csv* combineCsv(Csv *b, int numOfValues);

private:
	vector<CsvEntry> CsvFile;

	CsvEntry getEntry(int index) const;
	int getTotalFrameNumber() const;
	int getCsvSize() const;
	void addCsvEntry(CsvEntry* b);
};