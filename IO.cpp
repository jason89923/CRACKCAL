#ifndef IOPage
#define IOPage

#include <iostream>

using namespace std;

#include "ClipboardGetter.cpp"
#include "KeyboardSimulator.cpp"

class IO {
   public:
    /*從剪貼簿讀取內容*/
    friend IO& operator>>(IO& io, char*& text) {
        ClipboardGetter clipboardGetter;
        text = (char*)clipboardGetter.getText();
        return io;
    }  // operator>>

    /*以模擬鍵盤輸出內容*/
    friend IO& operator<<(IO& io, const char* text) {
        KeyboardSimulator keyboardSimulator;
        keyboardSimulator.input(text, &force_stop, &pause);
        force_stop = false;
        pause = false;
        return io;
    }

   private:
    static bool force_stop;
    static bool pause;
    friend class MissionManager;
};

bool IO::force_stop = false;
bool IO::pause = false;

#endif