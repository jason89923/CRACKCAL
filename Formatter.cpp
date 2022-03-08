#ifndef FormatterPage
#define FormatterPage

#include <stdio.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <time.h>

#include "IO.cpp"

using namespace std;

class Delimiter : public map<string, int> {
   public:
    bool contains(char& str) {
        Delimiter::iterator iter;
        string temp;
        temp.push_back(str);
        iter = this->find(temp);
        if (iter != this->end()) {
            return true;
        }

        return false;
    }

    bool contains(string& str) {
        Delimiter::iterator iter;
        iter = this->find(str);
        if (iter != this->end()) {
            return true;
        }

        return false;
    }

   protected:
    void build(vector<string>& delimiters) {
        for (int i = 0; i < delimiters.size(); i++) {
            map::operator[](delimiters[i]) = i;
        }
    }
};

class SingleDelimiter : public Delimiter {
   public:
    SingleDelimiter() {
        vector<string> delimiters = {"{", "}", "[", "]", "#", "(", ")", "<", ">", ":", ";", ".", "*", "+", "-", "*", "/", "%", "^", "&", "|", "~", "!", "=", ",", "\'", "\""};
        Delimiter::build(delimiters);
    }
};

class DoubleDelimiter : public Delimiter {
   public:
    DoubleDelimiter() {
        vector<string> delimiters = {"+=", "-=", "*=", "/=", "%=", "^=", "&=", "|=", "<<", ">>", "==", "!=", "<=", ">=", "&&", "||", "++", "--", "->"};
        Delimiter::build(delimiters);
    }
};

class Formatter {
   private:
    static SingleDelimiter singleDelimiter;
    static DoubleDelimiter doubleDelimiter;

    void getToken(char* text, vector<string>& tokenList) {
        tokenList.clear();
        string buffer;
        bool isReadingString = false, isReadingChar = false, escapeCharacter = false;
        for (int i = 0; text[i] != '\0'; i++) {
            if (!isReadingChar && !isReadingString && (text[i] == ' ' || text[i] == '\n' || text[i] == '\t' || text[i] == '\r')) {
                if (buffer.length() > 0) {
                    tokenList.push_back(buffer);
                    buffer.clear();
                }
            } else if (!isReadingChar && !isReadingString && singleDelimiter.contains(text[i])) {
                if (buffer.length() > 0) {
                    tokenList.push_back(buffer);
                    buffer.clear();
                }

                if (text[i] == '\"') {
                    isReadingString = true;
                    buffer.push_back(text[i]);
                } else if (text[i] == '\'') {
                    isReadingChar = true;
                    buffer.push_back(text[i]);
                } else {
                    string temp;
                    temp.push_back(text[i]);
                    tokenList.push_back(temp);
                }
            } else {
                buffer.push_back(text[i]);
                if (escapeCharacter) {
                    escapeCharacter = false;
                } else {
                    if (text[i] == '\"') {
                        if (isReadingString) {
                            tokenList.push_back(buffer);
                            buffer.clear();
                        }

                        isReadingString = false;
                    } else if (text[i] == '\'') {
                        if (isReadingChar) {
                            tokenList.push_back(buffer);
                            buffer.clear();
                        }

                        isReadingChar = false;
                    }

                    if ((isReadingString || isReadingChar) && text[i] == '\\') {
                        escapeCharacter = true;
                    }
                }
            }
        }

        if (buffer.size() > 0) {
            tokenList.push_back(buffer);
        }
    }

    void mergeToken(vector<string>& tokenList) {
        for (int i = 0; i < tokenList.size() - 1; i++) {
            string currentCombination = tokenList[i] + tokenList[i + 1];
            if (doubleDelimiter.contains(currentCombination)) {
                tokenList[i] = currentCombination;
                tokenList.erase(tokenList.begin() + i + 1);
            }
        }
    }

   public:
    Formatter(char* text) {
        time_t t = clock();
        vector<string> tokenList;
        this->getToken(text, tokenList);
        this->mergeToken(tokenList);
        for (int i = 0; i < tokenList.size(); i++) {
            cout << tokenList[i] << endl;
        }

        cout << clock() - t << endl;
    }
};

SingleDelimiter Formatter::singleDelimiter;
DoubleDelimiter Formatter::doubleDelimiter;

IO io;

int main() {
    char* text;
    io >> text;
    Formatter formatter = text;
    return 0;
}
#endif