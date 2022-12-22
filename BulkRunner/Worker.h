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
    //BulkThreadResults threadResults;
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
        Worker(std::stringstream& resultStream, int& readers, int& writers, int& seconds, std::string& clientPath);

	private:

        void CreateChildProcess();
        void ReadFromPipe(std::stringstream& resultStream);
        void ErrorExit(PTSTR);
        


		LPCSTR filePath = "C:/Users/Nate/Desktop/CapsTesting/ReferenceTCPClientv3.0.exe";//"C:/Users/Nate/Desktop/CapsTesting/ProtocolVerifier.exe");//"C:/Users/Nate/Documents/GitHub/CapsClient/x64/Debug/TCPClient.exe");//"C:/Users/Nate/Documents/GitHub/Caps/x64/Release/TCPServerMutithreaded.exe");
		LPCTSTR createFilePath = TEXT("C:/Users/Nate/Desktop/test.txt");
        //LPWSTR createFilePath = 'C:/Users/Nate/Desktop/test.txt';

		HANDLE g_hChildStd_IN_Rd = NULL;
		HANDLE g_hChildStd_IN_Wr = NULL;
		HANDLE g_hChildStd_OUT_Rd = NULL;
		HANDLE g_hChildStd_OUT_Wr = NULL;

};