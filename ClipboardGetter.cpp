#ifndef ClipboardGetterPage
#define ClipboardGetterPage
#include <string.h>
#include <windows.h>

#include <stdexcept>
using namespace std;

class INITClipboard {
   public:
    INITClipboard() {
        if (!OpenClipboard(NULL))
            throw runtime_error("Can't open clipboard.");
    }

    ~INITClipboard() {
        CloseClipboard();
    }

    // Ban copy
   private:
    INITClipboard(const INITClipboard &);
    INITClipboard &operator=(const INITClipboard &);
};

class INITTextGlobalLock {
   public:
    INITTextGlobalLock(HANDLE &hData) : m_hData(hData) {
        const char *string = static_cast<const char *>(GlobalLock(m_hData));
        if (!string)
            throw runtime_error("Can't acquire lock on clipboard text.");
        else
            str = new char[strlen(string) + 1];
        strcpy((char *)str, string);
    }

    ~INITTextGlobalLock() {
        GlobalUnlock(m_hData);
        GlobalFree(m_hData);
    }

    const char *get() const {
        return str;
    }

   private:
    HANDLE m_hData;
    const char *str;
};

class ClipboardGetter {
   public:
    ClipboardGetter() {
        INITClipboard clipboard;

        HANDLE hData = GetClipboardData(CF_TEXT);
        if (hData == NULL)
            throw runtime_error("Can't get clipboard text.");

        INITTextGlobalLock textGlobalLock = hData;
        text = textGlobalLock.get();
    }

    const char *getText() const {
        return text;
    }

   private:
    const char *text;
};

class ClipboardSetter {
   public:
    ClipboardSetter(string &str) {
        INITClipboard clipboard;

        HGLOBAL hMemory;
        LPTSTR lpMemory;

        if (!EmptyClipboard()) {
            throw runtime_error("fail to clear clipboard");
        }

        if ((hMemory = GlobalAlloc(GMEM_MOVEABLE, str.length())) == NULL) {
            throw runtime_error("Can't acquire lock on clipboard text.");
        }

        if ((lpMemory = (LPTSTR)GlobalLock(hMemory)) == NULL) {
            throw runtime_error("Can't acquire lock on clipboard text.");
        }

        memcpy_s(lpMemory, str.length(), str.c_str(), str.length());

        GlobalUnlock(hMemory);

        if (SetClipboardData(CF_TEXT, hMemory) == NULL) {
            throw runtime_error("fail to clear clipboard");
        }
    }
};
#endif