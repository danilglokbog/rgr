#include "file.h"

vector<unsigned char> readFile(const string& filename) {
    ifstream in(filename, ios::binary | ios::ate);
    if (!in){
        throw runtime_error("Ошибка открытия вашего файла " + filename);    
    }
    size_t size = in.tellg(); 
    in.seekg(0);   
    vector<unsigned char> bytes(size);
    if (!in.read(reinterpret_cast<char*>(bytes.data()), size)) {
        throw runtime_error("Ошибка чтения вашего файла " + filename);
    }  
    return bytes;
}

void writeFile(const string& filename, const vector<unsigned char>& data) {
    ofstream out(filename, ios::binary);
    if (!out){
        throw runtime_error("Ошибка создания вашего файла " + filename);
    }
    if (!out.write(reinterpret_cast<const char*>(data.data()), data.size())) {
        throw runtime_error("Ошибка записи в ваш файл " + filename);
    }
}
