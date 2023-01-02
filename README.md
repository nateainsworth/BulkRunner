# Concurrent Programming

# Bulk Runner
This *Section* should help you get started on running the bulk runner application. This application will run through all possible combinations of 1-5 readers and 1-5 writers. With the specified server and client.

## **Table of Contents**
- [**Runing the Application**](#running-the-application)
    - [**Run from .EXE**](#running-from-exe)
    - [**Run from IDE**](#run-from-ide)
- [**References**](#references)


# Running the Application

## Running from .EXE
Before starting this section make sure no client or server is running on 127.0.0.1. This application will automatically open them for us with the correct command line arguments. Make sure that the file path contains all servers and clients you wish to run. Make sure the filepath of the file used excludes spacing or this could cause issues with the URL manipulation (Rename reference client and Reference server to have no space in the file name).

1. Go to the file containing the .EXE within file explorer click into the file `URL` and type `cmd` to open a command prompt at this URL.
2. Click `Tab` on your keyboard until the EXE file name is displayed. After the EXE url you will need to provide one command line argument to define which combination of server and client to run. 
    ```C++ 
    1: myClient : myServer
    2: refClient : refServer
    3: refClient : myServer
    4: myClient : refServer 
    ```
    click `enter` on your keyboard to run. You will notice for each combination


3. Follow the read me file provided by the Client application to connect to the server.

<br />

## Run From IDE
1. Open in visual studio, open the `Project` menu, Click `TCPClient Properties`, select `Debugging` click into the field for `Command Arguments` and provide arguments as follows           
    ```C++ 
    1: myClient : myServer
    2: refClient : refServer
    3: refClient : myServer
    4: myClient : refServer 
    ```

2. Should you wish to change the clients and reference servers you must update the switch case within main to contain the new exe file names.

3. Now you can run the application within visual studio.

<br />

# References
Jona | How do I open an .exe from another C++ .exe?. (2013). StackOverflow. from https://stackoverflow.com/questions/15435994/how-do-i-open-an-exe-from-another-c-exe

Bridge-Microsoft, K., msatranjr, mijacobs, drewbatgit, YellowAfterlife, Kartiku, DCtheGeek, v-kents, GrantMeStrength, cminyard, &amp; vvavrychuk. (2021, February 1). `Creating a child process with redirected input and output` - win32 apps. Win32 apps | Microsoft Learn. from https://learn.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output 