//==================================
// Author: IRAKLI CHAKABERIA   =====
// Coauthor: PETER SVIHRA      =====
// Date: 2017-02-14            =====
//==================================

#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TG3DLine.h>
#include <TApplication.h>
#include <TSysEvtHandler.h>

#include "TGFileDialog.h"
#include "TGTextEdit.h"
#include "TGText.h"
#include "TGDimension.h"
#include "TObjString.h"

#include <deque>
#include "dataprocess.h"

#include <iostream>

const char * fileTypes[] = {
        "data files", "*.dat",
        "SoPhy files", "*.tpx3",
        "ROOT files", "*.root",
	0, 0
};

const char * fileTypesCorr[] = {
        "lookup table", "*.csv",
        0, 0
};

class TextMargin : public TGHorizontalFrame {
	protected:
	TGNumberEntry * fEntry;
	TGLabel * label;

	public:
        TextMargin(const TGWindow *p, const char *name, TGNumberFormat::EStyle style = TGNumberFormat::kNESInteger) : TGHorizontalFrame(p)
        {
                fEntry = new TGNumberEntry(this, 0, 6, -1, style);
		AddFrame(fEntry, new TGLayoutHints(kLHintsLeft));
		label = new TGLabel(this, name);
		AddFrame(label, new TGLayoutHints(kLHintsLeft, 10));
	}
	TGTextEntry * GetEntry() const { return fEntry->GetNumberEntry(); }
	void SetEntry(int entry) { fEntry->SetNumber(entry); }
	void SetEnabled(bool state) {
//                fEntry->SetState(!state);
                if (label->IsDisabled()) { label->Enable(); fEntry->SetState(kTRUE); }
                else { label->Disable(); fEntry->SetState(kFALSE); }
	}

//        ClassDef(TextMargin, 0);
};

class DRGui : public TGMainFrame {
protected:
	TGTextButton * fButton;
        TGTextButton * sButton;
        TGTextButton * bButton;
        TGTextButton * cButton;
        TGTextEdit * rootFile;
        TGTextEdit * corrFile;

        TSignalHandler * sigHandler;

public:
	bool bnHits;
	bool bCol;
	bool bRow;
	bool bToA;
	bool bToT;
        bool bTrig;
        bool bTrigTime;
        bool bTrigToA;
        bool bCentroid;
        bool bProcTree;
        bool bCsv;

        CorrType ctCorrection;
        TString stCorrection;
        TGRadioButton * off;
        TGRadioButton * create;

        ProcType ptProcess;
        bool bCombineInput;
        int inputNumber;

	bool bSingleFile;
	bool bNoTrigWindow;

        float timeWindow;
        float timeStart;
	int linesPerFile;

        int    m_gapPix;
        double m_gapTime;

	DRGui();
        void ProcessNames();
	void RunReducer();
        void StopReducer();
	void ApplyCutsHits(char *);
        void ApplyCutsWindow(char *);
        void ApplyCutsStart(char *);
	void ApplyCutsLines(char *);

        void ApplyCutsCentroidPixel(char * val);
        void ApplyCutsCentroidTime(char * val);

        void SelectOff(Bool_t);
        void SelectUse(Bool_t);
        void SelectNew(Bool_t);

	void SelectCol(Bool_t);
	void SelectRow(Bool_t);
	void SelectToA(Bool_t);
	void SelectToT(Bool_t);
        void SelectTrig(Bool_t);
        void SelectTrigTime(Bool_t);
        void SelectTrigToA(Bool_t);
        void SelectData(Bool_t);
        void SelectRoot(Bool_t);
        void SelectAll(Bool_t);
        void SelectProcTree(Bool_t);
        void SelectCsv(Bool_t);
	void SelectSingleFile(Bool_t);
	void SelectNoTrigWindow(Bool_t);
        void SelectCent(Bool_t);

        void SelectCombined(Bool_t);

	void Browse();
        void BrowseCorr();

//        ClassDef(DRGui, 0);

private:
        TString m_infoMsg;
        TObjString m_inputNames[32];
};

void DRGUI() {
    //
    // Initialize library
//    gROOT->ProcessLine(".L dataprocess.cpp+");

    new DRGui();
}

DRGui::DRGui() : TGMainFrame(gClient->GetRoot(), 10, 20, kHorizontalFrame) {
	SetCleanup(kDeepCleanup);

//	All variables are selected by default
	bCol = true;
	bRow = true;
	bToA = true;
	bToT = true;
        bTrig = true;
        bTrigTime = true;
        bTrigToA = true;
        bProcTree = false;
        bCsv = true;
	bNoTrigWindow = false;
        bSingleFile = false;
        bCombineInput = false;
        bCentroid = false;
        inputNumber = 0;

        ctCorrection = corrOff;

        ptProcess = procAll;

	linesPerFile = 100000;
        timeWindow = 40;
        timeStart = 0;

        m_gapPix = 0;
        m_gapTime = 1.0;


        m_infoMsg = "Select File";

//	Controls on right
	TGVerticalFrame * controls = new TGVerticalFrame(this);
        AddFrame(controls, new TGLayoutHints(kLHintsRight | kLHintsNormal | kLHintsExpandY, 5, 5, 5, 5));

//	VARIABLE and PROCESSES ON LEFT
	TGVerticalFrame * variables = new TGVerticalFrame(this);
        AddFrame(variables, new TGLayoutHints(kLHintsLeft | kLHintsNormal | kLHintsExpandY, 5, 5, 5, 5));

//	Separator
	TGVertical3DLine *separator = new TGVertical3DLine(this);
        AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));

	TGVertical3DLine *separator1 = new TGVertical3DLine(this);
	AddFrame(separator1, new TGLayoutHints(kLHintsExpandY));

//	Contents 
	TGVerticalFrame * contents = new TGVerticalFrame(this);
	AddFrame(contents, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

//	The button for test
	fButton = new TGTextButton(contents, "&RUN THE DATA REDUCER");
//	fButton->Resize(200, 200);
//	fButton->ChangeOptions(fButton->GetOptions() | kFixedSize);
	fButton->SetToolTipText("Run the reducer macro", 200);

	fButton->Connect("Pressed()", "DRGui", this, "RunReducer()");

//      STOPPING EXECUTION
        sButton = new TGTextButton(contents, "&STOP THE DATA REDUCER");
        sButton->SetToolTipText("Stop the reducer macro", 200);

        sButton->Connect("Pressed()", "DRGui", this, "StopReducer()");

        sigHandler = new TSignalHandler(kSigInterrupt);
        sigHandler->Add();
        sigHandler->Connect("Notified()", "DRGui", this, "StopReducer()");

//      CORRECTION USAGE
        TGGroupFrame * corrGroup = new TGGroupFrame(contents, "Timewalk Correction");

        corrGroup->SetTitlePos(TGGroupFrame::kCenter);

        TGHButtonGroup * correction = new TGHButtonGroup(corrGroup);

                        off    = new TGRadioButton(correction, "off");
        TGRadioButton * use    = new TGRadioButton(correction, "use");
                        create = new TGRadioButton(correction, "new");

        off   ->SetOn();
        use   ->SetOn(kFALSE);
        create->SetOn(kFALSE);

        correction->AddFrame(off   ,   new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        correction->AddFrame(use   ,   new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        correction->AddFrame(create,   new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        off    ->Connect("Toggled(Bool_t)", "DRGui", this, "SelectOff(Bool_t)");
        use    ->Connect("Toggled(Bool_t)", "DRGui", this, "SelectUse(Bool_t)");
        create ->Connect("Toggled(Bool_t)", "DRGui", this, "SelectNew(Bool_t)");
        create->SetEnabled(kFALSE);

        correction->SetRadioButtonExclusive(kTRUE);


        corrFile = new TGTextEdit(corrGroup, 600,60);
        cButton = new TGTextButton(corrGroup, "Browse Correction File");
        corrGroup->AddFrame(correction, new TGLayoutHints(kLHintsExpandX));
        corrGroup->AddFrame(corrFile, new TGLayoutHints(kLHintsExpandX));
        corrGroup->AddFrame(cButton, new TGLayoutHints(kLHintsExpandX));

        cButton->Connect("Clicked()", "DRGui", this, "BrowseCorr()");
        cButton->SetEnabled(kFALSE);

        contents->AddFrame(corrGroup, new TGLayoutHints(kLHintsExpandX));

//	BROWSE FILE
        rootFile = new TGTextEdit(contents, 600,200, m_infoMsg);
        //rootFile->SetText();
	bButton = new TGTextButton(contents, "Browse Files");
        contents->AddFrame(rootFile, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
        contents->AddFrame(bButton, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 10, 10, 10, 10));
        contents->AddFrame(fButton, new TGLayoutHints(kLHintsBottom| kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
        contents->AddFrame(sButton, new TGLayoutHints(kLHintsBottom| kLHintsExpandX, 5, 5, 5, 5));

        bButton->Connect("Clicked()", "DRGui", this, "Browse()");

//	VARIABLES
	TGGroupFrame * varsGroup = new TGGroupFrame(variables, "Variables");
	varsGroup->SetTitlePos(TGGroupFrame::kCenter);
        TGCheckButton * col     = new TGCheckButton(varsGroup, "Col");
        TGCheckButton * row     = new TGCheckButton(varsGroup, "Row");
        TGCheckButton * toa     = new TGCheckButton(varsGroup, "ToA");
        TGCheckButton * tot     = new TGCheckButton(varsGroup, "ToT");
        TGCheckButton * trig    = new TGCheckButton(varsGroup, "Trigger ID");
        TGCheckButton * trigTime= new TGCheckButton(varsGroup, "Trigger Time");
        TGCheckButton * trigToA = new TGCheckButton(varsGroup, "ToA-Trigger");

	col->SetOn();
	row->SetOn();
	toa->SetOn();
	tot->SetOn();
        trig->SetOn();
        trigTime->SetOn();
        trigToA->SetOn();

        varsGroup->AddFrame(col,     new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        varsGroup->AddFrame(row,     new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        varsGroup->AddFrame(toa,     new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        varsGroup->AddFrame(tot,     new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        varsGroup->AddFrame(trig,    new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        varsGroup->AddFrame(trigTime,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        varsGroup->AddFrame(trigToA, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        col     ->Connect("Toggled(Bool_t)", "DRGui", this, "SelectCol(Bool_t)");
        row     ->Connect("Toggled(Bool_t)", "DRGui", this, "SelectRow(Bool_t)");
        toa     ->Connect("Toggled(Bool_t)", "DRGui", this, "SelectToA(Bool_t)");
        tot     ->Connect("Toggled(Bool_t)", "DRGui", this, "SelectToT(Bool_t)");
        trig    ->Connect("Toggled(Bool_t)", "DRGui", this, "SelectTrig(Bool_t)");
        trigTime->Connect("Toggled(Bool_t)", "DRGui", this, "SelectTrigTime(Bool_t)");
        trigToA ->Connect("Toggled(Bool_t)", "DRGui", this, "SelectTrigToA(Bool_t)");

        variables->AddFrame(varsGroup, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

//      PROCESSES
        TGVButtonGroup * procGroup = new TGVButtonGroup(variables, "Process");
        procGroup->SetTitlePos(TGGroupFrame::kCenter);

        TGRadioButton * all     = new TGRadioButton(procGroup, "all");
        TGRadioButton * data    = new TGRadioButton(procGroup, "data");
        TGRadioButton * root    = new TGRadioButton(procGroup, "root");

        TGCheckButton * procTree = new TGCheckButton(procGroup, "procTree");
        TGCheckButton * csv      = new TGCheckButton(procGroup, "csv");

        all ->SetOn();
        data->SetOn(kFALSE);
        root->SetOn(kFALSE);
        procTree->SetOn(kFALSE);
        csv->SetOn(kTRUE);

        procGroup->AddFrame(procTree,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        procGroup->AddFrame(all,     new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        procGroup->AddFrame(data,    new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
        procGroup->AddFrame(root,    new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        all     ->Connect("Toggled(Bool_t)", "DRGui", this, "SelectAll(Bool_t)");
        data    ->Connect("Toggled(Bool_t)", "DRGui", this, "SelectData(Bool_t)");
        root    ->Connect("Toggled(Bool_t)", "DRGui", this, "SelectRoot(Bool_t)");
        procTree->Connect("Toggled(Bool_t)", "DRGui", this, "SelectProcTree(Bool_t");
        csv     ->Connect("Toggled(Bool_t)", "DRGui", this, "SelectCsv(Bool_t");

        procGroup->SetRadioButtonExclusive(kTRUE);
        variables->AddFrame(procGroup, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

//	CUTS
	TGGroupFrame * cuts = new TGGroupFrame(controls, "Cuts");
	cuts->SetTitlePos(TGGroupFrame::kCenter);

        TextMargin * timeW = new TextMargin(cuts, "Time Window Size (micro s)", TGNumberFormat::kNESRealOne);
        timeW->SetEntry(timeWindow);
        TextMargin * timeS = new TextMargin(cuts, "Time Window Start (micro s)", TGNumberFormat::kNESRealOne);
        timeS->SetEntry(timeStart);
	TGCheckButton * noTrigWindow = new TGCheckButton(cuts, "All Data");

        cuts->AddFrame(timeS, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 2, 2));
        cuts->AddFrame(timeW, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 2, 2));
        cuts->AddFrame(noTrigWindow, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 2, 2));

        noTrigWindow->Connect("Toggled(Bool_t)", "TextMargin", timeS, "SetEnabled(Bool_t)");
        noTrigWindow->Connect("Toggled(Bool_t)", "TextMargin", timeW, "SetEnabled(Bool_t)");
	noTrigWindow->Connect("Toggled(Bool_t)", "DRGui", this, "SelectNoTrigWindow(Bool_t)");
        timeW->GetEntry()->Connect("TextChanged(char*)", "DRGui", this, "ApplyCutsWindow(char*)");
        timeS->GetEntry()->Connect("TextChanged(char*)", "DRGui", this, "ApplyCutsStart(char*)");

        controls->AddFrame(cuts, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

//	OUTPUT CONTROL
	TGGroupFrame * outControl = new TGGroupFrame(controls, "Output File");
	outControl->SetTitlePos(TGGroupFrame::kCenter);

	TGCheckButton * singleFile = new TGCheckButton(outControl, "Single File");
        TextMargin * nLines = new TextMargin(outControl, "Lines per File");

        outControl->AddFrame(singleFile, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 2, 2));
        outControl->AddFrame(nLines, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 2, 2));

        nLines->SetEntry(linesPerFile);
	nLines->GetEntry()->Connect("TextChanged(char*)", "DRGui", this, "ApplyCutsLines(char*)");
	singleFile->Connect("Toggled(Bool_t)", "TextMargin", nLines, "SetEnabled(Bool_t)");
	singleFile->Connect("Toggled(Bool_t)", "DRGui", this, "SelectSingleFile(Bool_t)");

        controls->AddFrame(outControl, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

//      INPUT CONTROL
        TGVButtonGroup * combGroup = new TGVButtonGroup(controls, "Input File");
        combGroup->SetTitlePos(TGGroupFrame::kCenter);
        TGCheckButton * combIn = new TGCheckButton(combGroup, "Combine .dat files");

        combIn->SetOn(bCombineInput);

        combGroup->AddFrame(combIn,    new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        combIn->Connect("Toggled(Bool_t)", "DRGui", this, "SelectCombined(Bool_t)");

        controls->AddFrame(combGroup, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

//	CENTROID CONTROL
        TGGroupFrame * centControl = new TGGroupFrame(controls, "Centroiding");
        centControl->SetTitlePos(TGGroupFrame::kCenter);

        TGCheckButton * cent = new TGCheckButton(centControl, "Centroid");
        TextMargin * gapPix  = new TextMargin(centControl, "Allowed pix gap", TGNumberFormat::kNESInteger);
        TextMargin * gapTime = new TextMargin(centControl, "Time Window (micro s)", TGNumberFormat::kNESRealTwo);

        centControl->AddFrame(cent,     new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 2, 2));
        centControl->AddFrame(gapPix,   new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 2, 2));
        centControl->AddFrame(gapTime,  new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 2, 2));

        gapPix ->SetEntry(m_gapPix);
        gapPix ->SetEnabled(kTRUE);
        gapTime->SetEntry(m_gapTime);
        gapTime ->SetEnabled(kTRUE);

        gapPix ->GetEntry()->Connect("TextChanged(char*)", "DRGui", this, "ApplyCutsCentroidPixel(char*)");
        gapTime->GetEntry()->Connect("TextChanged(char*)", "DRGui", this, "ApplyCutsCentroidTime(char*)");
        cent    ->Connect("Toggled(Bool_t)", "TextMargin", gapPix,  "SetEnabled(Bool_t)");
        cent    ->Connect("Toggled(Bool_t)", "TextMargin", gapTime, "SetEnabled(Bool_t)");
        cent    ->Connect("Toggled(Bool_t)", "DRGui",      this,    "SelectCent(Bool_t)");

        controls->AddFrame(centControl, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));


//	QUIT
	TGTextButton *quit = new TGTextButton(controls, "Quit");
	controls->AddFrame(quit, new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 0, 0, 0, 5));
	quit->Connect("Pressed()", "TApplication", gApplication, "Terminate()");

	Connect("CloseWindow()", "TApplication", gApplication, "Terminate()");
	DontCallClose();

	MapSubwindows();
	Resize();

	SetWMSizeHints(GetDefaultWidth(), GetDefaultHeight(), 1000, 1000, 0 ,0);
	SetWindowName("RRGui");
	MapRaised();
}

void DRGui::ApplyCutsWindow(char * val){ timeWindow = abs(atof(val)); }
void DRGui::ApplyCutsStart(char * val) { timeStart  = abs(atof(val)); }
void DRGui::ApplyCutsLines(char * val) { linesPerFile = abs(atoi(val)); }

void DRGui::ApplyCutsCentroidPixel(char * val) { m_gapPix = abs(atoi(val)); }
void DRGui::ApplyCutsCentroidTime (char * val) { m_gapTime  = abs(atof(val)); }

void DRGui::SelectCol(Bool_t check)      { bCol = check; }
void DRGui::SelectRow(Bool_t check)      { bRow = check; }
void DRGui::SelectToA(Bool_t check)      { bToA = check; }
void DRGui::SelectToT(Bool_t check)      { bToT = check; }
void DRGui::SelectTrig(Bool_t check)     { bTrig = check; }
void DRGui::SelectTrigTime(Bool_t check) { bTrigTime = check; }
void DRGui::SelectTrigToA(Bool_t check)  { bTrigToA = check; }

void DRGui::SelectCent(Bool_t check)    { bCentroid = check; if (!check) {off->SetOn(kTRUE); create->SetOn(kFALSE);} create->SetEnabled(check);}
void DRGui::SelectAll(Bool_t check)     { if (check) ptProcess = procAll; }
void DRGui::SelectData(Bool_t check)    { if (check) ptProcess = procDat; }
void DRGui::SelectRoot(Bool_t check)    { if (check) ptProcess = procRoot; }
void DRGui::SelectProcTree(Bool_t check){ bProcTree = check; }

void DRGui::SelectCsv(Bool_t check){ bCsv = check; }

void DRGui::SelectOff(Bool_t check) { if (check) ctCorrection = corrOff; corrFile->Clear(); cButton->SetEnabled(!check); }
void DRGui::SelectUse(Bool_t check) { if (check) ctCorrection = corrUse; corrFile->Clear(); cButton->SetEnabled(check); }
void DRGui::SelectNew(Bool_t check) { if (check) ctCorrection = corrNew; corrFile->Clear(); cButton->SetEnabled(!check); }

void DRGui::SelectCombined(Bool_t check)    { if (check) bCombineInput = check; }

void DRGui::SelectSingleFile(Bool_t check) { bSingleFile = check; }
void DRGui::SelectNoTrigWindow(Bool_t check) { bNoTrigWindow = check; }

void DRGui::BrowseCorr()
{
        static TString dir(".");
        TGFileInfo fileInfo;
        fileInfo.SetMultipleSelection(kFALSE);
        fileInfo.fFileTypes = fileTypesCorr;
        fileInfo.fIniDir = StrDup(dir);

        //
        // browsing dialog - multiple files or one file only
        new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fileInfo);
        if (fileInfo.fMultipleSelection && fileInfo.fFileNamesList)
        {
            corrFile->Clear();

            TObjString *el;
            TIter next(fileInfo.fFileNamesList);
            while ((el = (TObjString *) next()))
            {
              corrFile->AddLine(el->GetString());
            }
        }
        else if (fileInfo.fFilename)
        {
            corrFile->Clear();
            corrFile->AddLine(fileInfo.fFilename);
        }
        corrFile->NextChar();
        corrFile->DelChar();
        dir = fileInfo.fIniDir;
}

void DRGui::Browse()
{
	static TString dir(".");
	TGFileInfo fileInfo;
        fileInfo.SetMultipleSelection(kTRUE);
	fileInfo.fFileTypes = fileTypes;
	fileInfo.fIniDir = StrDup(dir);

        //
        // browsing dialog - multiple files or one file only
        new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fileInfo);
        if (fileInfo.fMultipleSelection && fileInfo.fFileNamesList)
        {
            rootFile->Clear();

            TObjString *el;
            TIter next(fileInfo.fFileNamesList);
            while ((el = (TObjString *) next()))
            {
              rootFile->AddLine(el->GetString());
            }
        }
        else if (fileInfo.fFilename)
        {
            rootFile->Clear();
            rootFile->AddLine(fileInfo.fFilename);
        }
        rootFile->NextChar();
        rootFile->DelChar();
        dir = fileInfo.fIniDir;
}

void DRGui::RunReducer()
{
    ProcessNames();
    TString tmpString;
    TStopwatch time;
    time.Start();
    DataProcess* processor = new DataProcess();
    processor->setCorrection(ctCorrection, stCorrection);

    if (bCombineInput)
    {
        processor->setName(m_inputNames, inputNumber);
        processor->setProcess(ptProcess);
        processor->setOptions(bCol, bRow, bToT, bToA, bTrig, bTrigTime, bTrigToA, bProcTree, bCsv, bCentroid, m_gapPix, m_gapTime, bNoTrigWindow, timeWindow, timeStart, bSingleFile, linesPerFile);
        processor->process();
    }
    else
    {
        for (int i = 0; i <  inputNumber; i++)
        {
            tmpString = m_inputNames[i].GetString();
            if (tmpString.EndsWith(".root"))
            {
                processor->setProcess(procRoot);
            }
            else if (tmpString.EndsWith(".dat") || tmpString.EndsWith(".tpx3"))
            {
                processor->setProcess(ptProcess);
            }

            processor->setName(tmpString);
            processor->setOptions(bCol, bRow, bToT, bToA, bTrig, bTrigTime, bTrigToA, bProcTree, bCsv, bCentroid, m_gapPix, m_gapTime, bNoTrigWindow, timeWindow, timeStart, bSingleFile, linesPerFile);
            processor->process();
        }
    }
    time.Stop();
    std::cout << std::endl;
    std::cout << "###################################" << std::endl;
    std::cout << "Total Runtime of the code: " << std::endl;
    time.Print();

    delete processor;
}

void DRGui::ProcessNames()
{
    TString fileName = "";
    TGText* inputFileNames = new TGText(rootFile->GetText());
    TGText* correctionFileNames = new TGText(corrFile->GetText());
    TGLongPosition pos = TGLongPosition(0,0);

    Int_t lineNumber = 0;
    Int_t lineLength = 0;
    inputNumber = 0;
    stCorrection = correctionFileNames->GetLine(pos,correctionFileNames->GetLineLength(0));
    while( (lineLength = inputFileNames->GetLineLength(lineNumber)) != -1)
    {
        pos = TGLongPosition(0,lineNumber++);
        fileName = inputFileNames->GetLine(pos,lineLength);

        if (fileName.EndsWith(".root") || fileName.EndsWith(".dat") || fileName.EndsWith(".tpx3"))
        {
            std::cout << fileName << " " << inputNumber << std::endl;
            m_inputNames[inputNumber++].SetString(fileName);
        }
        else
        {
            std::cout << " ========================================== " << std::endl;
            std::cout << " ======== INVALID ENTRY FILE NAME ========= " << std::endl;
            std::cout << fileName << endl;
            std::cout << " ========================================== " << std::endl;
        }
    }
}

void DRGui::StopReducer()
{
    std::cout << "==============================================="  << std::endl;
    std::cout << "======== REDUCER SUCCESSFULLY STOPPED ========="  << std::endl;
    std::cout << "==============================================="  << std::endl;
    gSystem->ExitLoop();
}
