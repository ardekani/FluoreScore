#include <vector>
#include <fstream>

using namespace std;

class CsvEntry{
public:
	CsvEntry();
	CsvEntry(unsigned char *ch, FILE *file, int numOfValues);
	CsvEntry(double *element, int elementSize, int numOfValues);
	void resetEntryElement();

	// IMMUTABLE METHODS
	double getEntryElement(int index) const;
	int getEntrySize() const;
	bool operator < (const CsvEntry) const;	
	void printCsvEntry(ofstream &out) const;
	void printPosEntry(ofstream &pos) const;
	void testPrintCsvEntry() const;

private:
	double* entryElement;
	int entrySize;

	double readElement(unsigned char *ch, FILE *file);
};