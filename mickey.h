#ifndef MICKEY_H
#define MICKEY_H
#include <vector>
#include <string>
using namespace std;
extern "C" {
    void mickeyEncFile(const string& inputFile, const string& outputFile, const string& keyFile);
    void mickeyDecFile(const string& inputFile, const string& keyFile, const string& outputFile);
    string mickeyEncText(const string& text, const string& keyFile);
    string mickeyDecText(const string& ciphtext, const string& keyFile);
}
#endif
