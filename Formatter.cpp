#ifndef FormatterPage
#define FormatterPage

#include <stdio.h>
#include <time.h>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

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
        vector<string> delimiters = {"+=", "-=", "*=", "/=", "%=", "^=", "&=", "|=", "<<", ">>", "==", "!=", "<=", ">=", "&&", "||", "++", "--", "->", "::", "//", "/*", "*/"};
        Delimiter::build(delimiters);
    }
};

class Token {
   public:
    string token;
    Token(string& token) {
        this->token = token;
    }

    Token(const char* token) {
        this->token = token;
    }

    Token& operator=(string& token) {
        this->token = token;
        return *this;
    }

    Token& operator=(const char* token) {
        this->token = string(token);
        return *this;
    }

    bool operator==(string str) {
        return token == str;
    }

    bool operator!=(string str) {
        return token != str;
    }

    friend ostream& operator<<(ostream& out, const Token& token) {
        out << token.token;
        return out;
    }
};

class Formatter {
   private:
    static SingleDelimiter singleDelimiter;
    static DoubleDelimiter doubleDelimiter;

    void getToken(char* text, vector<Token>& tokenList) {
        tokenList.clear();
        string buffer;
        bool isReadingString = false, isReadingChar = false, escapeCharacter = false;
        for (int i = 0; text[i] != '\0'; i++) {
            if (!isReadingChar && !isReadingString && (text[i] == ' ' || text[i] == '\n' || text[i] == '\t' || text[i] == '\r')) {
                if (buffer.length() > 0) {
                    tokenList.push_back(buffer);
                    buffer.clear();
                }

                if (text[i] == '\n' && tokenList[tokenList.size() - 1] != "\n") {
                    tokenList.push_back("\n");
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

    void mergeToken(vector<Token>& tokenList) {
        bool include = false;
        for (int i = 0; i < tokenList.size() - 1; i++) {
            string currentCombination = tokenList[i].token + tokenList[i + 1].token;
            if (doubleDelimiter.contains(currentCombination)) {
                tokenList[i] = currentCombination;
                tokenList.erase(tokenList.begin() + i + 1);
            }

            if (tokenList[i] == "else" && tokenList[i + 1] == "if") {
                tokenList[i] = "else if";
                tokenList.erase(tokenList.begin() + i + 1);
            }

            if (tokenList[i] == "\n") {
                include = false;
            } else if (tokenList[i] == "#") {
                include = true;
            }

            if (include && tokenList[i] == "<") {
                string inclusion;
                while (i < tokenList.size()) {
                    string currentToken = tokenList[i].token;
                    inclusion += currentToken;
                    tokenList.erase(tokenList.begin() + i);
                    if (currentToken == ">") {
                        break;
                    }
                }

                tokenList.insert(tokenList.begin() + i, inclusion);
            }
        }
    }

    bool whetherToAddWhiteSpace(Token& token1, Token& token2) {
        if (token2 == "(") {
            if (token1 == "if" || token1 == "else if" || token1 == "while" || token1 == "for" || token1 == "&&" || token1 == "||" || token1 == "(" || token1 == "throw") {
                return true;
            }
            return false;
        }

        if (token1 == "(" && token2 == ")") {
            return false;
        }

        if (token1 == "!") {
            return false;
        }

        if (token2 == "[" || token2 == "," || token2 == "++" || token2 == "--") {
            return false;
        }

        if (token1 != "." && token2 != ".") {
            return true;
        }

        return false;
    }

    string isFuctionDefinition(vector<Token>& line) {
        if (line.size() < 4) {
            return "-1";
        }

        vector<string> necessityTokens = {"(", ")", "{"};
        int functionNameIndex = -10;

        for (int i = 0; i < line.size(); i++) {
            if (necessityTokens.size() > 0) {
                if (line[i] == necessityTokens[0]) {
                    necessityTokens.erase(necessityTokens.begin());
                }

                if (line[i] == "(" && functionNameIndex == -10) {
                    functionNameIndex = i - 1;
                }
            }
        }

        if (necessityTokens.size() == 0 && functionNameIndex >= 0) {
            return line[functionNameIndex].token + "()";
        }

        return "-1";
    }

    void singleLine(vector<Token>& line) {
        if (line[0] == "}") {
            levelCounter--;
        }

        for (int i = 0; i < levelCounter; i++) {
            ss << "  ";
        }

        if (autoSpace) {
            ss << "  ";
            autoSpace = false;
            commentStack.pop_back();
        }

        if ((line[0] == "if" || line[0] == "else if" || line[0] == "else" || line[0] == "while" || line[0] == "for") && line[line.size() - 1] != "{") {
            autoSpace = true;
        }

        for (int i = 0; i < line.size() - 1; i++) {
            ss << line[i];
            if (whetherToAddWhiteSpace(line[i], line[i + 1])) {
                ss << " ";
            }
        }

        ss << line[line.size() - 1];
        if (line[0] == "}" && commentStack.size() > 0) {
            ss << " // " << commentStack[commentStack.size() - 1];
            commentStack.pop_back();
        }
        ss << "\n";

        if (line[0] == "}" && line[line.size() - 1] == ";") {
            ss << "\n";
        }

        if (line[line.size() - 1] == "{") {
            levelCounter++;
        }

        if (line[0] == "if" || line[0] == "while" || line[0] == "for" || line[0] == "else" || line[0] == "else if" || line[0] == "try" || line[0] == "catch") {
            commentStack.push_back(line[0]);
        } else if (line[0] == "class" || line[0] == "struct" || line[0] == "enum" && line.size() > 1) {
            string comment = line[0].token + " " + line[1].token;
            commentStack.push_back(comment);
        } else {
            string functionName = isFuctionDefinition(line);
            if (functionName != "-1") {
                commentStack.push_back(functionName);
            }
        }
    }

    void separateLine(vector<Token>& tokenList) {
        for (int i = 0; i < tokenList.size() - 1; i++) {
            if (tokenList[i] == "}") {
                if (tokenList[i + 1] == "else" || tokenList[i + 1] == "else if") {
                    tokenList.insert(tokenList.begin() + i + 1, "\n");
                }
            }
        }
    }

    void reshape(vector<Token>& tokenList) {
        vector<Token> line;
        for (int i = 0; i < tokenList.size(); i++) {
            if (tokenList[i] != "\n") {
                line.push_back(tokenList[i]);
            } else {
                if (line.size() == 0) {
                    ss << "\n";
                } else {
                    singleLine(line);
                    line.clear();
                }
            }
        }

        if (line.size() > 0) {
            singleLine(line);
        }
    }

    void deleteComment(vector<Token>& tokenList) {
        bool deleteSingleLineComment = false;
        bool deleteMultiLineComment = false;
        for (int i = 0; i < tokenList.size(); i++) {
            if (!deleteSingleLineComment && !deleteMultiLineComment) {
                if (tokenList[i] == "//") {
                    deleteSingleLineComment = true;
                } else if (tokenList[i] == "/*") {
                    deleteMultiLineComment = true;
                }
            }

            if (tokenList[i] == "\n") {
                deleteSingleLineComment = false;
            } else if (tokenList[i] == "*/") {
                deleteMultiLineComment = false;
                tokenList.erase(tokenList.begin() + i);
                i--;
            }

            if (deleteSingleLineComment || deleteMultiLineComment) {
                tokenList.erase(tokenList.begin() + i);
                i--;
            }
        }
    }

    void addLine(vector<Token>& tokenList) {
        for (int i = 0; i < tokenList.size() - 2; i++) {
            if (tokenList[i] == "}" && tokenList[i + 1] == "\n" && (tokenList[i + 2] != "}" && tokenList[i + 2] != "\n" && tokenList[i + 2] != "else" && tokenList[i + 2] != "else if")) {
                tokenList.insert(tokenList.begin() + i + 1, "\n");
            } else if ((tokenList[i] == "\n" && tokenList[i + 1] == "{")) {
                tokenList.erase(tokenList.begin() + i);
                i--;
            }

            if (tokenList[i] == "class" || tokenList[i] == "struct" || tokenList[i] == "typedef" || tokenList[i] == "enum" || tokenList[i] == "using") {
                tokenList.insert(tokenList.begin() + i, "\n");
                i++;
            }

            if (tokenList[i] == "typedef" && tokenList[i + 1] == "struct") {
                i++;
            }
        }
    }

    stringstream ss;
    int levelCounter;
    bool autoSpace = false;
    vector<Token> commentStack;

   public:
    Formatter(char* text) {
        ss.clear();
        commentStack.clear();
        levelCounter = 0;
        autoSpace = false;
        time_t t = clock();
        vector<Token> tokenList;
        getToken(text, tokenList);
        mergeToken(tokenList);
        separateLine(tokenList);
        deleteComment(tokenList);
        addLine(tokenList);
        reshape(tokenList);

        fstream file("output.cpp", ios::out);
        file << ss.str();

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