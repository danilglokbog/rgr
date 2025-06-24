#ifndef FILE_H
#define FILE_H
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
using namespace std;
vector<unsigned char> readFile(const string& filename);
void writeFile(const string& filename, const vector<unsigned char>& data);
#endif
