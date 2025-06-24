#include "aes_menu.h"
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
void aesCiph() {
    void* aesLib = dlopen("./libaes.so", RTLD_LAZY);
    if (!aesLib) {
        cerr << "Ошибка загрузки библиотеки AES: " << dlerror() << endl;
        return;
    }

    auto aesEncFile = (void (*)(const string&, const string&, const string&))dlsym(aesLib, "aesEncFile");
    auto aesDecFile = (void (*)(const string&, const string&, const string&))dlsym(aesLib, "aesDecFile");
    auto aesEncText = (string (*)(const string&, const string&))dlsym(aesLib, "aesEncText");
    auto aesDecText = (string (*)(const string&, const string&))dlsym(aesLib, "aesDecText");

    bool repeat = true;
    while (repeat) {
        try {
            system("clear");
            cout << "!====AES-128 CBC=====!" << endl;
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
                case CiphAction::ENCRYPT: { //шифрование
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
                    
                    switch (inChoice) {
                        case InCiph::CONSOLE_INPUT: { //шифрование с консоли
                            string text, keyFile;
                            cout << "Введите текст для шифрования: ";
                            getline(cin, text);
                            cout << "Введите имя файла для сохранения ключа: ";
                            getline(cin, keyFile);
                            
                            string ciphtext = aesEncText(text, keyFile);
                            
                            cout << "Результат шифрования:" << endl;
                            for (auto c : ciphtext) {
                                cout << hex << setw(2) << setfill('0') << static_cast<int>(static_cast<unsigned char>(c)) << " ";
                            }
                            cout << dec << endl;
                            cout << "Ключ и IV сохранены в файл: " << keyFile << endl;
                            
                            cout << "Уважаемый пользователь, чтобы продолжить, нажмите Enter";
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            repeat = false;
                            break;
                        }
                        case InCiph::FILE_INPUT: { //шифрование с файла
                            string inputFile, outputFile, keyFile;
                            cout << "Введите имя файла для шифрования: ";
                            getline(cin, inputFile);
                            cout << "Введите имя файла для сохранения зашифрованных данных: ";
                            getline(cin, outputFile);
                            cout << "Введите имя файла для сохранения ключа: ";
                            getline(cin, keyFile);
                            
                            aesEncFile(inputFile, outputFile, keyFile);
                            
                            cout << "Файл успешно зашифрован!" << endl;
                            cout << "Зашифрованный файл: " << outputFile << endl;
                            cout << "Ключ и IV сохранены в файл: " << keyFile << endl;
                            
                            cout << "Уважаемый пользователь, чтобы продолжить, нажмите Enter";
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            repeat = false;
                            break;
                        }
                        case InCiph::BACK:
                            break;
                    }
                    break;
                }
                case CiphAction::DECRYPT: { //дешифрование
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
                    string ciphtext, keyFile, outputFile;
                    
                    switch (decryptSource) {
                        case InCiph::CONSOLE_INPUT: { //дешифрование с консоли
                            cout << "Введите зашифрованные данные (hex): ";
			    string hexInput;
			    getline(cin, hexInput);
			    
			    vector<unsigned char> ciphertextVec;
			    for (size_t i = 0; i < hexInput.length(); i += 2) {
				string byteString = hexInput.substr(i, 2);
				unsigned char byte = static_cast<unsigned char>(stoul(byteString, nullptr, 16));
				ciphertextVec.push_back(byte);
			    }
			    
			    cout << "Введите имя файла с ключом: ";
			    getline(cin, keyFile);
			    
			    string ciphertext(ciphertextVec.begin(), ciphertextVec.end());
			    string result = aesDecText(ciphertext, keyFile);
			    cout << "Результат расшифровки: " << result << endl;
			    break;
                        }
                        case InCiph::FILE_INPUT: { //дешифрование с файла
                            string inputFile;
                            cout << "Введите имя зашифрованного файла: ";
                            getline(cin, inputFile);
                            cout << "Введите имя файла для сохранения расшифрованных данных: ";
                            getline(cin, outputFile);
                            cout << "Введите имя файла с ключом: ";
                            getline(cin, keyFile);
                            
                            aesDecFile(inputFile, keyFile, outputFile);
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
    
    dlclose(aesLib);
}
