#include "KeyboardSimulator.cpp"


int main() {
    KeyboardSimulator keyboardSimulator;
    ClipboardGetter clipboardGetter;
    Sleep(3000);
    keyboardSimulator.input(clipboardGetter.getText());
}