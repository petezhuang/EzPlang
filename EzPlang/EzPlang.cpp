#include<iostream>
#include"lexer.h"
#include"val.h"
#include"expression.h"
#include"parser.h"
#include <fstream>
using namespace std;
string readTxtFile(const string& filename) {
    ifstream file(filename);
    string content, line;
    if (file.is_open()) {
        while (getline(file, line)) {
            content += line + "\n";
        }
        content += "\n";
        file.close();
    }
    else {
        cout << "Unable to open txt" << endl;
    }
    return content;
}

int main() {
    string filename = "test.txt";
    string a = readTxtFile(filename);
    setlevel();
    vector<token> tokens = lexer(a);
    parser(0, tokens.size() - 1, tokens);
}