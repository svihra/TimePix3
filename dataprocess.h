#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <deque>

#include "TSysEvtHandler.h"

#include "TObjString.h"
#include "TString.h"
#include "TStopwatch.h"

#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

#define MAXHITS 65536       // minimal value for sorting 65536

using namespace std;

enum ProcType
{
    procDat     = 0,
    procRoot    = 1,
    procAll     = 2
};

enum CorrType
{
    corrOff     = 0,
    corrUse     = 1,
    corrNew     = 2
};

enum DataType
{
    dtDat  = 0,
    dtTpx  = 1,
    dtCent = 2,
    dtTime = 3,

    dtStandard = 4
};

struct LookupTable
{
    UInt_t    ToT;
    Double_t  dToA;
};

class DataProcess
{
public:
    DataProcess();
    ~DataProcess();
    void setName(TString fileNameInput);
    void setName(TObjString *fileNameInput, Int_t size);
    void setProcess(ProcType process);
    void setCorrection(CorrType correction, TString fileNameCorrection = "./lookupTable.csv");
    void setNEntries(UInt_t nEntries);
    Int_t setOptions(Bool_t bCol = kTRUE,
                      Bool_t bRow = kTRUE,
                      Bool_t bToT = kTRUE,
                      Bool_t bToA = kTRUE,
                      Bool_t bTrig = kTRUE,
                      Bool_t bTrigTime = kTRUE,
                      Bool_t bTrigToA = kTRUE,
                      Bool_t bProcTree = kFALSE,
                      Bool_t bCsv = kTRUE,
                      Bool_t bCentroid = kFALSE,
                      Int_t gapPixel = 0,
                      Float_t gapTime = 1.0,
                      Bool_t bForwardTrig = kTRUE,
                      Bool_t bNoTrigWindow = kFALSE,
                      Float_t timeWindow = 40,
                      Float_t timeStart = 0,
                      Bool_t singleFile = kFALSE,
                      Int_t  linesPerFile = 100000);
    Int_t process();
    void StopLoop();

private:
    void Init();

    Int_t processFileNames();
    void finishMsg(TString fileName, TString operation, ULong64_t events, ULong64_t fileCounter = 1);
    ULong64_t roundToNs(ULong64_t number);

    Int_t openCorr(Bool_t create = kTRUE);
    Int_t openDat(ULong64_t fileCounter = 0);
    Int_t openCsv(DataType type = dtStandard, TString fileCounter = "");
    Int_t openRoot(ULong64_t fileCounter = 0);

    void  findCluster(ULong64_t index, ULong64_t stop, deque<UInt_t >* cols, deque<UInt_t >* rows, deque<ULong64_t >* toas, deque<UInt_t> *tots, deque<Bool_t >* centered, deque<ULong64_t> *indices);
    Int_t processDat();
    Int_t skipHeader();

    Int_t processRoot();
    void  plotStandardData();

    void loadCorrection();
    void createCorrection();

    void appendProcTree(ULong64_t ToATrig[], Double_t ToF[], Long64_t &entryTime, ULong64_t id = 0);

    void closeCorr();
    void closeDat();
    void closeCsv();
    void closeRoot();

private:
    //
    // global variables
    ProcType    m_process;
    CorrType    m_correction;
    TString     m_correctionName;
    ULong64_t   m_maxEntries;
    TStopwatch  m_time;
    ULong64_t   m_pixelCounter;
    TSignalHandler* m_sigHandler;
    Bool_t      m_bFirstTrig;
    Bool_t      m_bDevID;

    ULong64_t   m_chainCounter;
    ULong64_t   m_treeMaxEntries;

    //
    // options of GUI
    Bool_t m_bCol;
    Bool_t m_bRow;    
    Bool_t m_bToT;
    Bool_t m_bToA;
    Bool_t m_bTrig;
    Bool_t m_bTrigTime;
    Bool_t m_bTrigToA;
    Bool_t m_bProcTree;
    Bool_t m_bCsv;
    Bool_t m_bCentroid;
    Bool_t m_bNoTrigWindow;
    Bool_t m_bForwardTrig;
    Bool_t m_bSingleFile;
    Bool_t m_bCorrCsv;

    Double_t  m_timeWindow;
    Double_t  m_timeStart;

    Long64_t  m_linesPerFile;

    //
    // centroiding parameters
    ULong64_t      m_gapTime;
    UInt_t         m_gapPix;

    //
    // file names
    DataType        m_type;
    ULong64_t       m_numInputs;
    deque<TString > m_fileNameInput;
    TString         m_fileNamePath;
    TString         m_fileName;
    deque<TString > m_fileNameDat;
    deque<TString > m_fileNameRoot;
    TString         m_fileNamePdf;
    TString         m_fileNameCsv;
    TString         m_fileNameCentCsv;
    TString         m_fileNameTimeCsv;

    //
    // files
    FILE*           m_fileCorr;
    deque<FILE* >   m_filesDat;
    deque<TFile* >  m_filesRoot;
    deque<FILE* >   m_filesCsv;
    deque<FILE* >   m_filesCentCsv;
    deque<FILE* >   m_filesTimeCsv;

    //
    // ROOT trees
    deque<TTree* > m_rawTree;
    deque<TTree* > m_timeTree;
    deque<TTree* > m_procTree;

    Long64_t m_nCent;
    Long64_t m_nTime;

    deque<Long64_t > m_nCents;
    deque<Long64_t > m_nTimes;

    //
    // ROOT histograms
    TH2I* m_pixelMap;
    TH2F* m_pixelMapToT;
    TH2F* m_pixelMapToA;

    TH1I* m_histToT;
    TH1I* m_histToA;

    TH2I* m_pixelCentMap;
    TH2F* m_pixelCentMapToT;
    TH2F* m_pixelCentMapToA;

    TH1I* m_histCentToT;
    TH1I* m_histCentToA;

    TDirectory* m_scanDir;
    Float_t m_scanToT[1023][1023];
    TH2F* m_histCentScan[1023];
    TH2F* m_mapCorr;
    TH2F* m_mapCorrErr;

    TH2F* m_histCentToTvsToA;
    TH2F* m_histCorrToTvsToA;

    TH1I* m_histTrigger;

    TH1F* m_histSpectrum;
    TH2F* m_ToTvsToF;
    TH1F* m_histCorrSpectrum;
    TH2F* m_corrToTvsToF;

    TH1F* m_histCentSpectrum;
    TH2F* m_centToTvsToF;

    //
    // single pixel data
    UInt_t      m_row;
    UInt_t      m_col;
    Double_t    m_posX, m_posY;
    Double_t    m_stdevX, m_stdevY;
    ULong64_t   m_stdevToA;
    UInt_t      m_ToT;
    ULong64_t   m_ToA;

    UInt_t      m_Rows[MAXHITS];
    UInt_t      m_Cols[MAXHITS];
    UInt_t      m_Size;
    UInt_t      m_ToTs[MAXHITS];
    ULong64_t   m_ToTt;
    ULong64_t   m_ToAs[MAXHITS];

    //
    // trigger data
    UInt_t      m_trigCnt;
    ULong64_t   m_trigTime;
    ULong64_t   m_trigTimeNext;

    //
    // correction table
    deque<LookupTable> m_lookupTable;
};

#endif // DATAPROCESS_H
