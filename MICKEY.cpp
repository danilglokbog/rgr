#include <bitset>
#include <random>
#include <iomanip>
#include <cstring>
#include "file.h"
#include "mickey.h"
using namespace std;
const int REGISTERSIZE = 100;
bitset<REGISTERSIZE> R, S;

//тапсы для регистра R
const int RTAPS[] = {
    0,1,3,4,5,6,9,12,13,16,19,20,21,22,25,28,37,38,
    41,42,45,46,50,52,54,56,58,60,61,63,64,65,66,67,
    71,72,79,80,81,82,87,88,89,90,91,92,94,95,96,97
};
const int RTAPSCOUNT = sizeof(RTAPS)/sizeof(RTAPS[0]);

//таблицы для регистра S
const bool COMP0[REGISTERSIZE-2] = {
    0,0,0,1,1,0,0,0,1,0,1,1,1,1,0,1,0,0,1,0,
    1,0,1,0,1,0,1,0,1,1,0,1,0,0,1,0,0,0,0,0,
    0,1,1,1,0,0,0,0,1,0,0,0,1,0,1,1,1,0,0,0,
    1,1,0,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,
    1,1,1,1,1,1,0,1,0,1,0,0,0,0,0,0,1,1
};

const bool COMP1[REGISTERSIZE-2] = {
    1,0,1,1,0,0,1,0,1,1,1,1,0,0,1,0,1,0,0,0,
    1,1,0,1,0,1,1,1,0,1,1,1,1,0,0,0,1,1,0,1,
    0,0,1,0,1,0,1,0,1,0,0,0,0,1,0,1,0,0,1,1,
    1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,0,0,0,
    1,0,0,0,0,1,1,1,0,0,0,1,0,0,1,1,0,0
};

const bool FB0[REGISTERSIZE] = {
    1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,0,0,1,0,1,
    1,1,1,1,1,1,1,1,1,0,0,1,1,0,0,0,0,0,0,1,
    1,1,0,0,1,0,0,1,0,1,0,1,0,0,1,0,1,1,1,1,
    0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,1,1,0,1,0,
    0,0,1,1,0,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0
};

const bool FB1[REGISTERSIZE] = {
    1,1,1,0,1,1,1,0,0,0,0,1,1,1,0,1,0,0,1,1,
    0,0,0,1,0,0,1,1,0,0,1,0,1,1,0,0,0,1,1,0,
    0,0,0,0,1,1,0,1,1,0,0,0,1,0,0,0,1,0,0,1,
    0,0,1,0,1,1,0,1,0,1,0,0,1,0,1,0,0,0,1,1,
    1,1,0,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,1
};

vector<unsigned char> genRandomBytes(size_t length) {
    vector<unsigned char> result(length);
    for (size_t i = 0; i < length; ++i) {
        result[i] = rand() % 256;
    }
    return result;
}

vector<unsigned char> genRandomIV() {
    size_t length = rand() % 11; // случайная длина от 0 до 10
    return genRandomBytes(length);
}


void clockR(bool inputBitR, bool controlBitR) {
    bool feedback = R[REGISTERSIZE-1] ^ inputBitR;
    bitset<REGISTERSIZE> newR;
    for (int i = 1; i < REGISTERSIZE; i++) {
        newR[i] = R[i-1];
    }
    newR[0] = 0;  
    //обратная связь
    for (int i = 0; i < RTAPSCOUNT; i++) {
        int pos = RTAPS[i];
        newR[pos] = newR[pos] ^ feedback;
    }  
    if (controlBitR) {
        bitset<REGISTERSIZE> shiftR = newR;
        newR ^= (shiftR << 1);
    }  
    R = newR;
}

void clockS(bool inputBitS, bool controlBitS) {
    bool feedback = S[REGISTERSIZE-1] ^ inputBitS;
    bitset<REGISTERSIZE> promS;
    for (int i = 1; i < REGISTERSIZE-1; i++) {
        promS[i] = S[i-1] ^ ((S[i] ^ COMP0[i-1]) & (S[i+1] ^ COMP1[i-1]));
    }
    promS[0] = 0;
    promS[REGISTERSIZE-1] = S[REGISTERSIZE-2];
    if (controlBitS == 0) {
        for (int i = 0; i < REGISTERSIZE; i++) {
            S[i] = promS[i] ^ (FB0[i] & feedback);
        }
    } else {
        for (int i = 0; i < REGISTERSIZE; i++) {
            S[i] = promS[i] ^ (FB1[i] & feedback);
        }
    }
}

void clockKG(bool mixing, bool inputBit) {
    //контрольные биты
    bool controlBitR = S[34] ^ R[67];
    bool controlBitS = S[67] ^ R[33];    
    //входной бит для регистра R
    bool inputBitR = inputBit;
    if (mixing) {
        inputBitR ^= S[50];
    }    
    //входной бит для регистра S
    bool inputBitS = inputBit;    
    //тактирование регистров
    clockR(inputBitR, controlBitR);
    clockS(inputBitS, controlBitS);
}

void initialization(const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    R.reset();
    S.reset();    
    //загрузка IV
    for (unsigned char byte : iv) {
        for (int i = 7; i >= 0; i--) {
            bool bit = (byte >> i) & 1;
            clockKG(true, bit);
        }
    }    
    //загрузка ключа
    for (unsigned char byte : key) {
        for (int i = 7; i >= 0; i--) {
            bool bit = (byte >> i) & 1;
            clockKG(true, bit);
        }
    }    
    //предварительное тактирование
    for (int i = 0; i < REGISTERSIZE; i++) {
        clockKG(true, 0);
    }
}

unsigned char generateKeystreamByte() {
    unsigned char byte = 0;
    for (int i = 0; i < 8; i++) {
        //генерация ключевого бита
        bool keyBit = R[0] ^ S[0];
        byte = (byte << 1) | keyBit;        
        //тактирование генератора
        clockKG(false, 0);
    }
    return byte;
}

vector<unsigned char> processData(const vector<unsigned char>& data) {
    vector<unsigned char> result;
    for (unsigned char byte : data) {
        result.push_back(byte ^ generateKeystreamByte());
    }
    return result;
}

extern "C" {
    void mickeyEncFile(const string& inputFile, const string& outputFile, const string& keyFile) {
        auto text = readFile(inputFile);
        vector<unsigned char> key = genRandomBytes(10); 
        vector<unsigned char> iv = genRandomIV();       
        vector<unsigned char> keyIv;
        keyIv.insert(keyIv.end(), key.begin(), key.end());
        keyIv.push_back(iv.size());
        keyIv.insert(keyIv.end(), iv.begin(), iv.end());
        writeFile(keyFile, keyIv);
        initialization(key, iv);
        auto ciphtext = processData(text);
        writeFile(outputFile, ciphtext);
    }

    void mickeyDecFile(const string& inputFile, const string& keyFile, const string& outputFile) {
        auto ciphtext = readFile(inputFile);
        auto keyIv = readFile(keyFile);
        vector<unsigned char> key(keyIv.begin(), keyIv.begin() + 10);
        size_t ivLength = keyIv[10];
        vector<unsigned char> iv(keyIv.begin() + 11, keyIv.begin() + 11 + ivLength);
        initialization(key, iv);
        auto text = processData(ciphtext);
        writeFile(outputFile, text);
    }

    string mickeyEncText(const string& text, const string& keyFile) {
        vector<unsigned char> textVec(text.begin(), text.end());
        vector<unsigned char> key = genRandomBytes(10);
        vector<unsigned char> iv = genRandomIV();      
        vector<unsigned char> keyIv;
        keyIv.insert(keyIv.end(), key.begin(), key.end());
        keyIv.push_back(iv.size());
        keyIv.insert(keyIv.end(), iv.begin(), iv.end());
        writeFile(keyFile, keyIv);
        initialization(key, iv);
        auto ciphtext = processData(textVec);
        return string(ciphtext.begin(), ciphtext.end());
    }

    string mickeyDecText(const string& ciphtext, const string& keyFile) {
        vector<unsigned char> ciphtextVec(ciphtext.begin(), ciphtext.end());
        auto keyIv = readFile(keyFile);
        vector<unsigned char> key(keyIv.begin(), keyIv.begin() + 10);
        size_t ivLength = keyIv[10];
        vector<unsigned char> iv(keyIv.begin() + 11, keyIv.begin() + 11 + ivLength);
        initialization(key, iv);
        auto text = processData(ciphtextVec);
        return string(text.begin(), text.end());
    }
}


