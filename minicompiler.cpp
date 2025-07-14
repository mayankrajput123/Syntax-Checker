#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <cctype>
using namespace std;

stack<pair<char, int>> bracketStack;
void push(char c, int lineNumber) {
    bracketStack.push({c, lineNumber});
}
void pop(char c, int lineNumber) {
    if (bracketStack.empty()) {
        cout << "ERROR: unmatched closing '" << c << "' at line " << lineNumber << endl;
    } else {
        char top = bracketStack.top().first;
        int topLine = bracketStack.top().second;
        if ((top == '(' && c == ')') || (top == '{' && c == '}')) {
            bracketStack.pop();
        } else {
            cout << "ERROR: mismatched '" << top << "' from line " << topLine << " and '" << c << "' at line " << lineNumber << endl;
            bracketStack.pop();
        }
    }
}

int main() {
    string filename;
    cout << "Enter the filename (e.g. index.cpp): ";
    cin >> filename;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return 1;
    }
    string line;
    int lineNumber = 0;
    while (getline(file, line)) {
        lineNumber++;
        cout << line << endl;
        if (line.find("include") == 0 || line.find("define") == 0) {
            cout << "ERROR: missing '#' at line " << lineNumber << endl;
        }
        if (!line.empty() && line[0] == '#' &&
            line.substr(0, 8) != "#include" &&
            line.substr(0, 7) != "#define") {
            cout << "ERROR: malformed preprocessor directive at line " << lineNumber << endl;
        }
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (c == '{' || c == '(') {
                push(c, lineNumber);
            } else if (c == '}' || c == ')') {
                pop(c, lineNumber);
            }
        }
        if (line.find("main") != string::npos &&
            line.find("(") != string::npos &&
            line.find(")") != string::npos &&
            line.find("{") == string::npos &&
            line.back() != ';') {
            cout << "ERROR: missing '{' at line " << lineNumber << endl;
        }
        if (line.find("if") != string::npos &&
            line.find("(") == string::npos) {
            cout << "ERROR: missing '(' at line " << lineNumber << endl;
        }
        if ((line.find("int ") != string::npos ||
             line.find("cin") != string::npos ||
             line.find("cout") != string::npos) &&
            line.find(';') == string::npos &&
            line.find("{") == string::npos &&
            line.find("}") == string::npos &&
            !(line.find("main") != string::npos && line.find("(") != string::npos)) {
            cout << "ERROR:  probably missing ';' at line " << lineNumber << endl;
        }
        size_t coutPos = line.find("cout");
        if (coutPos != string::npos) {
            size_t checkPos = coutPos + 4;
            while (checkPos < line.length() && isspace(line[checkPos])) checkPos++;
            if (line.substr(checkPos, 2) != "<<") {
                cout << "ERROR: missing '<<' at line " << lineNumber << endl;
            }
        }
        size_t cinPos = line.find("cin");
        if (cinPos != string::npos) {
            size_t checkPos = cinPos + 3;
            while (checkPos < line.length() && isspace(line[checkPos])) checkPos++;
            if (line.substr(checkPos, 2) != ">>") {
                cout << "ERROR: missing '>>' at line " << lineNumber << endl;
            }
        }
    }
    while (!bracketStack.empty()) {
        char c = bracketStack.top().first;
        int line = bracketStack.top().second;
        bracketStack.pop();
        cout << "ERROR: missing closing '" << (c == '(' ? ')' : '}') << "' for '" << c << "' opened at line " << line << endl;
    }
    file.close();
    return 0;
}
