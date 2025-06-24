#include <iomanip>
#include <random>
#include <cstring>
#include "file.h"
#include "aes.h"

//таблица замен для шифрования
const unsigned char sbox[256] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

//обратная таблица замен для дешифрования
const unsigned char inv_sbox[256] = {
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

//константы для раундовых ключей
const unsigned char Rcon[10] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};


vector<unsigned char> genRandBytes(size_t length) {
    vector<unsigned char> result(length);
    for (size_t i = 0; i < length; ++i) {
        result[i] = rand() % 256;
    }
    return result;
}

//замена байтов(S-box)
void subBytes(vector<unsigned char>& state) {
    for (size_t i = 0; i < 16; ++i) {
        state[i] = sbox[state[i]];
    }
}

//обратная замена байтов(обратный S-box)
void invSubBytes(vector<unsigned char>& state) {
    for (size_t i = 0; i < 16; ++i) {
        state[i] = inv_sbox[state[i]];
    }
}

//сдвиг строк
void shiftRows(vector<unsigned char>& state) {
    //вторая строка(первая строка не сдвигается)
    unsigned char rotatedByte = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = rotatedByte;
    
    //третья строка
    rotatedByte = state[2];
    state[2] = state[10];
    state[10] = rotatedByte;
    
    rotatedByte = state[6];
    state[6] = state[14];
    state[14] = rotatedByte;
    
    //четвертая строка
    rotatedByte = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7] = state[3];
    state[3] = rotatedByte;
}

//обратный сдвиг строк
void invShiftRows(vector<unsigned char>& state) {
    //вторая строка(первая строка не сдвигается)
    unsigned char rotatedByte = state[13];
    state[13] = state[9];
    state[9] = state[5];
    state[5] = state[1];
    state[1] = rotatedByte;
    
    //третья строка
    rotatedByte = state[2];
    state[2] = state[10];
    state[10] = rotatedByte;
    
    rotatedByte = state[6];
    state[6] = state[14];
    state[14] = rotatedByte;
    
    //четвертая строка
    rotatedByte = state[3];
    state[3] = state[7];
    state[7] = state[11];
    state[11] = state[15];
    state[15] = rotatedByte;
}

// умножение в поле Галуа
unsigned char galoisMul(unsigned char a, unsigned char b) {
    unsigned char p = 0;
    for (int i = 0; i < 8; ++i) {
        if (b & 1){
            p ^= a;
        }
        bool hiBitSet = (a & 0x80);
        a <<= 1;
        if (hiBitSet){
            a ^= 0x1b;
        }
        b >>= 1;
    }
    return p;
}

//MixColumns
void mixColumns(vector<unsigned char>& state) {
    for (size_t i = 0; i < 4; ++i) {
        unsigned char s0 = state[4*i];
        unsigned char s1 = state[4*i+1];
        unsigned char s2 = state[4*i+2];
        unsigned char s3 = state[4*i+3];
        state[4*i]   = galoisMul(0x02, s0) ^ galoisMul(0x03, s1) ^ s2 ^ s3;
        state[4*i+1] = s0 ^ galoisMul(0x02, s1) ^ galoisMul(0x03, s2) ^ s3;
        state[4*i+2] = s0 ^ s1 ^ galoisMul(0x02, s2) ^ galoisMul(0x03, s3);
        state[4*i+3] = galoisMul(0x03, s0) ^ s1 ^ s2 ^ galoisMul(0x02, s3);
    }
}

//обратное MixColumns
void invMixColumns(vector<unsigned char>& state) {
    for (size_t i = 0; i < 4; ++i) {
        unsigned char s0 = state[4*i];
        unsigned char s1 = state[4*i+1];
        unsigned char s2 = state[4*i+2];
        unsigned char s3 = state[4*i+3];
        state[4*i]   = galoisMul(0x0e, s0) ^ galoisMul(0x0b, s1) ^ galoisMul(0x0d, s2) ^ galoisMul(0x09, s3);
        state[4*i+1] = galoisMul(0x09, s0) ^ galoisMul(0x0e, s1) ^ galoisMul(0x0b, s2) ^ galoisMul(0x0d, s3);
        state[4*i+2] = galoisMul(0x0d, s0) ^ galoisMul(0x09, s1) ^ galoisMul(0x0e, s2) ^ galoisMul(0x0b, s3);
        state[4*i+3] = galoisMul(0x0b, s0) ^ galoisMul(0x0d, s1) ^ galoisMul(0x09, s2) ^ galoisMul(0x0e, s3);
    }
}

//добавление раундового ключа
void addRoundKey(vector<unsigned char>& state, const vector<unsigned char>& roundKey) {
    for (size_t i = 0; i < 16; ++i) {
        state[i] ^= roundKey[i];
    }
}

//ядро расширения ключа
void keyScheduleCore(vector<unsigned char>& word, unsigned char rcon) {
    unsigned char rotatedByte = word[0];
    word[0] = word[1];
    word[1] = word[2];
    word[2] = word[3];
    word[3] = rotatedByte;   
    for (size_t i = 0; i < 4; ++i) {
        word[i] = sbox[word[i]];
    }    
    word[0] ^= rcon;
}

//расширение ключа
vector<vector<unsigned char>> expandKey(const vector<unsigned char>& key) {
    vector<vector<unsigned char>> roundKeys(11, vector<unsigned char>(16));  
    //первый раундовый ключ - это сам ключ
    for (size_t i = 0; i < 16; ++i) {
        roundKeys[0][i] = key[i];
    }
    for (size_t i = 1; i <= 10; ++i) {
        vector<unsigned char> prevKey = roundKeys[i-1];
        vector<unsigned char> newKey(16);
        vector<unsigned char> lastWord(prevKey.begin() + 12, prevKey.begin() + 16);
        keyScheduleCore(lastWord, Rcon[i]);
        for (size_t j = 0; j < 4; ++j) {
            newKey[j] = prevKey[j] ^ lastWord[j];
        }
        for (size_t j = 4; j < 16; ++j) {
            newKey[j] = newKey[j-4] ^ prevKey[j];
        }
        roundKeys[i] = newKey;
    }
    return roundKeys;
}

//шифрование блока
vector<unsigned char> encBlock(vector<unsigned char> block, const vector<vector<unsigned char>>& roundKeys) {
    addRoundKey(block, roundKeys[0]);
    for (size_t round = 1; round <= 9; ++round) {
        subBytes(block);
        shiftRows(block);
        mixColumns(block);
        addRoundKey(block, roundKeys[round]);
    }
    subBytes(block);
    shiftRows(block);
    addRoundKey(block, roundKeys[10]);
    return block;
}

//дешифрование блока
vector<unsigned char> decBlock(vector<unsigned char> block, const vector<vector<unsigned char>>& roundKeys) {
    addRoundKey(block, roundKeys[10]);
    for (int round = 9; round >= 1; --round) {
        invShiftRows(block);
        invSubBytes(block);
        addRoundKey(block, roundKeys[round]);
        invMixColumns(block);
    }
    invShiftRows(block);
    invSubBytes(block);
    addRoundKey(block, roundKeys[0]);    
    return block;
}

vector<unsigned char> padData(const vector<unsigned char>& data) {
    if (data.size() % 16 == 0) {
        return data;
    }
    size_t lenPad = 16 - (data.size() % 16);
    vector<unsigned char> padded = data;
    for (size_t i = 0; i < lenPad; ++i) {
        padded.push_back(static_cast<unsigned char>(lenPad));
    }
    return padded;
}

vector<unsigned char> unpadData(const vector<unsigned char>& data) {
    if (data.empty() || data.size() % 16 != 0) {
        return data;
    }
    unsigned char lenPad = data.back();
    if (lenPad == 0 || lenPad > 16 || lenPad > data.size()) {
        return data;
    }
    for (size_t i = data.size() - lenPad; i < data.size(); ++i) {
        if (static_cast<unsigned char>(data[i]) != lenPad) {
            return data;
        }
    }
    return vector<unsigned char>(data.begin(), data.end() - lenPad);
}

//шифрование (CBC)
vector<unsigned char> cbcEncrypt(const vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv, const string& filename) {
    vector<unsigned char> padded = padData(text);
    vector<unsigned char> ciphtext;
    vector<unsigned char> prevBlock = iv;
    auto roundKeys = expandKey(key);
    for (size_t i = 0; i < padded.size(); i += 16) {
        vector<unsigned char> block(padded.begin() + i, padded.begin() + i + 16);
        //XOR с предыдущим блоком/IV
        for (size_t j = 0; j < 16; ++j) {
            block[j] ^= prevBlock[j];
        }
        block = encBlock(block, roundKeys);
        prevBlock = block;
        ciphtext.insert(ciphtext.end(), block.begin(), block.end());
    }
    //сохраняем ключ и IV в файл
    ofstream keyFile(filename, ios::binary);
    keyFile.write(reinterpret_cast<const char*>(key.data()), key.size());
    keyFile.write(reinterpret_cast<const char*>(iv.data()), iv.size());
    keyFile.close();    
    return ciphtext;
}

//дешифрование (CBC)
vector<unsigned char> cbcDecrypt(const vector<unsigned char>& ciphtext, const vector<unsigned char>& key,  const vector<unsigned char>& iv) {
    vector<unsigned char> text;
    vector<unsigned char> prevBlock = iv;
    auto roundKeys = expandKey(key);
    for (size_t i = 0; i < ciphtext.size(); i += 16) {
        vector<unsigned char> block(ciphtext.begin() + i, ciphtext.begin() + i + 16);
        vector<unsigned char> encBlock = block;
        block = decBlock(block, roundKeys);
        //XOR с предыдущим блоком/IV
        for (size_t j = 0; j < 16; ++j) {
            block[j] ^= prevBlock[j];
        }    
        prevBlock = encBlock;
        text.insert(text.end(), block.begin(), block.end());
    }
    return unpadData(text);
}

extern "C" {
    void aesEncFile(const string& inputFile, const string& outputFile, const string& keyFile) {
        auto text = readFile(inputFile);
        vector<unsigned char> key = genRandBytes(16);
        vector<unsigned char> iv = genRandBytes(16);
        auto ciphtext = cbcEncrypt(text, key, iv, keyFile);
        writeFile(outputFile, ciphtext);
    }

    void aesDecFile(const string& inputFile, const string& keyFile, const string& outputFile) {
        auto ciphtext = readFile(inputFile);
        auto keyIv = readFile(keyFile);
        vector<unsigned char> key(keyIv.begin(), keyIv.begin() + 16);
        vector<unsigned char> iv(keyIv.begin() + 16, keyIv.end());
        auto text = cbcDecrypt(ciphtext, key, iv);
        writeFile(outputFile, text);
    }

    string aesEncText(const string& text, const string& keyFile) {
        vector<unsigned char> textVec(text.begin(), text.end());
        vector<unsigned char> key = genRandBytes(16);
        vector<unsigned char> iv = genRandBytes(16);
        auto ciphtext = cbcEncrypt(textVec, key, iv, keyFile);
        return string(ciphtext.begin(), ciphtext.end());
    }

    string aesDecText(const string& ciphtext, const string& keyFile) {
        vector<unsigned char> ciphtextVec(ciphtext.begin(), ciphtext.end());
        auto keyIv = readFile(keyFile);
        vector<unsigned char> key(keyIv.begin(), keyIv.begin() + 16);
        vector<unsigned char> iv(keyIv.begin() + 16, keyIv.end());
        auto text = cbcDecrypt(ciphtextVec, key, iv);
        return string(text.begin(), text.end());
    }
    
}

