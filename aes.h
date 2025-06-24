#ifndef AES_H
#define AES_H
#include <vector>
#include <string>
using namespace std;
extern "C" {
    void aesEncFile(const string& inputFile, const string& outputFile, const string& keyFile);
    void aesDecFile(const string& inputFile, const string& keyFile, const string& outputFile);
    string aesEncText(const string& text, const string& keyFile);
    string aesDecText(const string& ciphtext, const string& keyFile);
}
#endif
