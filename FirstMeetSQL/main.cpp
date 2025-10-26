#include "sqlite/sqlite3.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <Windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;

// Функция для выполнения SQL команд без возврата результата
bool executeSQL(sqlite3* db, const string& sql) {
    char* errorMessage = nullptr;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);

    if (result != SQLITE_OK) {
        cout << "Ошибка SQL: " << errorMessage << endl;
        sqlite3_free(errorMessage);
        return false;
    }
    return true;
}

// Функция для инициализации базы данных (создание таблиц)
void initializeDatabase(sqlite3* db) {

    // Таблица сотрудников
    string createEmployees =
        "CREATE TABLE IF NOT EXISTS Employees ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    FIO TEXT NOT NULL,"
        "    otdel TEXT NOT NULL,"   
        "    position TEXT NOT NULL,"
        "    salary REAL NOT NULL,"
        "    isBoss TEXT NOT NULL,"
        "    EnterKey INTEGER NOT NULL"  
        ");";

    executeSQL(db, createEmployees);
}

// Функция для добавления сотрудника
void addEmployee(sqlite3* db) {
    srand(time(nullptr));
    string fio, otdel, position, isboss;
    double salary;
    int enkey;

    cout << "\n=== Добавление нового сотрудника ===" << endl;

    cout << "Введите ФИО сотрудника: ";
    cin.ignore();
    getline(cin, fio);

    cout << "Введите отдел, куда будет назначен сотрудник: ";
    getline(cin, otdel);

    cout << "Введите должность сотрудника: ";
    getline(cin, position);

    cout << "Введите зарплату сотрудника: ";
    cin >> salary;

    cout << "Будет ли сотрудник начальником отдела? (да/нет): ";
    cin.ignore();
    getline(cin, isboss);

    enkey = rand() % 10000;
    cout << "Ключ сотрудника: " << enkey << endl;

    // Создаем SQL запрос с параметрами
    sqlite3_stmt* stmt;
    string sql = "INSERT INTO Employees (FIO, otdel, position, salary, isBoss, EnterKey) VALUES (?, ?, ?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, fio.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, otdel.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, position.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 4, salary);
        sqlite3_bind_text(stmt, 5, isboss.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 6, enkey);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Сотрудник успешно добавлен!" << endl;
        }
        else {
            cout << "Ошибка при добавлении сотрудника!" << endl;
        }

        sqlite3_finalize(stmt);
    }
}

// Функция для показа всех сотрудников
void showAllEmployees(sqlite3* db) {
    cout << "\n=== Список всех сотрудников ===" << endl;

    sqlite3_stmt* stmt;
    string sql = "SELECT id, FIO, otdel, position, salary, isBoss, EnterKey FROM Employees ORDER BY id;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        cout << left << setw(10) << "ID"
            << setw(20) << "Имя"
            << setw(15) << "Отдел"
            << setw(15) << "Должность"
            << setw(20) << "Зарплата"
            << setw(20) << "Нач. отдела?"
            << setw(20) << "Ключ доступа" << endl;
        cout << "----------------------------------------------------------------------------------------------------------------" << endl;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char* name = sqlite3_column_text(stmt, 1);
            const unsigned char* otdel = sqlite3_column_text(stmt, 2);
            const unsigned char* position = sqlite3_column_text(stmt, 3);
            double salary = sqlite3_column_double(stmt, 4);
            const unsigned char* isboss = sqlite3_column_text(stmt, 5);
            int enkey = sqlite3_column_int(stmt, 6);

            cout << setw(5) << id
                << setw(30) << name
                << setw(20) << otdel
                << setw(20) << position
                << setw(15) << salary
                << setw(15) << isboss
                << setw(15) << enkey << endl;
        }

        sqlite3_finalize(stmt);
    }
}

// Функция для удаления сотрудника
void deleteEmployee(sqlite3* db) {
    int id;

    cout << "\n=== Удаление сотрудника ===" << endl;
    showAllEmployees(db);

    cout << "Введите ID сотрудника для удаления: ";
    cin >> id;

    /*// Сначала удаляем зарплаты сотрудника (из-за внешнего ключа)
    string deleteSalaries = "DELETE FROM Salaries WHERE employee_id = " + to_string(id) + ";";
    executeSQL(db, deleteSalaries);
    */

    // Затем удаляем самого сотрудника
    string deleteEmployee = "DELETE FROM Employees WHERE id = " + to_string(id) + ";";

    if (executeSQL(db, deleteEmployee)) {
        cout << "Сотрудник удалён!" << endl;
    }
}

// Главное меню
void showMenu() {
    cout << "\n========== СИСТЕМА УПРАВЛЕНИЯ СОТРУДНИКАМИ ==========" << endl;
    cout << "1. Показать всех сотрудников" << endl;
    cout << "2. Добавить сотрудника" << endl;
    cout << "3. Удалить сотрудника" << endl;
    cout << "4. Выход" << endl;
    cout << "Выберите действие: ";
}

int LogInAlfaVer(sqlite3* db, int enkey) {
    int res = 0;
    string enkF = "SELECT position FROM Employees WHERE EnterKey = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, enkF.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, enkey);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* pos = (const char*)sqlite3_column_text(stmt, 0);
            if (pos != nullptr && string(pos) == "Директор") { 
                res = 1;
            }
        } else {
            cout << "Неверный ключ доступа!";
            res = -1;
        }
        sqlite3_finalize(stmt);
    }

    return res;
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    sqlite3* db;
    int enkey;

    // Открываем базу данных
    if (sqlite3_open("company.db", &db) != SQLITE_OK) {
        cerr << "Ошибка открытия базы данных: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    // Инициализируем таблицы
    initializeDatabase(db);

    cout << "Введите ваш ключ доступа: ";
    cin >> enkey;
    int res1 = LogInAlfaVer(db, enkey);
    cout << res1 << endl;
    if (res1 == 1) {
        cout << "Вы успешно авторизованы под аккаунтом Директора!" << endl;
        int choice;
        do {
            showMenu();
            cin >> choice;

            switch (choice) {
            case 1:
                showAllEmployees(db);
                break;
            case 2:
                addEmployee(db);
                break;
            case 3:
                deleteEmployee(db);
                break;
            case 4:
                cout << "Выход из программы..." << endl;
                break;
            default:
                cout << "Неверный выбор! Попробуйте снова." << endl;
            }

        } while (choice != 4);

    } if (res1 == -1) {
        int choice1 = 0;
        do {
            choice1 += 1;
        } while (choice1 != 1);
        //cout << "В разработке..." << endl;
    }

    // Закрываем базу данных
    sqlite3_close(db);
    cout << "База данных закрыта. До свидания!" << endl;

    return 0;
}