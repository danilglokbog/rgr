#include <iomanip>
#include <cstdint>
#include <cstring>
#include "file.h"
#include "magic.h"
using namespace std;

// проверка, что квадрат магический
bool isSquareMagic(const vector<vector<int>>& square) {
    int n = square.size();
    if (n == 0){
        return false;
    }
    int target = n * (n * n + 1) / 2;
    vector<bool> numbers(n * n + 1, false);
    for (int i = 0; i < n; ++i) {
        int rowSum = 0;
        int colSum = 0;
        for (int j = 0; j < n; ++j) {
            int num = square[i][j];
            if (num < 1 || num > n * n || numbers[num]) {
                return false;
            }
            numbers[num] = true;
            rowSum += num;
            colSum += square[j][i];
        }
        if (rowSum != target || colSum != target){ 
            return false;
        }
    }
    int diag1 = 0;
    int diag2 = 0;
    for (int i = 0; i < n; ++i) {
        diag1 += square[i][i];
        diag2 += square[i][n-1-i];
    }
    return diag1 == target && diag2 == target;
}

// метод сиамский
vector<vector<int>> methodSiamez(int n) {
    vector<vector<int>> square(n, vector<int>(n, 0));
    int row = 0; 
    int col = n / 2;
    for (int num = 1; num <= n*n; ++num) {
        square[row][col] = num;
        int nextRow = (row - 1 + n) % n;
        int nextCol = (col + 1) % n;
        if (square[nextRow][nextCol] != 0) {
            row = (row + 1) % n;
        } else {
            row = nextRow;
            col = nextCol;
        }
    }
    return square;
}

//метод Рауз-Болла
vector<vector<int>> methodRouseBall(int n) {
    vector<vector<int>> square1(n, vector<int>(n));
    vector<vector<int>> square2(n, vector<int>(n));
  
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            square1[i][j] = i * n + j + 1; 
            square2[i][j] = n * n - (i * n + j);  
        }
    }

    vector<vector<int>> result(n, vector<int>(n));
    for (int blockRow = 0; blockRow < n; blockRow += 4) {
        for (int blockCol = 0; blockCol < n; blockCol += 4) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (i == j || i + j == 3) {
                        result[blockRow + i][blockCol + j] = square2[blockRow + i][blockCol + j];
                    } else {
                        result[blockRow + i][blockCol + j] = square1[blockRow + i][blockCol + j];
                    }
                }
            }
        }
    }
    
    return result;
}

//метод Стрейчи
vector<vector<int>> methodStrachey(int n) {
    int m = (n - 2) / 4;
    int subSqSize = 2*m + 1;
    vector<vector<int>> baseSquare = methodSiamez(subSqSize);
    vector<vector<vector<int>>> quadrants(4, vector<vector<int>>(subSqSize, vector<int>(subSqSize)));
    int quadrantConsts[4] = {0, 2 * subSqSize * subSqSize, 3 * subSqSize * subSqSize, subSqSize * subSqSize};   
    for (int q = 0; q < 4; ++q) {
        for (int i = 0; i < subSqSize; ++i) {
            for (int j = 0; j < subSqSize; ++j) {
                quadrants[q][i][j] = baseSquare[i][j] + quadrantConsts[q];
            }
        }
    }
    vector<vector<int>> square(n, vector<int>(n));
    for (int i = 0; i < subSqSize; ++i) {
        for (int j = 0; j < subSqSize; ++j) {
            square[i][j] = quadrants[0][i][j];                    
            square[i][j+subSqSize] = quadrants[1][i][j];          
            square[i+subSqSize][j] = quadrants[2][i][j];          
            square[i+subSqSize][j+subSqSize] = quadrants[3][i][j];
        }
    }
    int k = (subSqSize - 1) / 2;
    for (int i = 0; i < subSqSize; ++i) {
        for (int j = 0; j <= k-1; ++j) {
            if (i != k) {
                int sq = square[i][j];
                square[i][j] = square[i+subSqSize][j];
                square[i+subSqSize][j] = sq;
            }
        }
    }
    for (int i = 0; i < subSqSize; ++i) {
        for (int j = n - k + 1; j < n; ++j) {
            int sq = square[i][j];
            square[i][j] = square[i+subSqSize][j];
            square[i+subSqSize][j] = sq;
        }
    }
    int sq = square[k][0];
    square[k][0] = square[k+subSqSize][0];
    square[k+subSqSize][0] = sq;
    
    sq = square[k][k];
    square[k][k] = square[k+subSqSize][k];
    square[k+subSqSize][k] = sq;
    
    return square;
}

//создание магического квадрата
vector<vector<int>> createMagicSquare(int n) {
    if (n % 2 == 1) {
        return methodSiamez(n);
    } else if (n % 4 == 0) {
        return methodRouseBall(n);
    } else {
        return methodStrachey(n);
    }
}

//шифрование
vector<unsigned char> encText(const vector<unsigned char>& text, int n, const string& squareFilename) {
    vector<vector<int>> square = createMagicSquare(n);    
    ofstream squareFile(squareFilename);
    if (!squareFile.is_open()) {
        throw runtime_error("Не удалось создать ваш файл для квадрата");
    }    
    for (const auto& row : square) {
        for (int num : row) {
            squareFile << setw(4) << num << " ";
        }
        squareFile << endl;
    }
    squareFile.close();       
    vector<unsigned char> result;
    size_t textSize = text.size();
    size_t blockSize = n * n;       
    for (size_t i = 0; i < textSize; i += blockSize) {
        size_t currBlockSize = (blockSize < textSize - i) ? blockSize : (textSize - i);
        vector<unsigned char> block(text.begin() + i, text.begin() + i + currBlockSize);
        while (block.size() < blockSize) {
            block.push_back(0);
        }
        vector<unsigned char> encBlock(blockSize);
        for (int row = 0; row < n; ++row) {
            for (int col = 0; col < n; ++col) {
                int pos = square[row][col] - 1;
                encBlock[row * n + col] = block[pos % block.size()];
            }
        }        
        result.insert(result.end(), encBlock.begin(), encBlock.end());
    }
    return result;
}

int detSquareSize(const string& squareFilename) {
    ifstream squareFile(squareFilename);
    int size = 0;
    string line;
    while (getline(squareFile, line)) {
        if (!line.empty()) {
            size++;
        }
    }
    squareFile.close();     
    return size;
}

//дешифрование
vector<unsigned char> decText(const vector<unsigned char>& ciphtext, const string& squareFilename) {
    int n = detSquareSize(squareFilename);    
    ifstream squareFile(squareFilename); 
    vector<vector<int>> square(n, vector<int>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            squareFile >> square[i][j];
        }
    }
    squareFile.close();   
    if (!isSquareMagic(square)) {
        throw runtime_error("Загруженный квадрат не является магическим");
    }
    vector<pair<int, int>> positionMap(n * n);
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            positionMap[square[row][col] - 1] = {row, col};
        }
    }       
    vector<unsigned char> result;
    size_t cipherSize = ciphtext.size();
    size_t blockSize = n * n;        
    for (size_t i = 0; i < cipherSize; i += blockSize) {
        size_t currBlockSize = (blockSize < cipherSize - i) ? blockSize : (cipherSize - i);
        vector<unsigned char> block(ciphtext.begin() + i, ciphtext.begin() + i + currBlockSize); 
        vector<unsigned char> decBlock(blockSize);
        for (size_t pos = 0; pos < block.size(); ++pos) {
            auto [row, col] = positionMap[pos];
            decBlock[pos] = block[row * n + col];
        }
        if (i + blockSize >= cipherSize) {
            while (!decBlock.empty() && decBlock.back() == 0) {
                decBlock.pop_back();
            }
        }        
        result.insert(result.end(), decBlock.begin(), decBlock.end());
    }
    return result;
}

extern "C" {
    void magicEncFile(const string& inputFile, const string& outputFile, int n, const string& squareFile) {
        auto text = readFile(inputFile);
        auto ciphtext = encText(text, n, squareFile);
        writeFile(outputFile, ciphtext);
    }

    void magicDecFile(const string& inputFile, const string& squareFile, const string& outputFile) {
        auto ciphtext = readFile(inputFile);
        auto text = decText(ciphtext, squareFile);
        writeFile(outputFile, text);
    }

    string magicEncText(const string& text, int n, const string& squareFile) {
        vector<unsigned char> textVec(text.begin(), text.end());
        auto ciphtext = encText(textVec, n, squareFile);
        return string(ciphtext.begin(), ciphtext.end());
    }

    string magicDecText(const string& ciphtext, const string& squareFile) {
        vector<unsigned char> ciphtextVec(ciphtext.begin(), ciphtext.end());
        auto text = decText(ciphtextVec, squareFile);
        return string(text.begin(), text.end());
    }
}
