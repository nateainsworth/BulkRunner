#include "worker.h"

LPSTR args;

Worker::Worker(std::stringstream& resultStream, unsigned int& readers, unsigned int& writers,unsigned int& seconds, std::string& clientPath, std::string& exportName){
    int test = 5;

    filePath = clientPath.c_str();
    std::string cmdLine = clientPath + " 127.0.0.1 " + std::to_string(writers) + " " + std::to_string(readers) + " " + std::to_string(seconds) + " " + " 0 ";
    exportTxtFile = exportName + "_r" + std::to_string(readers) + "_w" + std::to_string(writers) + ".txt";
    //createFilePath 
    args = const_cast<char*>(cmdLine.c_str());

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


    // Create the child process. 

    CreateChildProcess();


    // Read from pipe that is the standard output for child process. 

    printf("\n->Contents of child process STDOUT:\n\n");

    ReadFromPipe( resultStream);

    printf("\n->End of parent execution.\n");

    

    // The remaining open handles are cleaned up when this process terminates. 
    // To avoid resource leaks in a larger application, close handles explicitly. 


}


void Worker::CreateChildProcess()
// Create a child process that uses the previously created pipes for STDIN and STDOUT.
{
    TCHAR szCmdline[] = TEXT("child");
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA siStartInfo;
    BOOL bSuccess = FALSE;

    // Set up members of the PROCESS_INFORMATION structure. 

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure. 
    // This structure specifies the STDIN and STDOUT handles for redirection.

    ZeroMemory(&siStartInfo, sizeof(STARTUPINFOA));
    siStartInfo.cb = sizeof(STARTUPINFOA);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput = g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;


    bSuccess = CreateProcessA(
        filePath,   // the path
        args,//NULL,//argv[1],        // Command line
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

void Worker::ReadFromPipe(std::stringstream& resultStream) {

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
        //int endLinePosition = writeLine.find_last_of('\n');
        //if (endLinePosition != -1) {
        //    writeLine[(endLinePosition + 1)] = '\0';
        //}
        //bSuccess = WriteFile(hParentStdOut, chBuf, dwRead, &dwWritten, NULL);
        std::ofstream myfile;
        myfile.open(exportTxtFile, std::ios::app);
        myfile << writeLine.substr(0, dwRead + 1);
        myfile.close();

        resultStream << writeLine.substr(0, dwRead + 1);

        bSuccess = WriteFile(hParentStdOut, chBuf,
            dwRead, &dwWritten, NULL);
        if (!bSuccess) break;
    }



}

void Worker::ErrorExit(PTSTR lpszFunction)

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