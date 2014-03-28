#include "Csv.h"
#include <iostream>
#include <algorithm> 
#include <functional> 


Csv::Csv(){
}

Csv::Csv(FILE *file, int numOfValues){
	unsigned char *ch = new unsigned char[1];
	*ch = fgetc(file);

	while( *ch != 0x0A ){  // skip the very first line
		*ch = fgetc(file);
	}
	*ch = fgetc(file);

	while(*ch!=feof(file) && *ch!=0xff){
		CsvEntry *temp = new CsvEntry(ch, file, numOfValues);
//temp->testPrintCsvEntry();
		CsvFile.push_back(*temp);
	}
	
}

void Csv::squeezeData(int flyNum, int flySize){
	double frameNum = 0;
	double realFlyNum = 0;
	double realFlySize = 0;

	sort(CsvFile.begin(),CsvFile.end());

	for(vector<CsvEntry>::iterator i=CsvFile.begin();i!=CsvFile.end();i++){
		if(i->getEntryElement(0)!=frameNum){
			frameNum = i->getEntryElement(0);
			if(i->getEntryElement(1)!=0){
				realFlyNum = 1;
			}
			else{
				realFlyNum = 0;
			}			
		}
		else{
			realFlyNum += 1;
		}
		realFlySize = i->getEntryElement(i->getEntrySize()-3);

		if(realFlyNum>flyNum){
			i = CsvFile.erase(i);
			i--;
		}
		else if(realFlySize<flySize){
			if(realFlyNum>1){
				i = CsvFile.erase(i);
				i--;
			}
			else if(realFlyNum==1){
				i->resetEntryElement();
			}
		}
	}
}

/**
	CSV combine method for single file
	@return: a new CSV file that combines element in original CSV file
*/
Csv* Csv::combineCsv(int numOfValues){ //numOfValues added by Reza
	Csv *comCsv = new Csv();
	// array that saves combined element
	int elementSize = this->getEntry(0).getEntrySize();
	double *element = new double[elementSize];
	for(int i=0;i<elementSize;i++){
		element[i] = 0;
	}
	// search through Csv file and combine data in same frame
	int entryNumberA = 0;  // entry # of file
	int frameNumberA = 0;  // frame # of current entry
	int temp = 0;
	while(entryNumberA<(this->getCsvSize())){
		CsvEntry tempEntry = this->getEntry(entryNumberA);
		frameNumberA = tempEntry.getEntryElement(0);
		temp = frameNumberA;

		while(frameNumberA==temp){
			element[0] = temp;
			if(tempEntry.getEntryElement(1)!=0){
				element[1] += 1;
			}
			for(int i=2;i<elementSize;i++){
				element[i] += tempEntry.getEntryElement(i);
			}
			entryNumberA++;
			// prevent out of Csv Entry boundary
			if(entryNumberA<(this->getCsvSize())){
				tempEntry = this->getEntry(entryNumberA);
				frameNumberA = tempEntry.getEntryElement(0);
			}
			else{
				frameNumberA += 1;
			}
		}
		// add new CSV entry into new CSV files
		CsvEntry *entryA = new CsvEntry(element,elementSize, numOfValues);
		comCsv->addCsvEntry(entryA);
		delete entryA;
		// reset array that saves combined element
		for(int i=0;i<elementSize;i++){
			element[i] = 0;
		}
	}
	delete element;
	return comCsv;
}

/**
	CSV combine method for two files
	@return: a new CSV file that combines element in original CSV files
*/
Csv* Csv::combineCsv(Csv *b, int numOfValues){
	Csv *comCsv = new Csv();
	// check if two files are synchronized
	int totalFrameNumberA = this->getTotalFrameNumber();
	int totalFrameNumberB = b->getTotalFrameNumber();
	if(totalFrameNumberA!=totalFrameNumberB){
		cout<<"WARNING: Data from CameraA and CameraB are not synchronized!\n Combine two files with warning."<<endl;
	}	
	// check if element size are same in 2 CSV files
	int elementSize = this->getEntry(0).getEntrySize();
	if(elementSize!=b->getEntry(0).getEntrySize()){
		cout<<"ERROR: different entry size of CameraA and CameraB detected!!\nExit without doing combination."<<endl;
		return comCsv;
	}
	// array that saves combined element
	double *element = new double[elementSize];
	for(int i=0;i<elementSize;i++){
		element[i] = 0;
	}
	// search through Csv file and combine data in same frame
	int entryNumberA = 0;
	int entryNumberB = 0;
	int frameNumberA = 0;
	int frameNumberB = 0;
	int temp = 0;
	while(entryNumberA<(this->getCsvSize())&&entryNumberB<(b->getCsvSize())){
		CsvEntry tempEntry = this->getEntry(entryNumberA);
		frameNumberA = tempEntry.getEntryElement(0);
		temp = frameNumberA;

		// combined data from CameraA
		while(frameNumberA==temp){
			element[0] = temp;
			if(tempEntry.getEntryElement(1)!=0){
				element[1] += 1;
			}
			for(int i=2;i<elementSize;i++){
				element[i] += tempEntry.getEntryElement(i);
			}
			entryNumberA++;
			// prevent out of Csv Entry boundary
			if(entryNumberA<(this->getCsvSize())){
				tempEntry = this->getEntry(entryNumberA);
				frameNumberA = tempEntry.getEntryElement(0);
			}
			else{
				frameNumberA += 1;
			}
		}
		// combined data from CameraB
		tempEntry = b->getEntry(entryNumberB);
		frameNumberB = tempEntry.getEntryElement(0);
		temp = frameNumberB;
		while(frameNumberB==temp){
			if(tempEntry.getEntryElement(1)!=0){
				element[1] += 1;
			}
			for(int i=2;i<elementSize;i++){
				element[i] += tempEntry.getEntryElement(i);
			}
			entryNumberB++;
			// prevent out of Csv Entry boundary
			if(entryNumberB<(b->getCsvSize())){
				tempEntry = b->getEntry(entryNumberB);
				frameNumberB = tempEntry.getEntryElement(0);
			}
			else{
				frameNumberB += 1;
			}
		}
		// add new CSV entry into new CSV files
		CsvEntry *entry = new CsvEntry(element,elementSize, numOfValues);
		comCsv->addCsvEntry(entry);
		delete entry;
		// reset array that saves combined element
		for(int i=0;i<elementSize;i++){
			element[i] = 0;
		}		
	}
	delete element;
	return comCsv;
}

//void Csv::printCsv(ofstream &out){
//	out << "frameNum,Nc,b1Num,b1Sum,b2Num,b2Sum,b3Num,b3Sum,b4Num,b4Sum,g1Num,g1Sum,g2Num,g2Sum,g3Num,g3Sum,g4Num,g4Sum,r1Num,r1Sum,r2Num,r2Sum,r3Num,r3Sum,r4Num,r4Sum,totalNumOfPixels"<<endl;
//	for(vector<CsvEntry>::iterator i=CsvFile.begin();i<CsvFile.end();i++){
//		i->printCsvEntry(out);
//	}
//}


void Csv::printCsv(ofstream &out, vector<char>channels, int nBins){ // channels and nBins is added by Reza
	out << "frameNum,Nc";
	for (int ii = 0;ii<channels.size();ii++)
		for (int jj = 1;jj<=nBins;jj++)
			out<<","<<channels[ii]<<jj<<"Num,"<<channels[ii]<<jj<<"Sum";
	out<<",totalNumOfPixels"<<endl;

	for(vector<CsvEntry>::iterator i=CsvFile.begin();i<CsvFile.end();i++){
		i->printCsvEntry(out);
	}
}




void Csv::printPos(ofstream &pos){
	pos << "frameNum,x,y"<<endl;
	for(vector<CsvEntry>::iterator i=CsvFile.begin();i<CsvFile.end();i++){
		i->printPosEntry(pos);
	}
}


//////////////////////////////////////////////////////////////////
// PRIVATE METHOD
CsvEntry Csv::getEntry(int index) const{
	return CsvFile.at(index);
}

int Csv::getTotalFrameNumber() const{
	int fileSize = CsvFile.size();
	int frameNumBegining = CsvFile.at(0).getEntryElement(0);
	int frameNumEnd = CsvFile.at(fileSize-1).getEntryElement(0);

	return (frameNumEnd-frameNumBegining+1);	
}

int Csv::getCsvSize() const{
	return CsvFile.size();
}

void Csv::addCsvEntry(CsvEntry* b){
	CsvFile.push_back(*b);
}