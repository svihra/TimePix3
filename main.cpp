#include <iostream>
#include <TApplication.h>
#include <TStopwatch.h>

#include <TString.h>
#include <TObjString.h>
#include <TList.h>
#include <TSystemDirectory.h>

#include "dataprocess.h"

int main(int argc, char **argv)
{
    TStopwatch time;
    time.Start();

    bool bCol = true;
    bool bRow = true;
    bool bToA = true;
    bool bToT = true;
    bool bTrig = true;
    bool bTrigTime = true;
    bool bTrigToA = true;
    bool bProcTree = true;
    bool bCsv = false;
    bool bCentroid = true;
    int gapPix = 1;
    float gapTime = 1.0;
    bool bNoTrigWindow = true;
    bool bForwardTrigger = true;

    float timeWindow = 0;
    float timeStart = 0;
    bool bSingleFile = true;

    int c;
    bool bash = false;
    bool combine = false;
    TString dirname = ".";
    TString filename = "";
    TString process = "procAll";
    UInt_t maxEntries = 0;

    while( ( c = getopt (argc, argv, ":bctd:f:p:e:") ) != -1 )
    {
        switch(c)
        {
            case 'b':
                bash = true;
                break;
            case 'c':
                bCsv = true;
                break;
            case 't':
                bForwardTrigger = false;
                break;
            case 'd':
                if (optarg){ dirname = (TString) optarg; combine = true; break;}
                else {std::cout << "Missing dir name" << std::endl;}
            case 'f':
                if(optarg){ filename = (TString) optarg; break;}
                else {std::cout << "Missing file name" << std::endl;}
            case 'p':
                if(optarg){ process = (TString) optarg; break;}
                else {std::cout << "Missing process type" << std::endl;}
            case 'e':
                if(optarg){ maxEntries = (UInt_t) std::atoll(optarg); break;}
                else {std::cout << "Define max entries " << std::endl;}
            case '?':
                std::cout << "Received unknown argument " << c << std::endl;
            default :
                std::cout << "Exiting code. Usage: " << std::endl;
                std::cout << "-b executes in bash" << std::endl;
                std::cout << "-c creates csv output" << std::endl;
                std::cout << "-t backward trigger" << std::endl;
                std::cout << "-d [] name of folder in which to combine (do not use -f)" << std::endl;
                std::cout << "-f [] name of file to process (do not use -d)" << std::endl;
                std::cout << "-p [] procAll/procDat/procRoot" << std::endl;
                std::cout << "-e [] max number of entries" << std::endl;
                return -1;
            break;
        }
    }
    if (bash)
    {
        DataProcess* processor = new DataProcess();

        if (process.Contains("procRoot"))
            processor->setProcess(ProcType::procRoot);
        else if (process.Contains("procDat"))
            processor->setProcess(ProcType::procDat);
        else
            processor->setProcess(ProcType::procAll);

        processor->setNEntries(maxEntries);
        processor->setOptions(bCol, bRow, bToT, bToA, bTrig, bTrigTime, bTrigToA, bProcTree, bCsv, bCentroid, gapPix, gapTime, bForwardTrigger, bNoTrigWindow, timeWindow, timeStart, bSingleFile);

        if (combine)
        {
            TObjString inputNames[256];
            int inputNumber = 0;

            TSystemDirectory dir(dirname, dirname);
            TList *files = dir.GetListOfFiles();
            if (files)
            {
                TSystemFile *file;
                TString fname;
                TIter next(files);
                while ((file=(TSystemFile*)next()))
                {
                    fname = file->GetName();
                    if (!file->IsDirectory() && fname.EndsWith(".dat"))
                    {
                        std::cout << fname << " " << inputNumber << " at " << dirname << std::endl;
                        inputNames[inputNumber++].SetString(dirname+"/"+fname);
                    }
                }
            }
            processor->setName(inputNames, inputNumber);

            if (process.Contains("procRoot"))
                processor->setCorrection(CorrType::corrUse, inputNames[0].GetString() + "_LTcorr.csv");
            else
                processor->setCorrection(CorrType::corrNew, inputNames[0].GetString() + "_LTcorr.csv");
        }
        else
        {
            std::cout << filename << " " << " at " << dirname << std::endl;
            processor->setName(filename);
            if (process.Contains("procRoot"))
                processor->setCorrection(CorrType::corrUse, filename + "_LTcorr.csv");
            else
                processor->setCorrection(CorrType::corrNew, filename + "_LTcorr.csv");
        }

        processor->process();
    }
//    else
//    {
//        TApplication theApp("App",&argc,argv);
//
//        new DrGUI(); // qt written app
//        theApp.Run();
//    }

    time.Stop();
    time.Print();
    return 0;
}

