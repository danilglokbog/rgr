#ifndef MAGIC_H
#define MAGIC_H
#include <vector>
#include <string>
using namespace std;
extern "C" {
    void magicEncFile(const string& inputFile, const string& outputFile, int n, const string& squareFile);
    void magicDecFile(const string& inputFile, const string& squareFile, const string& outputFile);
    string magicEncText(const string& text, int n, const string& squareFile);
    string magicDecText(const string& ciphtext, const string& squareFile);
}
#endif
