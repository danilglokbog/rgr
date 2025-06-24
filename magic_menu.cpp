#include "magic_menu.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <iomanip>
#include <dlfcn.h>
#include <vector>
using namespace std;

enum class CiphAction { 
    ENCRYPT = 1, 
    DECRYPT = 2, 
    BACK = 3 
};
enum class InCiph { 
    CONSOLE_INPUT = 1, 
    FILE_INPUT = 2, 
    BACK = 3 
};
void magicSquareCiph() {
    void* magicLib = dlopen("./libmagic.so", RTLD_LAZY);
    if (!magicLib) {
        cerr << "Ошибка загрузки библиотеки MAGIC: " << dlerror() << endl;
        return;
    }

    auto magicEncFile = (void (*)(const string&, const string&, int, const string&))dlsym(magicLib, "magicEncFile");
    auto magicDecFile = (void (*)(const string&, const string&, const string&))dlsym(magicLib, "magicDecFile");
    auto magicEncText = (string (*)(const string&, int, const string&))dlsym(magicLib, "magicEncText");
    auto magicDecText = (string (*)(const string&, const string&))dlsym(magicLib, "magicDecText");

    bool repeat = true;
    while (repeat) {
        try {
            system("clear");
            cout << "!=======MAGIC SQUARE CIPHER=======!" << endl;
            cout << "1. Зашифровать данные" << endl;
            cout << "2. Расшифровать данные" << endl;
            cout << "3. Вернуться в главное меню" << endl;
            cout << "Выберите действие: ";
            
            int action;
            if (!(cin >> action) || action < 1 || action > 3) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw runtime_error("Неверный выбор действия. Введите число от 1 до 3.");
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            

            
            CiphAction ciphAction = static_cast<CiphAction>(action);
            
            switch (ciphAction) {
                case CiphAction::ENCRYPT: {
                    system("clear");
                    cout << "1. Ввести текст с консоли" << endl;
                    cout << "2. Зашифровать файл" << endl;
                    cout << "3. Назад" << endl;
                    cout << "Выберите источник данных: ";
                    
                    int choice;
                    if (!(cin >> choice) || choice < 1 || choice > 3) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        throw runtime_error("Неверный выбор. Введите число от 1 до 3.");
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    
                    InCiph inChoice = static_cast<InCiph>(choice);
                    string text, squareFile;
                    int n = 0;
                    
                    switch (inChoice) {
                        case InCiph::CONSOLE_INPUT: {
                            cout << "Введите текст для шифрования: ";
                            string inputText;
                            getline(cin, inputText);
                            text = inputText;
                            
                            cout << "Введите имя файла для сохранения квадрата: ";
                            getline(cin, squareFile);
                            
                            cout << "Введите размер квадрата (3-1000): ";
                            if (!(cin >> n) || n < 3 || n > 1000) {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                throw runtime_error("Неверный размер квадрата. Допустимый диапазон: 3-1000.");
                            }
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            
                            string ciphtext = magicEncText(text, n, squareFile);
                            
                            cout << "Результат шифрования: ";
                            for (auto c : ciphtext) {
                                cout << hex << setw(2) << setfill('0') << static_cast<int>(static_cast<unsigned char>(c)) << " ";
                            }
                            cout << dec << endl;
                            cout << "Магический квадрат сохранен в: " << squareFile << endl;
                            break;
                        }
                        case InCiph::FILE_INPUT: {
                            string inputFile, outputFile;
                            cout << "Введите имя файла для шифрования: ";
                            getline(cin, inputFile);
                            cout << "Введите имя файла для сохранения зашифрованных данных: ";
                            getline(cin, outputFile);
                            cout << "Введите имя файла для сохранения квадрата: ";
                            getline(cin, squareFile);
                            
                            cout << "Введите размер квадрата (3-1000): ";
                            if (!(cin >> n) || n < 3 || n > 1000) {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                throw runtime_error("Неверный размер квадрата. Допустимый диапазон: 3-1000.");
                            }
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            
                            magicEncFile(inputFile, outputFile, n, squareFile);
                            
                            cout << "Файл успешно зашифрован!" << endl;
                            cout << "Зашифрованный файл: " << outputFile << endl;
                            cout << "Магический квадрат сохранен в: " << squareFile << endl;
                            break;
                        }
                        case InCiph::BACK:
                            continue;
                    }
                    
                    cout << "Уважаемый пользователь, чтобы продолжить, нажмите Enter";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    repeat = false;
                    break;
                }
                case CiphAction::DECRYPT: {
                    system("clear");
                    cout << "1. Ввести зашифрованные данные с консоли" << endl;
                    cout << "2. Загрузить зашифрованные данные из файла" << endl;
                    cout << "3. Назад" << endl;
                    cout << "Выберите источник данных: ";
                    
                    int source;
                    if (!(cin >> source) || source < 1 || source > 3) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        throw runtime_error("Неверный выбор. Введите число от 1 до 3.");
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    
                    InCiph decryptSource = static_cast<InCiph>(source);
                    string ciphtext, squareFile, outputFile;
                    
                    switch (decryptSource) {
                        case InCiph::CONSOLE_INPUT: {
			    cout << "Введите зашифрованные данные (hex): ";
			    string hexInput;
			    getline(cin, hexInput);
			    
			    vector<unsigned char> ciphertextVec;
			    for (size_t i = 0; i < hexInput.length(); i += 2) {
				string byteString = hexInput.substr(i, 2);
				unsigned char byte = static_cast<unsigned char>(stoul(byteString, nullptr, 16));
				ciphertextVec.push_back(byte);
			    }
			    
			    cout << "Введите имя файла с квадратом: ";
			    getline(cin, squareFile);
			    
			    string ciphertext(ciphertextVec.begin(), ciphertextVec.end());
			    string result = magicDecText(ciphertext, squareFile);
			    cout << "Результат расшифровки: " << result << endl;
			    break;
			}
                        case InCiph::FILE_INPUT: {
                            string inputFile;
                            cout << "Введите имя зашифрованного файла: ";
                            getline(cin, inputFile);
                            cout << "Введите имя файла для сохранения расшифрованных данных: ";
                            getline(cin, outputFile);
                            cout << "Введите имя файла с квадратом: ";
                            getline(cin, squareFile);
                            
                            magicDecFile(inputFile, squareFile, outputFile);
                            cout << "Файл успешно расшифрован и сохранен в: " << outputFile << endl;
                            break;
                        }
                        case InCiph::BACK:
                            break;
                    }
                    
                    if (decryptSource != InCiph::BACK) {
                        cout << "Уважаемый пользователь, чтобы продолжить, нажмите Enter";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    repeat = false;
                    break;
                }
                case CiphAction::BACK:
                    repeat = false;
                    break;
            }
        }
        catch (const exception& e) {
            system("clear");
            cerr << e.what() << endl;
            cout << "Уважаемый пользователь, чтобы продолжить, нажмите Enter";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    
    dlclose(magicLib);
}
