#include <windows.h> 
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <sstream>
#include "worker.h"

std::vector<float> getDataFromLine(std::string line);

std::string threadHeaders(unsigned int& readers,unsigned int& writers);
std::vector<std::string> printThreadResults(unsigned int& readers,unsigned int& writers,unsigned int& seconds, BulkResults& testResult);

LPCTSTR serverPath = TEXT("C:/Users/c0042245/Downloads/TCPServerMutithreaded.exe");///ReferenceTCPServerv2.0.exe");//



void startup(LPCTSTR lpApplicationName);

int main(int argc, char* argv[])
{

    /*
        1: myClient : myServer
        2: refClient : refServer
        3: refClient : myServer
        4: myClient : refServer
    */

    std::string selectedServer = "";
    std::string selectedClient = "";
    std::string exportName = "";


    std::string runnerPath = "C:/Users/c0042245/Downloads";//"C:/Users/c0042245/OneDrive - Sheffield Hallam University/VSCode/BulkRunner/x64/Release";//argv[0];
    //int position = runnerPath.find_last_of("\\");
    std::string dirPath = runnerPath;//runnerPath.substr(0, position);

    switch (atoi(argv[1])) {
    case 1:
        selectedServer = dirPath + "/TCPServerMutithreaded.exe";
        selectedClient = dirPath + "/TCPClient.exe";
        exportName = dirPath + "/results/myClient_myServer";
        break;
    case 2:
        selectedServer = dirPath + "/ReferenceTCPServerv2.0.exe";
        selectedClient = dirPath + "/ReferenceTCPClientv3.0.exe";
        exportName = dirPath + "/results/rClient_rServer";
        break;
    case 3:
        selectedServer = dirPath + "/TCPServerMutithreaded.exe";
        selectedClient = dirPath + "/ReferenceTCPClientv3.0.exe";
        exportName = dirPath + "/results/rClient_myServer";
        break;
    case 4:
        selectedServer = dirPath + "/ReferenceTCPServerv2.0.exe";
        selectedClient = dirPath + "/TCPClient.exe";
        exportName = dirPath + "/results/myClient_rServer";
        break;
    default:
        std::cout << "No server or client selected";
    }
    

    std::vector<TCHAR> convertedPath(selectedServer.begin(), selectedServer.end());
    //serverPath = &convertedPath[0];
    
    std::cout << "Server Path: \n" << serverPath << std::endl;

    unsigned int readers = 5;
    unsigned int writers = 5;
    unsigned int seconds = 10;
    unsigned int iterations = 10;



    

    for (unsigned int reader = 1; reader < readers + 1; reader++) {
        for (unsigned int writer = 1; writer < writers + 1; writer++) {
        
            for (unsigned int it = 0; it < iterations; it++) {
                startup(serverPath);

                std::stringstream resultStream;
                Worker worker(resultStream, reader, writer, seconds, selectedClient, exportName);


                BulkResults testResult;

                std::string threadType;
                int threadNumber = 0;
                std::string line;
                while (std::getline(resultStream, line)) {


                    if (int found = line.find("POST thread") != -1) {
                        std::vector<float> intData = getDataFromLine(line);
                        threadNumber = intData[0];
                        threadType = "writer";
                        continue;

                    }

                    if (int found = line.find("READ thread") != -1) {
                        std::vector<float> intData = getDataFromLine(line);
                        threadNumber = intData[0];
                        threadType = "reader";

                        continue;

                    }

                    if (int found = line.find("Second") != -1) {
                        std::vector<float> intData = getDataFromLine(line);

                        BulkThreadResults results;
                        testResult.threadResults[threadType][threadNumber].requests.push_back(intData[1]);//.push_back(intData[0]);


                        continue;

                    }

                    if (int found = line.find("Average:") != -1) {
                        std::vector<float> intData = getDataFromLine(line);

                        testResult.threadResults[threadType][threadNumber].averages.push_back(intData[0]);//.requests.push_back(intData[1]);
                        continue;
                    }

                    if (int found = line.find("Runtime:") != -1) {
                        std::vector<float> intData = getDataFromLine(line);

                        testResult.threadResults[threadType][threadNumber].runtimes.push_back(intData[0]);
                        continue;
                    }

                    if (int found = line.find("Total poster requests:") != -1) {
                        std::vector<float> intData = getDataFromLine(line);

                        testResult.totalPoster = intData[0];
                        continue;
                    }

                    if (int found = line.find("Average requests per poster thread:") != -1) {
                        std::vector<float> intData = getDataFromLine(line);

                        testResult.averagePosterReqs = intData[0];
                        continue;
                    }

                    if (int found = line.find("Total reader requests:") != -1) {
                        std::vector<float> intData = getDataFromLine(line);
                        testResult.totalReader = intData[0];
                        continue;
                    }

                    if (int found = line.find("Average requests per reader thread:") != -1) {
                        std::vector<float> intData = getDataFromLine(line);
                        testResult.averageReaderReqs = intData[0];

                        continue;
                    }

                    if (int found = line.find("Total requests:") != -1) {
                        std::vector<float> intData = getDataFromLine(line);
                        testResult.totalRequests = intData[0];
                        continue;
                    }

                    if (int found = line.find("Average requests per thread:") != -1) {
                        std::vector<float> intData = getDataFromLine(line);
                        testResult.averagePerThread = intData[0];
                        continue;
                    }

                    //TODO CHANGE TO FLOAT LESS COSTLY THAN DOUBLE 
                    if (int found = line.find("Average requests per thread per second:") != -1) {
                        std::vector<float> intData = getDataFromLine(line);
                        testResult.averagePerSecond = intData[0];
                        continue;
                    }


                }
                std::cout << "Done extracting results";

                std::ofstream csvFile;
                std::string fileName = exportName + "_r" + std::to_string(reader) + "_w" + std::to_string(writer) + ".csv";
                csvFile.open(fileName, std::ios::app);
                csvFile << "Readers:," << reader << ",\n";
                csvFile << "Writers:," << writer << ",\n";
                csvFile << ",,\n";
                csvFile << "Run " << std::to_string(it + 1 ) << " / " << std::to_string(iterations) << ",,\n";



                //csvFile << tableHeader;
                csvFile << threadHeaders(reader, writer) << ",\n";
                //std::string* tableHeader = new std::string[seconds];
    
                std::vector<std::string> threadResults = printThreadResults(reader, writer, seconds, testResult);


                for (unsigned int i = 0; i < threadResults.size(); i++) {
                    csvFile << threadResults[i] << "\n";
                };


                csvFile << ",,\n";
                csvFile << ",,\n";
                csvFile << "Total poster requests:,"<< testResult.totalPoster << ",\n";
                csvFile << "Average requests per poster thread:,"<< testResult.averagePosterReqs << ",\n";
                csvFile << "Total reader requests:,"<< testResult.totalReader << ",\n";
                csvFile << "Average requests per reader thread:,"<< testResult.averageReaderReqs << ",\n";
                csvFile << "Total requests:,"<< testResult.totalRequests << ",\n";
                csvFile << "Average requests per thread:," << testResult.averagePerThread << ",\n";
                csvFile << "Average requests per thread per second:,"<< testResult.averagePerSecond << ",\n";

                csvFile << ",,\n";
                csvFile << ",,\n";
                csvFile << ",,\n";

                csvFile.close();
            };
        }
    }


    return 0;
}



std::string threadHeaders(unsigned int& readers, unsigned int& writers) {
    std::string tableHeader = ",";

    for (unsigned int w = 0; w < writers; w++) {

        tableHeader += "POST " + std::to_string(w) + ",";
    }

    for (unsigned int r = 0; r < readers; r++) {
        tableHeader += "READ " + std::to_string(r) + ",";
    }

    return tableHeader;
}

std::vector<std::string> printThreadResults(unsigned int& readers, unsigned int& writers, unsigned int& seconds, BulkResults& testResult) {
    std::vector<std::string> threadResults(seconds );
    threadResults.push_back("Average,");
    threadResults.push_back("Runtime,");
    for (unsigned int s = 0; s < seconds; s++) {
        threadResults[s] = "Second " + std::to_string(s) + ",";
        

        for (unsigned int w = 0; w < writers; w++) {
            threadResults[s] += std::to_string(testResult.threadResults["writer"][w].requests[s]) + ",";
            if (s == 0) {
                threadResults[seconds] += std::to_string(testResult.threadResults["writer"][w].averages[0]) + ",";
                threadResults[seconds + 1] += std::to_string(testResult.threadResults["writer"][w].runtimes[0]) + ",";
            }

        }

        for (unsigned int r = 0; r < readers; r++) {
            threadResults[s] += std::to_string(testResult.threadResults["reader"][r].requests[s]) + ",";
            if (s == 0) {
                threadResults[seconds] += std::to_string(testResult.threadResults["reader"][r].averages[0]) + ",";
                threadResults[seconds + 1] += std::to_string(testResult.threadResults["reader"][r].runtimes[0]) + ",";
            }
        }
    }
    return threadResults;

}


//get integer value from string
std::vector<float> getDataFromLine(std::string line) {
    std::stringstream wordStream;
    std::vector<float> intValues;
    wordStream << line;
    std::string word;
    int testConvert;

    // loop through each individual word
    while (!wordStream.eof()) {
        wordStream >> word;
        // check if word can convert to int 
        //TODO CHANGE TO FLOAT LESS COSTLY THAN DOUBLE 
        if (std::stringstream(word) >> testConvert) {
            intValues.push_back(testConvert);
        }
        //reset
        word = "";
    }
    return intValues;
}


// startup function taken from (Jona, 2013) https://stackoverflow.com/questions/15435994/how-do-i-open-an-exe-from-another-c-exe
void startup(LPCTSTR lpApplicationName)
{
    // additional information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // start the program up
    CreateProcess(
        lpApplicationName,   // the path
        NULL,//argv[1],        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE,//0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    );
    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
