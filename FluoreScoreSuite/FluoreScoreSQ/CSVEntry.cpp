# include <iostream>
#include "CsvEntry.h"
#include <math.h>


CsvEntry::CsvEntry(){
	entrySize = 0;
}

CsvEntry::CsvEntry(unsigned char *ch, FILE *file,int numOfValues){// int numOfValues added by Reza to make this adabtable for different number of bins/colors
//	entryElement = new double[29];
	entryElement = new double[numOfValues];

	int index = 0;
	while(!feof(file) && *ch!=0xff && *ch!=0x0d){
		entryElement[index] = readElement(ch, file);
		index ++;
	}
	if(*ch == 0x0d){
		*ch = fgetc(file);  // skip '\n'
		*ch = fgetc(file);  // get element next line
	}
	entrySize = index;
}

CsvEntry::CsvEntry(double *element, int elementSize,int numOfValues){ // int numOfValues added by Reza to make this adabtable for different number of bins/colors
	//entryElement = new double[29]; commented by Reza
	entryElement = new double[numOfValues]; // Added by Reza 
	entryElement[0] = element[0];
	entryElement[1] = element[1];
	for(int i=2;i<elementSize;i++){
		if(element[1]==0){
			entryElement[i] = 0;;
		}
		else{
			entryElement[i] = element[i]/element[1];
		}
	}
	entrySize = elementSize;
}

void CsvEntry::resetEntryElement(){
	for(int i=1;i<entrySize;i++){
		entryElement[i] = 0;
	}
}

void CsvEntry::printCsvEntry(ofstream &out) const{
	unsigned int i;
	for(i = 0;i<(entrySize-1-2);i++){ // skip 2 position + total number
		out << entryElement[i] << ',';
	}
	out << entryElement[i] <<endl;  // output total number
}

////////////////////////////
///////TEST TEST TEST///////
void CsvEntry::testPrintCsvEntry() const{
	unsigned int i;
	for(i = 0;i<(entrySize-1-2);i++){ // skip 2 position + total number
		cout << entryElement[i] << ',';
	}
	cout << entryElement[i] <<endl;  // output total number
}
///////TEST TEST TEST///////
////////////////////////////


void CsvEntry::printPosEntry(ofstream &pos) const{
	unsigned int i;
	if (entryElement[1]!=0){
		pos << entryElement[0] << ","<<entryElement[entrySize-2] << "," << entryElement[entrySize-1]<<endl;
	}
}


double CsvEntry::getEntryElement(int index) const{
	return entryElement[index];
}

int CsvEntry::getEntrySize() const{
	return entrySize;
}

bool CsvEntry::operator < (const CsvEntry b) const{
	if(entrySize!=b.getEntrySize()){
		cout<<"WARNING: different entry size detected!"<<endl;
	}
	if(entryElement[0]==b.getEntryElement(0)){
		return entryElement[entrySize-3]>b.getEntryElement(b.getEntrySize()-3);
	}
	else{
		return entryElement[0]<b.getEntryElement(0);
	}
}

//////////////////////////////////////////////////////////////////
// PRIVATE METHOD
double CsvEntry::readElement(unsigned char *ch, FILE *file){
	int temp = 0;
	int expon = 0;
	int digit = 0;
	int digitCount = 0;
	while(*ch!=feof(file) && *ch!=0xff && *ch != 0x2c && *ch != 'E'  && *ch != 'e' && *ch != 0x2e && *ch != 0x0d) { // not ',' not 'E/e' not '.' ---not '/n' case for integer ypoint
		*ch -= 0x30;  // convert a char num into a real num
		temp = temp*10+*ch;
		*ch = fgetc(file);
	}
	if(*ch == 'E'|| *ch == 'e'){
		*ch = fgetc(file);
		while(*ch != 0x2c){  // not ','
			*ch -= 0x30;
			expon = expon*10+*ch;
			*ch = fgetc(file);
		}
	}
	if(*ch == 0x2e){
		*ch = fgetc(file);
		while(*ch != 0x2c && *ch != 0x0d && *ch!=feof(file) && *ch!=0xff){  // not ',' not '/n' not 'EOF' not 'FF'
			*ch -= 0x30;
			digit = digit*10+*ch;
			digitCount +=1;
			*ch = fgetc(file);
		}
	}

	if(*ch == 0x2c){  // if ','
		*ch = fgetc(file);
	}

	double element = ((temp+digit*pow((double)10,(double)(-digitCount)))*pow((double)10,(double)expon));
	return element;
}