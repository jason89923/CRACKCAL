#ifndef KeyboardListenerPage

#define KeyboardListenerPage

#include <Windows.h>
#include <stdio.h>

class KeyboardListener {
   public:
    KeyboardListener() {
        CreateThread(NULL, 0, ListenerThread, NULL, 0, NULL);
    }

   private:
    static bool hotKey; 

    static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION) {
            if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN)) {
                if (((PKBDLLHOOKSTRUCT)lParam)->vkCode == 121) {
                    MissionManager::start();
                    return 1;
                }  // if
                else if (((PKBDLLHOOKSTRUCT)lParam)->vkCode == 123) {
                    MissionManager::stop();
                    return 1;
                }  // else if
                else if (((PKBDLLHOOKSTRUCT)lParam)->vkCode == 162) {
                    hotKey = true;
                }  // else if
                else if (((PKBDLLHOOKSTRUCT)lParam)->vkCode == 86) {
                    if (hotKey) {

                    MissionManager::formatAndWriteIntoClipboard();
                }  // else if
                    }
            }
            else if ((wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP)) {
                if (((PKBDLLHOOKSTRUCT)lParam)->vkCode == 162) {
                    hotKey = false;
                }  // else if
            }
        }
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    static DWORD WINAPI ListenerThread(LPVOID lpParam) {
        HHOOK hhkLowLevelKeyboard = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProc, 0, 0);

        MSG msg;
        while (!GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }  // while

        UnhookWindowsHookEx(hhkLowLevelKeyboard);
        return 0;
    }
};

bool KeyboardListener::hotKey = false;

#endif