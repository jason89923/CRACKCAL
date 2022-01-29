#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <exception>
#include <iostream>
#include <stdexcept>
using namespace std;

class INITClipboard {
   public:
    INITClipboard() {
        if (!OpenClipboard(nullptr))
            throw runtime_error("Can't open clipboard.");
    }

    ~INITClipboard() {
        CloseClipboard();
    }

    // Ban copy
   private:
    INITClipboard(const INITClipboard&);
    INITClipboard& operator=(const INITClipboard&);
};

class INITTextGlobalLock {
   public:
    INITTextGlobalLock(HANDLE& hData) : m_hData(hData) {
        str = static_cast<const char*>(GlobalLock(m_hData));
        if (!str)
            throw runtime_error("Can't acquire lock on clipboard text.");
    }

    ~INITTextGlobalLock() {
        GlobalUnlock(m_hData);
    }

    const char* Get() const {
        return str;
    }

   private:
    HANDLE m_hData;
    const char* str;
};

class ClipboardGetter {
   public:
    ClipboardGetter() {
        INITClipboard clipboard;

        HANDLE hData = GetClipboardData(CF_TEXT);
        if (hData == nullptr)
            throw runtime_error("Can't get clipboard text.");

        INITTextGlobalLock textGlobalLock = hData;
        text = textGlobalLock.Get();
    }

    const char* getText() const {
        return text;
    }

   private:
    const char* text;
};