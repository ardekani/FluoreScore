/*
/**
FluoreScoreSQ (version 1.0.0)
A part of FluoreScoreSuite
for more details please visit http:\\fluorescore.cmb.usc.edu
Written by Yichuan Michelle Huang (huangyic@usc.edu)
Updated by Reza Ardekani (dehestan@usc.edu)
April 2012
*/

# include "Csv.h"
# include <iostream>
# include <fstream>
# include <string>
# include <conio.h>
using namespace std;

int main(int argc, char *argv[]){

	if (argc<2)
	{
		cout<<"Some input arguments are missing\nPlease enter input arguments as below:";
		//cout<<"cmd I: FluoreScoreSD.exe input1 input2 output1 output2 output3 flyNum flySize"<<endl;
		cout<<"\ncmd I: FluoreScoreSD.exe <view1input> <view2input> <combinedOutput> <flyNum> <flySize>";
		cout<<"\ncmd II: FluoreScoreSD.exe <view1input> <view2input> <combinedOutput> <outputpos2D_1> <outputpos2D_2> <flyNum> <flySize>";
		cout<<"\nPlease visit http://fluorescore.cmb.usc for more info\nPress Enter to exit...";
		_getch();
		return 0;


	}

	FILE *file, *file2;
	int flyNum, flySize;

	std::vector<char> chnls;
	int nBins;
	chnls.push_back('b');
	chnls.push_back('g');
	chnls.push_back('r');

	nBins = 4; 
	int numOfValues = 29; 



	if(!(argc==8 || argc==6)){
		cout<<"Some input arguments are missing\nPlease enter input arguments as below:";
		//cout<<"cmd I: FluoreScoreSD.exe input1 input2 output1 output2 output3 flyNum flySize"<<endl;
		cout<<"\nFluoreScoreSD.exe <cam1> <cam2> <output> <flyNum> <flySize>";
		cout<<"\nPlease visit http://fluorescore.cmb.usc for more info\nPress Enter to exit...";
		_getch();
		return 0;
	}

	if(argc==8){
		file = fopen(argv[1],"rb");  // argv[1] input1  
		file2 = fopen(argv[2],"rb");  // argv[2] input2
		flyNum = atoi(argv[6]);  // fly number
		flySize = atoi(argv[7]);  // minimum fly size
	}

	if(argc==6){
		file = fopen(argv[1],"rb");  // argv[1] input1
		file2 = fopen(argv[2],"rb");  // argv[2] input2
		flyNum = atoi(argv[4]);  // fly number
		flySize = atoi(argv[5]);  // minimum fly size
	}

	if(!file){
		cout<<"Input file 1 not exist!"<<endl;
		_getch();
		return 0;
	}
	if(!file2){
		cout<<"Input file 2 not exist!"<<endl;
		_getch();
		return 0;
	}

	if(argc==8&&flyNum>1){
		cout<<"ERROR: Can't output position file since fly number is more than one!"<<endl;
		_getch();
		return 0;
	}


	printf("\nstart squeezing data ...");
	Csv *csv = new Csv(file,numOfValues);
	Csv *csv2 = new Csv(file2,numOfValues);
	fclose(file);
	fclose(file2);

	csv->squeezeData(flyNum, flySize);
	csv2->squeezeData(flyNum, flySize);
	if(argc==8){
		ofstream out2dPos1(argv[4]); // output_pos1 
		ofstream out2dPos2(argv[5]); // output_pos2
		csv->printPos(out2dPos1);
		csv2->printPos(out2dPos2);
	}
	Csv *newCsv = csv->combineCsv(csv2, numOfValues);
	ofstream out(argv[3]); // output

	newCsv->printCsv(out, chnls, nBins);
	out.close();
	delete csv;
	delete csv2;
	delete newCsv;
	cout<<"processed finished! ..."<<endl;
}