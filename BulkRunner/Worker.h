#pragma once
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>
#include <string>

#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <sstream>

#include <regex>
#include <unordered_map>
#define BUFSIZE 4096 


struct BulkThreadResults
{
    std::vector<int> requests;
    std::vector<float> runtimes;
    std::vector<float> averages;

};

struct BulkResults
{

    std::unordered_map<std::string, std::unordered_map<int, BulkThreadResults>> threadResults;
    int totalPoster;
    float averagePosterReqs;
    int totalReader;
    float averageReaderReqs;

    int totalRequests;
    float averagePerThread;
    float averagePerSecond;
};


class Worker {
	public:
        Worker(std::stringstream& resultStream,unsigned int& readers,unsigned int& writers,unsigned int& seconds, std::string& clientPath, std::string& exportName);

	private:

        void CreateChildProcess();
        void ReadFromPipe(std::stringstream& resultStream);
        void ErrorExit(PTSTR);
        
        std::string exportTxtFile;


		LPCSTR filePath = "";
		LPCTSTR createFilePath = TEXT("");


		HANDLE g_hChildStd_IN_Rd = NULL;
		HANDLE g_hChildStd_IN_Wr = NULL;
		HANDLE g_hChildStd_OUT_Rd = NULL;
		HANDLE g_hChildStd_OUT_Wr = NULL;

};