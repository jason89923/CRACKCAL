#ifndef MissionManagerPage
#define MissionManagerPage

#include <windows.h>

#include "Formatter.cpp"
#include "IO.cpp"

class MissionManager {
   public:
    MissionManager() {
        while (true) {
            Sleep(10);
            if (processing) {
                run();
                processing = false;
            }  // if
        }      // while
    }

    static void start() {
        if (!processing) {
            processing = true;
        }  // if
        else {
            pause();
        }  // else
    }

    static void stop() {
        if (processing && !IO::force_stop) {
            IO::force_stop = true;
        }  // if
    }

    static void pause() {
        if (processing) {
            IO::pause = !IO::pause;
        }  // if
    }

    static void formatAndWriteIntoClipboard() {
        string result = format();
        if (result != "") {
            io << result;
        }
    }

   private:
    static IO io;
    static bool processing;

    static string format() {
        char* text = NULL;
        try {
            io >> text;
            Formatter formatter = text;
            return formatter.str();
        } catch (const exception& e) {
            const char* msg = e.what();
            char errorMsg[500];
            strcpy(errorMsg, msg);
            //MessageBox(NULL, strcat(errorMsg, "\n\rPlease recopy to fix the problem"), "Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
        }

        if (text != NULL) {
            delete[] text;
        }

        return "";
    }

    void run() {
        try {
            string result = format();
            if (result != "") {
                io << result.c_str();
            }
        } catch (const exception& e) {
            
        }
    }
};

bool MissionManager::processing = false;
IO MissionManager::io;

#endif
