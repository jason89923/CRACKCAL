#ifndef MissionManagerPage
#define MissionManagerPage

#include <windows.h>

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
        } // else
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

   private:
    IO io;
    static bool processing;

    void run() {
        try {
            char* text = NULL;
            io >> text;
            io << text;
        }  // try
        catch (exception& e) {
            MessageBox(NULL, strcat((char*)e.what(), "\n\rPlease restart the program to fix the problem"), "Error", MB_OK | MB_ICONERROR);
        }  // catch
    }
};

bool MissionManager::processing = false;

#endif