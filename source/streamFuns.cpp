#include "streamFuns.h"
#include "ErrorWarning.h"
#include <fstream>
#include <sys/statvfs.h>
#include <stdio.h>
#define fstream_Chunk_Max 2147483647
unsigned long long fstreamReadBig(std::ifstream &S, char* A, unsigned long long N) {
    unsigned long long C=0;
    for (unsigned long long ii=0; ii<N/fstream_Chunk_Max; ii++) {
        S.read(A+C,fstream_Chunk_Max);
        C+=S.gcount();
        if (!S.good()) break;
    };
    S.read(A+C,N%fstream_Chunk_Max);
    C+=S.gcount();
    return C;
};

void fstreamWriteBig(std::ofstream &S, char* A, unsigned long long N, std::string fileName, std::string errorID, Parameters *P) {

    struct statvfs statvfsBuf;
    statvfs(fileName.c_str(), &statvfsBuf);
    P->inOut->logMain << "Writing " << N << " bytes into " <<fileName << " ; empty space on disk = " << statvfsBuf.f_bavail * statvfsBuf.f_bsize <<" bytes ..." <<flush;
    
    unsigned long long C=0;
    for (unsigned long long ii=0; ii<N/fstream_Chunk_Max; ii++) {
        S.write(A+C,fstream_Chunk_Max);
        C+=fstream_Chunk_Max;
    };
    S.write(A+C,N%fstream_Chunk_Max);
    if (S.fail()) {//failed to write

        struct statvfs statvfsBuf;
        statvfs(fileName.c_str(), &statvfsBuf);

        remove(fileName.c_str());
        
        ostringstream errOut;
        errOut << errorID<<": exiting because of *OUTPUT FILE* error: could not write the output file "<< fileName <<"\n";
        errOut << "fail()=" <<S.fail() <<" ; bad()="<< S.bad()<<"\n";
        errOut << "Solution: check that you have enough space on the disk\n";
        errOut <<"File size = "<< N <<" bytes\n";
        errOut <<"Empty space on disk = " << statvfsBuf.f_bavail * statvfsBuf.f_bsize <<" bytes\n";
        exitWithError(errOut.str(),std::cerr, P->inOut->logMain, EXIT_CODE_FILE_WRITE, *P);
    };
    P->inOut->logMain << " done\n" <<flush;
    C+=N%fstream_Chunk_Max;
};

void ofstrOpen (std::string fileName, std::string errorID, Parameters *P, ofstream & ofStream) {//open file 'fileName', generate error if cannot open
    ofStream.open(fileName.c_str());
    if (ofStream.fail()) {//
//         dir1=fileName.substr(0,fileName.find_last_of("/")+1);
//         if (dir1=="") dir1="./";
        ostringstream errOut;
        errOut << errorID<<": exiting because of *OUTPUT FILE* error: could not create output file "<< fileName <<"\n";
        errOut << "Solution: check that the path exists and you have write permission for this file\n";
        exitWithError(errOut.str(),std::cerr, P->inOut->logMain, EXIT_CODE_FILE_OPEN, *P);
    };    
};
