#include <iostream>
#include <limits>
#include <cstdlib>
#include <dlfcn.h>
#include "aes_menu.h"
#include "mickey_menu.h"
#include "magic_menu.h"

using namespace std;

enum class CiphType {
    AES = 1,
    MICKEY = 2,
    MAGIC = 3,
    EXIT = 4
};

int main() {
    while (true) {
        try {
            system("clear");
            cout << "=== ENCRYPTION ALGORITHM RGR ===" << endl;
            cout << "         1. AES-128 (CBC)       " << endl;
            cout << "         2. MICKEY              " << endl;
            cout << "         3. MAGIC SQUARE        " << endl;
            cout << "         4. EXIT                " << endl;
            cout << "================================" << endl;
            cout << "Выберите любой алгоритм (либо выход): ";
            
            int input;
            if (!(cin >> input) || input < 1 || input > 4) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw runtime_error("Ошибка: Введите число от 1 до 4!");
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            CiphType choice = static_cast<CiphType>(input);
            
            switch (choice) {
                case CiphType::AES:
                    aesCiph();
                    break;
                case CiphType::MICKEY:
                    mickeyCiph();
                    break;
                case CiphType::MAGIC:
                    magicSquareCiph();
                    break;
                case CiphType::EXIT:
                    cout << "Выход из программы. Всего доброго!" << endl;
                    return 0;
            }
        }
        catch (const exception& e) {
            system("clear");
            cerr << e.what() << endl;
            cout << "Уважаемый пользователь, чтобы продолжить, нажмите Enter";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}
