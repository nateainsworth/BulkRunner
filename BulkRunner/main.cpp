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

std::string threadHeaders(int& readers, int& writers);
std::vector<std::string> printThreadResults(int& readers, int& writers, int& seconds, BulkResults& testResult);

LPCTSTR serverPath = TEXT("C:/Users/Nate/Documents/GitHub/Caps/x64/Release/TCPServerMutithreaded.exe");



void startup(LPCTSTR lpApplicationName);

int main(int argc, char* argv[])
{



    std::cout << argv[0];
    
    int readers = 5;
    int writers = 5;
    int seconds = 10;
    int iterations = 10;

    
    std::string combo = "rClient_rServer";


    for (unsigned int it = 0; it < iterations; it++) {
        startup(serverPath);

        std::stringstream resultStream;
        Worker worker(resultStream, readers, writers, seconds);


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
        std::string fileName = "C:/Users/Nate/Desktop/" + combo + "_r" + std::to_string(readers) + "_w" + std::to_string(writers) + ".csv";
        csvFile.open(fileName, std::ios::app);
        csvFile << "Readers:," << readers << ",\n";
        csvFile << "Writers:," << writers << ",\n";
        csvFile << ",,\n";
        csvFile << "Run" << std::to_string(it + 1 ) << " / " << std::to_string(iterations) << ",,\n";



        //csvFile << tableHeader;
        csvFile << threadHeaders(readers, writers) << ",\n";
        //std::string* tableHeader = new std::string[seconds];
    
        std::vector<std::string> threadResults = printThreadResults(readers, writers, seconds, testResult);


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


    return 0;
}



std::string threadHeaders(int& readers, int& writers) {
    std::string tableHeader = ",";

    for (unsigned int w = 0; w < writers; w++) {

        tableHeader += "POST " + std::to_string(w) + ",";
    }

    for (unsigned int r = 0; r < readers; r++) {
        tableHeader += "READ " + std::to_string(r) + ",";
    }

    return tableHeader;
}

std::vector<std::string> printThreadResults(int& readers, int& writers, int& seconds, BulkResults& testResult) {
    std::vector<std::string> threadResults(seconds);
    for (unsigned int s = 0; s < seconds; s++) {
        threadResults[s] = "Second " + std::to_string(s) + ",";

        for (unsigned int w = 0; w < writers; w++) {
            threadResults[s] += std::to_string(testResult.threadResults["reader"][w].requests[s]) + ",";

        }

        for (unsigned int r = 0; r < readers; r++) {
            threadResults[s] += std::to_string(testResult.threadResults["reader"][r].requests[s]) + ",";
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
