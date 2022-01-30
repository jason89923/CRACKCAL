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
    static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION) {
            if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP)) {
                if (((PKBDLLHOOKSTRUCT)lParam)->vkCode == 121) {
                    if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN)) {
                        MissionManager::start();
                    }  // if
                }      // if
                else if (((PKBDLLHOOKSTRUCT)lParam)->vkCode == 122) {
                    if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN)) {
                        MissionManager::pause();
                    }  // if
                }      // else if
                else if (((PKBDLLHOOKSTRUCT)lParam)->vkCode == 123) {
                    if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN)) {
                        MissionManager::stop();
                    }  // if
                }      // else if
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

#endif