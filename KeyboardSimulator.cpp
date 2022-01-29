#include <windows.h>

#include <map>

#include "ClipboardGetter.cpp"

class KeyInfo {
   public:
    int value;
    bool isHotkey;
    KeyInfo(int value, bool isHotkey) {
        this->value = value;
        this->isHotkey = isHotkey;
    }
};

class keyMap : public map<char, KeyInfo*> {
   public:
    keyMap() {
        for (char i = 'A'; i <= 'Z'; i++) {
            this->operator[]((char)i) = new KeyInfo(i, true);
            this->operator[]((char)(i + 32)) = new KeyInfo(i, false);
        }

        for (char i = '0'; i <= '9'; i++) {
            this->operator[]((char)i) = new KeyInfo(i, false);
        }

        char temp[] = "!@#$%^&*(";
        for (int i = 0; temp[i] != '\0'; i++) {
            this->operator[](temp[i]) = new KeyInfo(i + 49, true);
        }
        this->operator[](')') = new KeyInfo(48, true);

        this->operator[](';') = new KeyInfo(186, false);
        this->operator[](':') = new KeyInfo(186, true);

        this->operator[]('=') = new KeyInfo(187, false);
        this->operator[]('+') = new KeyInfo(187, true);

        this->operator[](',') = new KeyInfo(188, false);
        this->operator[]('<') = new KeyInfo(188, true);

        this->operator[]('-') = new KeyInfo(189, false);
        this->operator[]('_') = new KeyInfo(189, true);

        this->operator[]('.') = new KeyInfo(190, false);
        this->operator[]('>') = new KeyInfo(190, true);

        this->operator[]('/') = new KeyInfo(191, false);
        this->operator[]('?') = new KeyInfo(191, true);

        this->operator[]('`') = new KeyInfo(192, false);
        this->operator[]('~') = new KeyInfo(192, true);

        this->operator[]('[') = new KeyInfo(219, false);
        this->operator[]('{') = new KeyInfo(219, true);

        this->operator[]('\\') = new KeyInfo(220, false);
        this->operator[]('|') = new KeyInfo(220, true);

        this->operator[](']') = new KeyInfo(221, false);
        this->operator[]('}') = new KeyInfo(221, true);

        this->operator[]('\'') = new KeyInfo(222, false);
        this->operator[]('"') = new KeyInfo(222, true);

        this->operator[](' ') = new KeyInfo(32, false);
        this->operator[]('\t') = new KeyInfo(9, false);
        this->operator[]('\n') = new KeyInfo(13, false);
    }
};

class KeyboardSimulator {
   private:
    keyMap keyMap;

   public:
    void input(const char* text) {
        for (int i = 0; text[i] != '\0'; i++) {
            input(text[i]);
            Sleep(1);
        }
    }

    void input(const char& ch) {
        keyMap::iterator iter = keyMap.find(ch);
        if (iter != keyMap.end()) {
            if (iter->second->isHotkey) {
                hotKey(iter->second->value);
            }  // if
            else {
                keyPress(iter->second->value);
            }  // else
        }      // if
    }

   private:
    void keyDown(int vk) {
        keybd_event(vk, 0, 0, 0);
    }

    void keyUp(int vk) {
        keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
    }

    void keyPress(int vk) {
        keyDown(vk);
        Sleep(1);
        keyUp(vk);
    }

    void hotKey(int vk) {
        keyDown(16);
        Sleep(1);
        keyPress(vk);
        Sleep(1);
        keyUp(16);
    }
};

void SetKeyBoard(int num) {
    HWND hCon = GetConsoleWindow();
    DWORD dwNewKeybLayout = num;  // Layout must be already loaded!
    PostMessage(hCon, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)dwNewKeybLayout);
    PostMessage(hCon, WM_INPUTLANGCHANGE, 0, (LPARAM)dwNewKeybLayout);
}  // SetKeyBoard

