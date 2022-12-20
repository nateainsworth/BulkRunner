#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>
#include <string>

#include <iostream>
#include <thread>
#include <vector>
#include <fstream>

#define BUFSIZE 4096 

LPCTSTR filePath = TEXT("C:/Users/Nate/Desktop/CapsTesting/ReferenceTCPClientv3.0.exe");//"C:/Users/Nate/Desktop/CapsTesting/ProtocolVerifier.exe");//"C:/Users/Nate/Documents/GitHub/CapsClient/x64/Debug/TCPClient.exe");//"C:/Users/Nate/Documents/GitHub/Caps/x64/Release/TCPServerMutithreaded.exe");
LPCTSTR createFilePath = TEXT("C:/Users/Nate/Desktop/test.txt");

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;

//HANDLE g_hInputFile = NULL;

void CreateChildProcess(void);
void WriteToPipe(void);
void ReadFromPipe(void);
void ErrorExit(PTSTR);

//LPCTSTR args = TEXT("127.0.0.1");
//LPWSTR args = const_cast<LPTSTR>(TEXT("127.0.0.1")); //["127.0.0.1"];
//std::basic_string<TCHAR> tstring[1] = "127.0.0.1";
wchar_t cmdArgs[] = L"C:/Users/Nate/Desktop/CapsTesting/ReferenceTCPClientv3.0.exe 127.0.0.1 5 5 10 0";//"C:/Users/Nate/Desktop/CapsTesting/ProtocolVerifier.exe 127.0.0.1";

int _tmain(int argc, TCHAR* argv[1])
{
 

    SECURITY_ATTRIBUTES saAttr;

    printf("\n->Start of parent execution.\n");

    // Set the bInheritHandle flag so pipe handles are inherited. 

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT. 

    if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)) {
        TCHAR sztext[] = TEXT("StdoutRd CreatePipe");
        ErrorExit(sztext);
    }

    // Ensure the read handle to the pipe for STDOUT is not inherited.

    if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
        TCHAR sztext[] = TEXT("Stdout SetHandleInformation");
        ErrorExit(sztext);
    }

    // Create a pipe for the child process's STDIN. 
    /*
    if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) {
        TCHAR sztext[] = TEXT("Stdin CreatePipe");
        ErrorExit(sztext);
    }

    // Ensure the write handle to the pipe for STDIN is not inherited. 

    if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) {
        TCHAR sztext[] = TEXT("Stdin SetHandleInformation");
        ErrorExit(sztext);
    }*/

    // Create the child process. 

    CreateChildProcess();

    // Get a handle to an input file for the parent. 
    // This example assumes a plain text file and uses string output to verify data flow. 
    /*
    if (argc == 1) {
        TCHAR sztext[] = TEXT("Please specify an input file.\n");
        ErrorExit(sztext);
    }*/
    /*
    g_hInputFile = CreateFile(
        createFilePath,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_READONLY,
        NULL);

    if (g_hInputFile == INVALID_HANDLE_VALUE)
    {
        TCHAR sztext[] = TEXT("CreateFile");
        ErrorExit(sztext);
    }

    // Write to the pipe that is the standard input for a child process. 
    // Data is written to the pipe's buffers, so it is not necessary to wait
    // until the child process is running before writing data.

    WriteToPipe();
    printf("\n->Contents of %S written to child STDIN pipe.\n", argv[1]);
    */

    // Read from pipe that is the standard output for child process. 

    printf("\n->Contents of child process STDOUT:\n\n");

        ReadFromPipe();
   
    printf("\n->End of parent execution.\n");

    // The remaining open handles are cleaned up when this process terminates. 
    // To avoid resource leaks in a larger application, close handles explicitly. 

    return 0;
}

void CreateChildProcess()
// Create a child process that uses the previously created pipes for STDIN and STDOUT.
{
    TCHAR szCmdline[] = TEXT("child");
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    BOOL bSuccess = FALSE;

    // Set up members of the PROCESS_INFORMATION structure. 

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure. 
    // This structure specifies the STDIN and STDOUT handles for redirection.

    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput = g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;


    bSuccess = CreateProcess(
        filePath,   // the path
        cmdArgs,//NULL,//argv[1],        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        TRUE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &siStartInfo,            // Pointer to STARTUPINFO structure
        &piProcInfo             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    );

    // If an error occurs, exit the application. 
    if (!bSuccess)
    {
        TCHAR sztext[] = TEXT("CreateProcess");
        ErrorExit(sztext);
    }
    else
    {
        // Close handles to the child process and its primary thread.
        // Some applications might keep these handles to monitor the status
        // of the child process, for example. 

        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);

        // Close handles to the stdin and stdout pipes no longer needed by the child process.
        // If they are not explicitly closed, there is no way to recognize that the child process has ended.

        CloseHandle(g_hChildStd_OUT_Wr);
        CloseHandle(g_hChildStd_IN_Rd);
    }
}
/*
void WriteToPipe(void)

// Read from a file and write its contents to the pipe for the child's STDIN.
// Stop when there is no more data. 
{
    DWORD dwRead, dwWritten;
    std::string chBuf[BUFSIZE];
    BOOL bSuccess = FALSE;

    for (;;)
    {
        bSuccess = ReadFile(g_hInputFile, chBuf, BUFSIZE, &dwRead, NULL);
        if (!bSuccess || dwRead == 0) break;

        bSuccess = WriteFile(g_hChildStd_IN_Wr, chBuf, dwRead, &dwWritten, NULL);
        if (!bSuccess) break;
    }

    // Close the pipe handle so the child process stops reading. 

    if (!CloseHandle(g_hChildStd_IN_Wr))
    {
        TCHAR sztext[] = TEXT("StdInWr CloseHandle");
        ErrorExit(sztext);
    }
}
*/
void ReadFromPipe(void){

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT. 
// Stop when there is no more data. 


    DWORD dwRead, dwWritten;
    CHAR chBuf[BUFSIZE];
    BOOL bSuccess = FALSE;
    HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    for (;;)
    {
        bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
        if (!bSuccess || dwRead == 0) break;
        std::string writeLine = std::string(chBuf);
        int endLinePosition = writeLine.find_last_of('\n');
        //if (endLinePosition != -1) {
        //    writeLine[(endLinePosition + 1)] = '\0';
        //}
        //bSuccess = WriteFile(hParentStdOut, chBuf, dwRead, &dwWritten, NULL);
        std::ofstream myfile;
        myfile.open("C:/Users/Nate/Desktop/test2.txt", std::ios::app);
        myfile << writeLine.substr(0 , endLinePosition + 1);
        myfile.close();

        bSuccess = WriteFile(hParentStdOut, chBuf,
            dwRead, &dwWritten, NULL);
        if (!bSuccess) break;
    }
    

    
}

void ErrorExit(PTSTR lpszFunction)

// Format a readable error message, display a message box, 
// and exit from the application.
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(1);
}