#include <iostream> // cout, endl
#include <fstream> // open, is_open
#include <string> // string
#include <sstream>
#include <cstring>
#include <vector> // vector
#include <cstdlib> // atoi, system
#include <iomanip> // setw
#include <ctime> // clock, CLOCKS_PER_SEC
#define PAGE_SIZE 30 // number of output per page


using namespace std; // standard naming space

typedef struct pageInfo {
	int name;
	int timeStamp;
	int refBit;   // 0 or 1 
	int counter; // 起始值為0
	int shiftReg[8];
	int shiftRegValue;
} pageInfo;

typedef struct reSult {  // 存上半部結果 步驟輸出 
	int ref;
	vector<pageInfo> currentFrame;
	char pageFault;  // 空或'F' 
} reSult;

typedef struct reSult2 { // 存下半部結果 統計資料 
	int pageFaults;
	int pageReplace;
	int frames;
} reSult2;


//--------------------------------------------------------------------

int pageFrame;
vector<int> PageRefSeq;
vector<pageInfo> PageFrameInMem;

vector<reSult> FIFOr;    // 先進先出 
reSult2 FIFOr2;

vector<reSult> LRUr;   // Least Recently Used 
reSult2 LRUr2;

vector<reSult> ARBr;   // Additional Reference Bits
reSult2 ARBr2;

vector<reSult> SCr;  // Second chance
reSult2 SCr2;

vector<reSult> LFUr;  // Least Frequently Used
reSult2 LFUr2;

vector<reSult> MFUr; // Most Frequently Used
reSult2 MFUr2;

//-------------------------------------------------------------------

int ctoi(char in) {
	int ia = in - '0';
	return ia;
} // ctoi

void ReadAll() {
	fstream inFile;
	string fileName;
	cout << " Input a file name : ";
	cin >> fileName;
	fileName = fileName + ".txt";


	inFile.open(fileName.c_str(), fstream::in);   // open file to read
	if (!inFile.is_open()) {                        // unable to open file
		cout << endl << "### " << fileName << " does not exist! ###" << endl;
		exit(0);
	} // if
	else {
		char c;
		inFile.get(c);
		pageFrame = ctoi(c); // get row 1 : pageFrame
		inFile.get(c);  // line enter 
		while (inFile.get(c)) {   // get each input record
			if (c != '\n' && c != '\t' && c != ' ') {
				PageRefSeq.push_back(ctoi(c));
			} // if 
		} // while 
	} // else

	inFile.close();
} // ReadAll

  //---------------------------------------------------

void CreatePageFrame(int ii) { // 初始化(7)跟reset(other) 記憶體的PageFrame 
	if (ii == 7) { // 初始化
		for (int a = 0; a < pageFrame; a++) {
			pageInfo oneP;
			oneP.counter = 0;
			oneP.name = -1;
			oneP.refBit = 0;
			oneP.timeStamp = 0;
			oneP.shiftRegValue = 0;
			for (int c = 0; c < 8; c++) {
				oneP.shiftReg[c] = 0;
			} // for 
			PageFrameInMem.push_back(oneP);
		} // for
	} // if
	else { // reset
		for (int b = 0; b < pageFrame; b++) {
			PageFrameInMem.at(b).counter = 0;
			PageFrameInMem.at(b).name = -1;
			PageFrameInMem.at(b).refBit = 0;
			PageFrameInMem.at(b).timeStamp = 0;
			PageFrameInMem.at(b).shiftRegValue = 0;
			for (int c = 0; c < 8; c++) {
				PageFrameInMem.at(b).shiftReg[c] = 0;
			} // for 
		} // for
	} // else 

} // CreatePageFrame()

  //-------------------------------------------------

bool writeRecords()
{

	cout << "type in output file name : ";
	string fname;
	cin >> fname;
	string FileName = fname + ".txt"; // output file name
	fstream fp;
	fp.open(FileName.c_str(), fstream::out); // create a new file to write
	if (!fp) {//如果開啟檔案失敗，fp為0；成功，fp為非0
		cout << "Fail to open file: " << fname << endl;
		return false;
	} // if
	else {

		fp << "-----------------------FIFO-----------------------" << endl;
		for (int a = 0; a < FIFOr.size(); a++) {
			fp << std::left << setw(10) << FIFOr.at(a).ref ;
			for (int b = 0; b < FIFOr.at(a).currentFrame.size(); b++) {
				if(FIFOr.at(a).currentFrame.at(b).name != -1 ) {
					fp << FIFOr.at(a).currentFrame.at(b).name;
				} // if
			} // for

			fp << std::left << setw(10) << "	    " << FIFOr.at(a).pageFault << endl;
		} // for

		fp << "Page Fault = " << FIFOr2.pageFaults << "  " << "Page Replaces = " << FIFOr2.pageReplace <<
			"  " << "Page Frames = " << FIFOr2.frames << endl << endl;


		fp << "-----------------------LRU-----------------------" << endl;
		for (int a = 0; a < LRUr.size(); a++) {
			fp << std::left << setw(10) << LRUr.at(a).ref ;
			for (int b = 0; b < LRUr.at(a).currentFrame.size(); b++) {
				if(LRUr.at(a).currentFrame.at(b).name != -1 ) {
					fp << LRUr.at(a).currentFrame.at(b).name;
				} // if
			} // for

			fp << std::left << setw(10) << "	    " << LRUr.at(a).pageFault << endl;
		} // for

		fp << "Page Fault = " << LRUr2.pageFaults << "  " << "Page Replaces = " << LRUr2.pageReplace <<
			"  " << "Page Frames = " << LRUr2.frames << endl << endl;


		fp << "-----------------------Additional Reference Bits-----------------------" << endl;
		for (int a = 0; a < ARBr.size(); a++) {
			fp << std::left << setw(10) << ARBr.at(a).ref ;
			for (int b = 0; b < ARBr.at(a).currentFrame.size(); b++) {
				if(ARBr.at(a).currentFrame.at(b).name != -1 ) {
					fp << ARBr.at(a).currentFrame.at(b).name;
				} // if
			} // for

			fp << std::left << setw(10) << "	    " << ARBr.at(a).pageFault << endl;
		} // for

		fp << "Page Fault = " << ARBr2.pageFaults << "  " << "Page Replaces = " << ARBr2.pageReplace <<
			"  " << "Page Frames = " << ARBr2.frames << endl << endl;


		fp << "-----------------------Second chance-----------------------" << endl;
		for (int a = 0; a < SCr.size(); a++) {
			fp << std::left << setw(10) << SCr.at(a).ref ;
			for (int b = 0; b < SCr.at(a).currentFrame.size(); b++) {
				if(SCr.at(a).currentFrame.at(b).name != -1 ) {
					fp << SCr.at(a).currentFrame.at(b).name;
				} // if
			} // for

			fp << std::left << setw(10) << "	    " << SCr.at(a).pageFault << endl;
		} // for

		fp << "Page Fault = " << SCr2.pageFaults << "  " << "Page Replaces = " << SCr2.pageReplace <<
			"  " << "Page Frames = " << SCr2.frames << endl << endl;


		fp << "-----------------------Least Frequently Used-----------------------" << endl;
		for (int a = 0; a < LFUr.size(); a++) {
			fp << std::left << setw(10) << LFUr.at(a).ref ;
			for (int b = 0; b < LFUr.at(a).currentFrame.size(); b++) {
				if(LFUr.at(a).currentFrame.at(b).name != -1 ) {
					fp << LFUr.at(a).currentFrame.at(b).name;
				} // if
			} // for

			fp << std::left << setw(10) << "	    " << LFUr.at(a).pageFault << endl;
		} // for

		fp << "Page Fault = " << LFUr2.pageFaults << "  " << "Page Replaces = " << LFUr2.pageReplace <<
			"  " << "Page Frames = " << LFUr2.frames << endl << endl;


		fp << "-----------------------Most Frequently Used-----------------------" << endl;
		for (int a = 0; a < MFUr.size(); a++) {
			fp << std::left << setw(10) << MFUr.at(a).ref;
			for (int b = 0; b < MFUr.at(a).currentFrame.size(); b++) {
				if(MFUr.at(a).currentFrame.at(b).name != -1 ) {
					fp << MFUr.at(a).currentFrame.at(b).name;
				} // if
			} // for

			fp << std::left << setw(10) << "	    " << MFUr.at(a).pageFault << endl;
		} // for

		fp << "Page Fault = " << MFUr2.pageFaults << "  " << "Page Replaces = " << MFUr2.pageReplace <<
			"  " << "Page Frames = " << MFUr2.frames << endl << endl;

	}   //end else    

	fp.close();//關閉檔案

	return true;		// it succeeds if there is at least one records

}   // end writeRecords




	//----------------------------------------------------------------------------

int isInFrame(int No) { // 去Frame裡面找輸入的頁編號
	for (int i = 0; i < PageFrameInMem.size(); i++) {
		if (PageFrameInMem.at(i).name == No) {
			return i;
		} // if
	} // for 

	return -1;
} // isInFrame

int hasSpace() { // 檢查記憶體中的pageFrame還有沒有空間 
	int c = -1;
	for (int v = 0; v < PageFrameInMem.size(); v++) {
		if (PageFrameInMem.at(v).name == -1) {
			c = v; 
			break;
		} // if
	} // for

	return c;
} // hasSpace

  //--------------------------------------------------------------------------------
void BubbleByTimeStamp() {
	for (int i = 0; i < PageFrameInMem.size() - 1; i++ ) {   // BubbleByTimeStamp

		for (int j = 0; j < PageFrameInMem.size() - i-1; j++) {
			if (PageFrameInMem.at(j).timeStamp > PageFrameInMem.at(j + 1).timeStamp) {
				swap(PageFrameInMem.at(j), PageFrameInMem.at(j + 1));
			} // if

		} // for
		

	} // for
} // BubbleByTimeStamp

void FIFO() {
	FIFOr.clear();
	FIFOr2.frames = pageFrame;
	FIFOr2.pageFaults = 0;
	FIFOr2.pageReplace = 0;

	int t = 0; // time
	CreatePageFrame(10);
	while (t < PageRefSeq.size()) {
		reSult oneR;
		oneR.ref = PageRefSeq.at(t);
		int InFrame = isInFrame(oneR.ref);

		if (InFrame != -1) { // 如果已經在頁框中 
			oneR.pageFault = '\0';
			oneR.currentFrame.assign(PageFrameInMem.begin(), PageFrameInMem.end());
		} // if
		else {   // 如果沒在頁框中 
			oneR.pageFault = 'F';
			FIFOr2.pageFaults++;
			int gg = hasSpace();
			if (gg != -1) { // if 有空間 
				PageFrameInMem.at(gg).name = PageRefSeq.at(t);
				PageFrameInMem.at(gg).timeStamp = t;
			} // if
			else { // 沒空間了 
				FIFOr2.pageReplace++;
				PageFrameInMem.at(0).name = PageRefSeq.at(t);
				PageFrameInMem.at(0).timeStamp = t;
			} // else 

			oneR.currentFrame.assign(PageFrameInMem.begin(), PageFrameInMem.end());
		} // else 

		FIFOr.push_back(oneR);
		BubbleByTimeStamp(); // 整理記憶體中的頁框 

		t++;
	} // while
} // FIFO

  //----------------------------------------------------------------------------

void LRU() {
	LRUr.clear();
	LRUr2.frames = pageFrame;
	LRUr2.pageFaults = 0;
	LRUr2.pageReplace = 0;

	int t = 0; // time
	CreatePageFrame(10);
	while (t < PageRefSeq.size()) {
		reSult oneR;
		oneR.ref = PageRefSeq.at(t);
		int InFrame = isInFrame(oneR.ref);

		if (InFrame != -1) { // 如果已經在頁框中 
			oneR.pageFault = '\0';
			PageFrameInMem.at(InFrame).timeStamp = t;
			oneR.currentFrame.assign(PageFrameInMem.begin(), PageFrameInMem.end());
		} // if
		else {   // 如果沒在頁框中 
			oneR.pageFault = 'F';
			LRUr2.pageFaults++;
			int gg = hasSpace();
			if (gg != -1) { // if 有空間 
				PageFrameInMem.at(gg).name = PageRefSeq.at(t);
				PageFrameInMem.at(gg).timeStamp = t;
			} // if
			else { // 沒空間了 
				LRUr2.pageReplace++;
				PageFrameInMem.at(0).name = PageRefSeq.at(t);
				PageFrameInMem.at(0).timeStamp = t;
			} // else 

			oneR.currentFrame.assign(PageFrameInMem.begin(), PageFrameInMem.end());
		} // else 

		LRUr.push_back(oneR);
		BubbleByTimeStamp(); // 整理記憶體中的頁框 

		t++;
	} // while
} // LRU

  //----------------------------------------------------------------------------

void Shhhift() {
	for (int c = 0; c < PageFrameInMem.size(); c++) {
		int outtBit = PageFrameInMem.at(c).refBit;
		PageFrameInMem.at(c).shiftReg[7] = PageFrameInMem.at(c).shiftReg[6];
		PageFrameInMem.at(c).shiftReg[6] = PageFrameInMem.at(c).shiftReg[5];
		PageFrameInMem.at(c).shiftReg[5] = PageFrameInMem.at(c).shiftReg[4];
		PageFrameInMem.at(c).shiftReg[4] = PageFrameInMem.at(c).shiftReg[3];
		PageFrameInMem.at(c).shiftReg[3] = PageFrameInMem.at(c).shiftReg[2];
		PageFrameInMem.at(c).shiftReg[2] = PageFrameInMem.at(c).shiftReg[1];
		PageFrameInMem.at(c).shiftReg[1] = PageFrameInMem.at(c).shiftReg[0];
		PageFrameInMem.at(c).shiftReg[0] = outtBit;
	} // for
} // Shhhift

void CalcuteShiftValue() {
	for (int c = 0; c < PageFrameInMem.size(); c++) {
		PageFrameInMem.at(c).shiftRegValue =
			(PageFrameInMem.at(c).shiftReg[7] * 1) + (PageFrameInMem.at(c).shiftReg[6] * 2) +
			(PageFrameInMem.at(c).shiftReg[5] * 4) + (PageFrameInMem.at(c).shiftReg[4] * 8) +
			(PageFrameInMem.at(c).shiftReg[3] * 16) + (PageFrameInMem.at(c).shiftReg[2] * 32) +
			(PageFrameInMem.at(c).shiftReg[1] * 64) + (PageFrameInMem.at(c).shiftReg[0] * 128);
			
//			cout << PageFrameInMem.at(c).name << ": " << PageFrameInMem.at(c).shiftRegValue << endl ; // test
	} // for
	
//	cout << "---------------------" << endl ;
} // CalcuteShiftValue()

void PageShift() { // 往左推 
	for (int c = 0; c < PageFrameInMem.size() -1 ; c++) {
		PageFrameInMem.at(c).counter = PageFrameInMem.at(c+1).counter ;
		PageFrameInMem.at(c).name = PageFrameInMem.at(c+1).name ;
		PageFrameInMem.at(c).refBit = PageFrameInMem.at(c+1).refBit ;
		PageFrameInMem.at(c).shiftRegValue = PageFrameInMem.at(c+1).shiftRegValue ;
		PageFrameInMem.at(c).timeStamp = PageFrameInMem.at(c+1).timeStamp ;
		for(int o = 0 ; o <8 ; o++ ) {
			PageFrameInMem.at(c).shiftReg[o] = PageFrameInMem.at(c+1).shiftReg[o] ;
		} // for
	} // for
} // PageShift()

int Find_Smallest_shift_Reg_Value() {
	vector<pageInfo> ttemp ; 
	ttemp.assign(PageFrameInMem.begin(), PageFrameInMem.end()) ; 
	for (int i = 0; i < ttemp.size() - 1; i++ ) {   // BubbleByTimeStamp
		for (int j = 0; j < ttemp.size() - i-1; j++) {
			if (ttemp.at(j).shiftRegValue > ttemp.at(j + 1).shiftRegValue) {
				swap(ttemp.at(j), ttemp.at(j + 1));
			} // if
		} // for
	} // for
	
	return isInFrame(ttemp.at(0).name) ;
}//FindSmallestshiftRegValue

void Put_samllest_to_the_leftMost() {
	int index = Find_Smallest_shift_Reg_Value() ; // 最小的 
	pageInfo temmp ; 
	temmp.name = PageFrameInMem.at(index).name ;
	temmp.refBit = PageFrameInMem.at(index).refBit ;
	temmp.timeStamp = PageFrameInMem.at(index).timeStamp ;
	temmp.shiftRegValue = PageFrameInMem.at(index).shiftRegValue ;
	for (int c = 0; c < 8; c++) {
		temmp.shiftReg[c] = PageFrameInMem.at(index).shiftReg[c];
	} // for 
	
	for( int x = index ; x-1 >= 0 ; x-- ) {
		PageFrameInMem.at(x).name = PageFrameInMem.at(x-1).name;
		PageFrameInMem.at(x).refBit = PageFrameInMem.at(x-1).refBit;
		PageFrameInMem.at(x).timeStamp = PageFrameInMem.at(x-1).timeStamp;
		PageFrameInMem.at(x).shiftRegValue = PageFrameInMem.at(x-1).shiftRegValue;
		for (int c = 0; c < 8; c++) {
			PageFrameInMem.at(x).shiftReg[c] = PageFrameInMem.at(x-1).shiftReg[c];
		} // for 
	} // for
	
	PageFrameInMem.at(0).name = temmp.name;
	PageFrameInMem.at(0).refBit = temmp.refBit;
	PageFrameInMem.at(0).timeStamp = temmp.timeStamp;
	PageFrameInMem.at(0).shiftRegValue = temmp.shiftRegValue;
	for (int c = 0; c < 8; c++) {
		PageFrameInMem.at(0).shiftReg[c] = temmp.shiftReg[c];
	} // for 
	
} // Put_samllest_to_the_leftMost

void AdditionalReferenceBits() {
	ARBr.clear();
	ARBr2.frames = pageFrame;
	ARBr2.pageFaults = 0;
	ARBr2.pageReplace = 0;

	int t = 0; // time
	CreatePageFrame(10);
	while (t < PageRefSeq.size()) {
		reSult oneR;
		oneR.ref = PageRefSeq.at(t);
		int InFrame = isInFrame(oneR.ref);

		if (InFrame != -1) { // 如果已經在頁框中 
			oneR.pageFault = '\0';
			PageFrameInMem.at(InFrame).refBit = 1;
			oneR.currentFrame.assign(PageFrameInMem.begin(), PageFrameInMem.end());
		} // if
		else {   // 如果沒在頁框中 
			oneR.pageFault = 'F';
			ARBr2.pageFaults++;
			int gg = hasSpace();
			if (gg != -1) { // if 有空間 
				PageFrameInMem.at(gg).name = PageRefSeq.at(t);
				PageFrameInMem.at(gg).refBit = 1;
				PageFrameInMem.at(gg).shiftRegValue = 0 ;
				for (int c = 0; c < 8; c++) {
					PageFrameInMem.at(gg).shiftReg[c] = 0;
				} // for 
			} // if
			else { // 沒空間了 
				ARBr2.pageReplace++;
				int lefmost = 0 ;
				bool hasKick = false ; // 有踢掉一頁了嗎? 
				while ( !hasKick ) {
						Put_samllest_to_the_leftMost() ;
						hasKick = true ;
						PageShift() ; 
						PageFrameInMem.at(PageFrameInMem.size()-1).name = PageRefSeq.at(t) ;
						PageFrameInMem.at(PageFrameInMem.size()-1).refBit = 1 ;
						PageFrameInMem.at(PageFrameInMem.size()-1).shiftRegValue = 0 ;
						PageFrameInMem.at(PageFrameInMem.size()-1).timeStamp = t ;
					
				} // while
			} // else 

			oneR.currentFrame.assign(PageFrameInMem.begin(), PageFrameInMem.end());
		} // else 

		Shhhift();
		CalcuteShiftValue();
		ARBr.push_back(oneR);
		for( int z = 0 ; z < PageFrameInMem.size() ; z++ ) {
			PageFrameInMem.at(z).refBit = 0 ; 
		} // for
		
		t++;
	} // while
} // AdditionalReferenceBits

  //----------------------------------------------------------------------------

void SecondChance() {
	SCr.clear();
	SCr2.frames = pageFrame;
	SCr2.pageFaults = 0;
	SCr2.pageReplace = 0;

	int t = 0; // time
	CreatePageFrame(10);
	while (t < PageRefSeq.size()) {
		reSult oneR;
		oneR.ref = PageRefSeq.at(t);
		int InFrame = isInFrame(oneR.ref);
		
		if (InFrame != -1) { // 如果已經在頁框中 
			oneR.pageFault = '\0';
			PageFrameInMem.at(InFrame).refBit = 1;
			oneR.currentFrame.assign(PageFrameInMem.begin(), PageFrameInMem.end());
		} // if
		else {   // 如果沒在頁框中 
			oneR.pageFault = 'F';
			SCr2.pageFaults++;
			int gg = hasSpace();
			if (gg != -1) { // if 有空間 
				PageFrameInMem.at(gg).name = PageRefSeq.at(t);
				PageFrameInMem.at(gg).refBit = 1;
				PageFrameInMem.at(gg).timeStamp = t;
			} // if
			else { // 沒空間了 
				SCr2.pageReplace++;
				int lefmost = 0 ;
				bool hasKick = false ; // 有踢掉一頁了嗎? 
				while ( !hasKick ) {
					
					if( PageFrameInMem.at(lefmost).refBit == 0 ) {
						hasKick = true ;
						PageShift() ; 
						PageFrameInMem.at(PageFrameInMem.size()-1).name = PageRefSeq.at(t) ;
						PageFrameInMem.at(PageFrameInMem.size()-1).refBit = 1 ;
						PageFrameInMem.at(PageFrameInMem.size()-1).timeStamp = t ;
						
					} // if
					else {
						pageInfo temp ; 
						
						PageFrameInMem.at(lefmost).refBit = 0 ; 
						PageFrameInMem.at(lefmost).timeStamp = t ;
						
						temp.name = PageFrameInMem.at(lefmost).name ;
						temp.refBit = PageFrameInMem.at(lefmost).refBit ;
						temp.timeStamp = PageFrameInMem.at(lefmost).timeStamp ;
						PageShift() ; 
						
						PageFrameInMem.at(PageFrameInMem.size()-1).name = temp.name ;
						PageFrameInMem.at(PageFrameInMem.size()-1).refBit = temp.refBit ;
						PageFrameInMem.at(PageFrameInMem.size()-1).timeStamp = temp.timeStamp ;
					} // else
				} // while

			} // else 

			oneR.currentFrame.assign(PageFrameInMem.begin(), PageFrameInMem.end());
		} // else 
		
		SCr.push_back(oneR);
		
		t++;
	} // while
} // SecondChance

  //----------------------------------------------------------------------------

int Find_Smallest_counter_Value() {
	vector<pageInfo> ttemp ; 
	ttemp.assign(PageFrameInMem.begin(), PageFrameInMem.end()) ; 
	for (int i = 0; i < ttemp.size() - 1; i++ ) {   // BubbleByTimeStamp
		for (int j = 0; j < ttemp.size() - i-1; j++) {
			if (ttemp.at(j).counter > ttemp.at(j + 1).counter) {
				swap(ttemp.at(j), ttemp.at(j + 1));
			} // if
			else if(ttemp.at(j).counter == ttemp.at(j + 1).counter &&
			 ttemp.at(j).timeStamp > ttemp.at(j + 1).timeStamp ){
				swap(ttemp.at(j), ttemp.at(j + 1));
			} // else if
		} // for
	} // for
	
	return isInFrame(ttemp.at(0).name) ;
}//Find_Smallest_counter_Value

void Put_samllest_to_the_leftMost2() {
	int index = Find_Smallest_counter_Value() ; // 最小的 
	pageInfo temmp ; 
	temmp.name = PageFrameInMem.at(index).name ;
	temmp.refBit = PageFrameInMem.at(index).refBit ;
	temmp.timeStamp = PageFrameInMem.at(index).timeStamp ;
	temmp.counter = PageFrameInMem.at(index).counter ;
	for (int c = 0; c < 8; c++) {
		temmp.shiftReg[c] = PageFrameInMem.at(index).shiftReg[c];
	} // for 
	
	for( int x = index ; x-1 >= 0 ; x-- ) {
		PageFrameInMem.at(x).name = PageFrameInMem.at(x-1).name;
		PageFrameInMem.at(x).refBit = PageFrameInMem.at(x-1).refBit;
		PageFrameInMem.at(x).timeStamp = PageFrameInMem.at(x-1).timeStamp;
		PageFrameInMem.at(x).counter = PageFrameInMem.at(x-1).counter;
		for (int c = 0; c < 8; c++) {
			PageFrameInMem.at(x).shiftReg[c] = PageFrameInMem.at(x-1).shiftReg[c];
		} // for 
	} // for
	
	PageFrameInMem.at(0).name = temmp.name;
	PageFrameInMem.at(0).refBit = temmp.refBit;
	PageFrameInMem.at(0).timeStamp = temmp.timeStamp;
	PageFrameInMem.at(0).counter = temmp.counter;
	for (int c = 0; c < 8; c++) {
		PageFrameInMem.at(0).shiftReg[c] = temmp.shiftReg[c];
	} // for 
	
} // Put_samllest_to_the_leftMost

void Least_Frequently_Used() {
	LFUr.clear();
	LFUr2.frames = pageFrame;
	LFUr2.pageFaults = 0;
	LFUr2.pageReplace = 0;

	int t = 0; // time
	CreatePageFrame(10);
	while (t < PageRefSeq.size()) {
		reSult oneR;
		oneR.ref = PageRefSeq.at(t);
		int InFrame = isInFrame(oneR.ref);

		if (InFrame != -1) { // 如果已經在頁框中 
			oneR.pageFault = '\0';
			PageFrameInMem.at(InFrame).counter++;
			PageFrameInMem.at(InFrame).timeStamp = t ;
			oneR.currentFrame.assign(PageFrameInMem.begin(), PageFrameInMem.end());
		} // if
		else {   // 如果沒在頁框中 
			oneR.pageFault = 'F';
			LFUr2.pageFaults++;
			int gg = hasSpace();
			if (gg != -1) { // if 有空間 
				PageFrameInMem.at(gg).name = PageRefSeq.at(t);
				PageFrameInMem.at(gg).counter = 0;
			} // if
			else { // 沒空間了 
				LFUr2.pageReplace++;
				int lefmost = 0 ;
				bool hasKick = false ; // 有踢掉一頁了嗎? 
				while ( !hasKick ) {
						Put_samllest_to_the_leftMost2() ;
						hasKick = true ;
						PageShift() ; 
						PageFrameInMem.at(PageFrameInMem.size()-1).name = PageRefSeq.at(t) ;
						PageFrameInMem.at(PageFrameInMem.size()-1).counter = 0;
						PageFrameInMem.at(PageFrameInMem.size()-1).timeStamp = t ;
					
				} // while
			} // else 

			oneR.currentFrame.assign(PageFrameInMem.begin(), PageFrameInMem.end());
		} // else 

		LFUr.push_back(oneR);
		cout << PageRefSeq.at(t) << endl ; // test
		for( int z = 0 ; z < PageFrameInMem.size() ; z++ ){ // test
			cout << PageFrameInMem.at(z).name << ": " << PageFrameInMem.at(z).counter <<endl ; 
		} // for
		cout << "--------------------" << endl ; // test
		
		t++;
	} // while
} // Least_Frequently_Used()

  //-----------------------------------------------------------------------------

int Find_biggest_counter_Value() {
	vector<pageInfo> ttemp ; 
	ttemp.assign(PageFrameInMem.begin(), PageFrameInMem.end()) ; 
	for (int i = 0; i < ttemp.size() - 1; i++ ) {   // BubbleByTimeStamp
		for (int j = 0; j < ttemp.size() - i-1; j++) {
			if (ttemp.at(j).counter < ttemp.at(j + 1).counter) {
				swap(ttemp.at(j), ttemp.at(j + 1));
			} // if
			else if(ttemp.at(j).counter == ttemp.at(j + 1).counter &&
			 ttemp.at(j).timeStamp > ttemp.at(j + 1).timeStamp ){
				swap(ttemp.at(j), ttemp.at(j + 1));
			} // else if
		} // for
	} // for
	
	return isInFrame(ttemp.at(0).name) ;
}//Find_biggest_counter_Value

void Put_biggest_to_the_leftMost() {
	int index = Find_biggest_counter_Value() ; // 最大的 
	pageInfo temmp ; 
	temmp.name = PageFrameInMem.at(index).name ;
	temmp.refBit = PageFrameInMem.at(index).refBit ;
	temmp.timeStamp = PageFrameInMem.at(index).timeStamp ;
	temmp.counter = PageFrameInMem.at(index).counter ;
	for (int c = 0; c < 8; c++) {
		temmp.shiftReg[c] = PageFrameInMem.at(index).shiftReg[c];
	} // for 
	
	for( int x = index ; x-1 >= 0 ; x-- ) {
		PageFrameInMem.at(x).name = PageFrameInMem.at(x-1).name;
		PageFrameInMem.at(x).refBit = PageFrameInMem.at(x-1).refBit;
		PageFrameInMem.at(x).timeStamp = PageFrameInMem.at(x-1).timeStamp;
		PageFrameInMem.at(x).counter = PageFrameInMem.at(x-1).counter;
		for (int c = 0; c < 8; c++) {
			PageFrameInMem.at(x).shiftReg[c] = PageFrameInMem.at(x-1).shiftReg[c];
		} // for 
	} // for
	
	PageFrameInMem.at(0).name = temmp.name;
	PageFrameInMem.at(0).refBit = temmp.refBit;
	PageFrameInMem.at(0).timeStamp = temmp.timeStamp;
	PageFrameInMem.at(0).counter = temmp.counter;
	for (int c = 0; c < 8; c++) {
		PageFrameInMem.at(0).shiftReg[c] = temmp.shiftReg[c];
	} // for 
	
} // Put_biggest_to_the_leftMost

void Most_Frequently_Used() {
	MFUr.clear();
	MFUr2.frames = pageFrame;
	MFUr2.pageFaults = 0;
	MFUr2.pageReplace = 0;

	int t = 0; // time
	CreatePageFrame(10);
	while (t < PageRefSeq.size()) {
		reSult oneR;
		oneR.ref = PageRefSeq.at(t);
		int InFrame = isInFrame(oneR.ref);

		if (InFrame != -1) { // 如果已經在頁框中 
			oneR.pageFault = '\0';
			PageFrameInMem.at(InFrame).counter++;
			PageFrameInMem.at(InFrame).timeStamp = t ;
			oneR.currentFrame.assign(PageFrameInMem.begin(), PageFrameInMem.end());
		} // if
		else {   // 如果沒在頁框中 
			oneR.pageFault = 'F';
			MFUr2.pageFaults++;
			int gg = hasSpace();
			if (gg != -1) { // if 有空間 
				PageFrameInMem.at(gg).name = PageRefSeq.at(t);
				PageFrameInMem.at(gg).counter++;
			} // if
			else { // 沒空間了 
				MFUr2.pageReplace++;
				int lefmost = 0 ;
				bool hasKick = false ; // 有踢掉一頁了嗎? 
				while ( !hasKick ) {
						Put_biggest_to_the_leftMost() ;
						hasKick = true ;
						PageShift() ; 
						PageFrameInMem.at(PageFrameInMem.size()-1).name = PageRefSeq.at(t) ;
						PageFrameInMem.at(PageFrameInMem.size()-1).counter = 0;
						PageFrameInMem.at(PageFrameInMem.size()-1).timeStamp = t ;
					
				} // while
			} // else 

			oneR.currentFrame.assign(PageFrameInMem.begin(), PageFrameInMem.end());
		} // else 

		MFUr.push_back(oneR);

		t++;
	} // while
} // Most_Frequently_Used()

  //-----------------------------------------------------------------------------

int main(void)
{


	ReadAll();
	CreatePageFrame(7); // 初始化 

	FIFO();
	LRU() ;
	AdditionalReferenceBits() ;
	SecondChance() ;
	Least_Frequently_Used() ;
	Most_Frequently_Used() ; 
//	cout << pageFrame << endl ; // test 
//	for ( int a = 0 ; a < PageRefSeq.size() ; a++ ) { // test
//		cout << PageRefSeq.at(0) ; 
//	} // for 

	if ( writeRecords() ) { // 寫檔 
		cout << "Write file successful !" << endl ;
	} // if
	else {
		cout << "Write file ERROR !!" << endl ;
	} // else 


	return 0;

} // main 
