#include "sqlite/sqlite3.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include "export_manager.h"

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
        "    id INTEGER PRIMARY KEY,"
        "    FIO TEXT NOT NULL,"
        "    otdel TEXT NOT NULL,"
        "    position TEXT NOT NULL,"
        "    salary REAL NOT NULL,"
        "    isBoss TEXT NOT NULL,"
        "    EnterKey INTEGER NOT NULL"
        ");";

    executeSQL(db, createEmployees);
}

// Функция показа ваших данных
void ShowU(sqlite3* db, int urid) {
    cout << "\n=== ДАННЫЕ О СОТРУДНИКЕ ===" << endl;

    sqlite3_stmt* stmt;
    string sql = "SELECT id, FIO, otdel, position, salary, isBoss, EnterKey FROM Employees WHERE id = ?;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, urid);

        cout << left << setw(10) << "ID"
            << setw(30) << "ФИО"
            << setw(15) << "Отдел"
            << setw(35) << "Должность"
            << setw(15) << "Зарплата"
            << setw(15) << "Нач. отдела?"
            << setw(15) << "Ключ доступа" << endl;
        cout << "------------------------------------------------------------------------------------------------------------------------------" << endl;

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
                << setw(35) << position
                << setw(15) << salary
                << setw(15) << isboss
                << setw(15) << enkey << endl;
        }
        sqlite3_finalize(stmt);
    }
}

// Функция показа своего отдела
void ShowUrOtdel(sqlite3* db, string urotd) {
    cout << "\n=== Список всех сотрудников вашего отдела: " + urotd + " ===" << endl;

    sqlite3_stmt* stmt;
    string sql = "SELECT id, FIO, position, salary, EnterKey FROM Employees WHERE otdel = ? ORDER BY id;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, urotd.c_str(), -1, SQLITE_STATIC);

        cout << left << setw(10) << "ID"
            << setw(30) << "ФИО"
            << setw(25) << "Должность"
            << setw(15) << "Зарплата"
            << setw(15) << "Ключ доступа" << endl;
        cout << "---------------------------------------------------------------------------------------------------------" << endl;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char* name = sqlite3_column_text(stmt, 1);
            const unsigned char* position = sqlite3_column_text(stmt, 2);
            double salary = sqlite3_column_double(stmt, 3);
            int enkey = sqlite3_column_int(stmt, 4);

            cout << setw(5) << id
                << setw(30) << name
                << setw(30) << position
                << setw(15) << salary
                << setw(15) << enkey << endl;
        }
        sqlite3_finalize(stmt);
    }
}

// Функция редактирования своего ключа доступа
void EditEnterKey(sqlite3* db, int oldkey, int id) {
    int newkey, uek;
    cout << "\n=== Редактирование ключа доступа ===" << endl;
    cout << "Введите текущий ключ доступа: ";
    cin >> uek;

    if (uek == oldkey) {
        cout << "Введите новый ключ доступа (любое целое число больше 1000): ";
        cin >> newkey;
        if (cin.good() && newkey > 1000) {
            string sql2 = "UPDATE Employees SET EnterKey = " + to_string(newkey) + " WHERE id = " + to_string(id) + ";";
            if (executeSQL(db, sql2)) {
                cout << "\nКлюч доступа успешно изменён!\n";
            }
            else {
                cout << "\nОшибка при редактировании ключа доступа!\n";
            }
        }
        else {
            cout << "\nВведено неверное значение!\n";
        }
        
    }
    else {
        cout << "\nВведён неверный ключ доступа!\n";
    }
}

// Функция показа своего начальника (Обычный Работник)
void ShowUrBoss(sqlite3* db, string urotd) {
    string isb = "да", nfio;
    sqlite3_stmt* stmt;
    string sql = "SELECT FIO FROM Employees WHERE otdel = ? AND isBoss = ?;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, urotd.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, isb.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* temp_nfio = (const char*)sqlite3_column_text(stmt, 0);
            if (temp_nfio != nullptr) {
                nfio = temp_nfio;
                cout << "\nНачальника вашего отдела зовут " << nfio << endl;
            }
        }
        sqlite3_finalize(stmt);
    }
}

// Функция для добавления сотрудника (Директор)
void addEmployee(sqlite3* db) {
    srand(time(nullptr));
    string fio, otdel, position, isboss;
    double salary;
    int enkey, new_id = 0;

    cout << "\n=== Добавление нового сотрудника ===" << endl;

    string max_id_sql = "SELECT MAX(id) FROM Employees;";
    sqlite3_stmt* stmt0;

    if (sqlite3_prepare_v2(db, max_id_sql.c_str(), -1, &stmt0, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt0) == SQLITE_ROW) {
            new_id = sqlite3_column_int(stmt0, 0) + 1;
        }
        sqlite3_finalize(stmt0);
    }

    cout << "Введите ФИО сотрудника: ";
    cin.ignore();
    getline(cin, fio);

    cout << "Введите отдел, куда будет назначен сотрудник: ";
    getline(cin, otdel);

    cout << "Введите должность сотрудника: ";
    getline(cin, position);

    cout << "Введите зарплату сотрудника (тыс. руб./мес.): ";
    cin >> salary;
    if (cin.good()) {

        cout << "Будет ли сотрудник начальником отдела? (да/нет)(*любые другие введённые значения будут восприняты как 'нет'): ";
        cin.ignore();
        getline(cin, isboss);
        if (isboss != "да" && isboss != "нет") {
            isboss = "нет";
        }

        enkey = rand() % (100000 - 1000 + 1) + 1000;
        cout << "Ключ сотрудника: " << enkey << endl;

        sqlite3_stmt* stmt;
        string sql = "INSERT INTO Employees (id, FIO, otdel, position, salary, isBoss, EnterKey) VALUES (?, ?, ?, ?, ?, ?, ?);";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_double(stmt, 1, new_id);
            sqlite3_bind_text(stmt, 2, fio.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, otdel.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 4, position.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_double(stmt, 5, salary);
            sqlite3_bind_text(stmt, 6, isboss.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 7, enkey);

            if (sqlite3_step(stmt) == SQLITE_DONE) {
                cout << "Сотрудник успешно добавлен!" << endl;
            }
            else {
                cout << "Ошибка при добавлении сотрудника!" << endl;
            }

            sqlite3_finalize(stmt);
        }
    }
    else {
        cout << "\nВведено неверное значение!\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

// Функция для удаления сотрудника (Директор)
void deleteEmployee(sqlite3* db) {
    int id;

    cout << "\n=== Удаление сотрудника ===" << endl;

    cout << "Введите ID сотрудника для удаления: ";
    cin >> id;

    string deleteEmployee = "DELETE FROM Employees WHERE id = " + to_string(id) + ";";

    if (executeSQL(db, deleteEmployee)) {
        cout << "Сотрудник удалён!" << endl;
    }
}

// Функция для показа всех сотрудников (Директор)
void showAllEmployees(sqlite3* db) {
    cout << "\n=== Список всех сотрудников ===" << endl;

    sqlite3_stmt* stmt;
    string sql = "SELECT id, FIO, otdel, position, salary, isBoss, EnterKey FROM Employees ORDER BY id;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        cout << left << setw(10) << "ID"
            << setw(30) << "ФИО"
            << setw(15) << "Отдел"
            << setw(35) << "Должность"
            << setw(15) << "Зарплата"
            << setw(15) << "Нач. отдела?"
            << setw(15) << "Ключ доступа" << endl;
        cout << "------------------------------------------------------------------------------------------------------------------------------" << endl;

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
                << setw(35) << position
                << setw(15) << salary
                << setw(15) << isboss
                << setw(15) << enkey << endl;
        }

        sqlite3_finalize(stmt);
    }
}

// Функция редактирования данных (Директор)
void editdatafunc(sqlite3* db, string column, string newdata, int id) {
    string sql2 = "UPDATE Employees SET '" + column + "' = '" + newdata + "' WHERE id = " + to_string(id) + ";";
    if (executeSQL(db, sql2)) {
        cout << "\nДанные успешно изменены!\n";
    }
    else {
        cout << "\nОшибка при редактировании данных!\n";
    }
}

// Функция редактирования данных. Менюшка. (Директор)
void EditData(sqlite3* db) {
    int id, choice;
    cout << "\n=== Редактирование данных сотрудника ===" << endl;
    cout << "Введите ID сотрудника для изменения его данных: ";
    cin >> id;

    string sql = "SELECT salary FROM Employees WHERE id = " + to_string(id) + ";";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            ShowU(db, id);
            cout << "\n=== РЕДАКТИРОВАНИЕ ДАННЫХ ===" << endl;
            cout << "1. Изменить ФИО" << endl;
            cout << "2. Изменить отдел" << endl;
            cout << "3. Изменить должность" << endl;
            cout << "4. Изменить зарплату" << endl;
            cout << "5. Изменить статус начальника" << endl;
            cout << "6. Изменить ключ доступа" << endl;
            cout << "Выберите поле для редактирования: ";
            cin >> choice;
            cin.ignore(10000, '\n');

            switch (choice) {
            case 1: {
                string newfio;
                cout << "Введите новое ФИО: ";
                getline(cin, newfio);
                editdatafunc(db, "FIO", newfio, id);
                break;
            }
            case 2: {
                string newot;
                cout << "Введите новый отдел: ";
                getline(cin, newot);
                editdatafunc(db, "otdel", newot, id);
                break;
            }
            case 3: {
                string newpos;
                cout << "Введите новую должность: ";
                getline(cin, newpos);
                editdatafunc(db, "position", newpos, id);
                break;
            }
            case 4: {
                string newsal;
                cout << "Введите новую зарплату: ";
                getline(cin, newsal);
                editdatafunc(db, "salary", newsal, id);
                break;
            }
            case 5: {
                string newisb;
                cout << "Введите новый статус начальника(да/нет): ";
                cin >> newisb;
                editdatafunc(db, "isBoss", newisb, id);
                break;
            }
            case 6: {
                string newenk;
                cout << "Введите новый ключ доступа: ";
                cin >> newenk;
                editdatafunc(db, "EnterKey", newenk, id);
                break;
            }
            case 0:
                cout << "Возврат в главное меню..." << endl;
                break;
            default:
                cout << "Неверный выбор!" << endl;
            }
        }
        else {
            cout << "\nОшибка: неверный id!\n";
        }
        sqlite3_finalize(stmt);
    }
}

// Функция для показа всех сотрудников (Бухгалтер)
void showAllEmployeesBuh(sqlite3* db) {
    cout << "\n=== Список всех сотрудников ===" << endl;

    sqlite3_stmt* stmt;
    string sql = "SELECT id, FIO, otdel, position, salary, isBoss FROM Employees ORDER BY id;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        cout << left << setw(10) << "ID"
            << setw(30) << "ФИО"
            << setw(15) << "Отдел"
            << setw(30) << "Должность"
            << setw(15) << "Зарплата"
            << setw(15) << "Нач. отдела?" << endl;
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char* name = sqlite3_column_text(stmt, 1);
            const unsigned char* otdel = sqlite3_column_text(stmt, 2);
            const unsigned char* position = sqlite3_column_text(stmt, 3);
            double salary = sqlite3_column_double(stmt, 4);
            const unsigned char* isboss = sqlite3_column_text(stmt, 5);

            cout << setw(5) << id
                << setw(30) << name
                << setw(20) << otdel
                << setw(30) << position
                << setw(15) << salary
                << setw(15) << isboss << endl;
        }
        sqlite3_finalize(stmt);
    }
}

// Функция редактирования зарплаты (Бухгатлтер)
void EditSalary(sqlite3* db) {
    int id;
    double newsal;
    cout << "\n=== Редактирование зарплаты сотрудника ===" << endl;
    cout << "Введите ID сотрудника для изменения его зарплаты: ";
    cin >> id;

    string sql = "SELECT salary FROM Employees WHERE id = " + to_string(id) + ";";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "Введите новую зарплату сотрудника: ";
            cin >> newsal;

            string sql2 = "UPDATE Employees SET salary = " + to_string(newsal) + " WHERE id = " + to_string(id) + ";";
            if (executeSQL(db, sql2)) {
                cout << "\nЗарплата успешно изменена!\n";
            }
            else {
                cout << "\nОшибка при редактировании зарплаты!\n";
            }
        }
        else {
            cout << "\nОшибка: неверный id!\n";
        }
        sqlite3_finalize(stmt);
    }
}

// Главное меню Обычного Работника
void showMenuEmpl() {
    cout << "\n========== СИСТЕМА УПРАВЛЕНИЯ СОТРУДНИКАМИ ==========" << endl;
    cout << "1. Показать свои данные" << endl;
    cout << "2. Показать начальника своего отдела" << endl;
    cout << "3. Редактировать свой ключ доступа" << endl;
    cout << "4. Выход" << endl;
    cout << "Выберите действие: ";
}

// Главное меню Начальнка Отдела
void showMenuBossEmpl() {
    cout << "\n========== СИСТЕМА УПРАВЛЕНИЯ СОТРУДНИКАМИ ==========" << endl;
    cout << "1. Показать свои данные" << endl;
    cout << "2. Показать сотрудников своего отдела" << endl;
    cout << "3. Редактировать свой ключ доступа" << endl;
    cout << "4. Выход" << endl;
    cout << "Выберите действие: ";
}

// Главное меню Бухгалтера
void showMenuBuh() {
    cout << "\n========== СИСТЕМА УПРАВЛЕНИЯ СОТРУДНИКАМИ ==========" << endl;
    cout << "1. Показать свои данные" << endl;
    cout << "2. Показать начальника своего отдела" << endl;
    cout << "3. Редактировать свой ключ доступа" << endl;
    cout << "4. Показать всех сотрудников" << endl;
    cout << "5. Редактировать зарплату сотрудника (по id)" << endl;
    cout << "6. Выход" << endl;
    cout << "Выберите действие: ";
}

// Главное меню Главного Бухгалтера
void showMenuGlBuh() {
    cout << "\n========== СИСТЕМА УПРАВЛЕНИЯ СОТРУДНИКАМИ ==========" << endl;
    cout << "1. Показать свои данные" << endl;
    cout << "2. Показать всех бухгалтеров" << endl;
    cout << "3. Редактировать свой ключ доступа" << endl;
    cout << "4. Показать всех сотрудников" << endl;
    cout << "5. Редактировать зарплату сотрудника (по id)" << endl;
    cout << "6. Выход" << endl;
    cout << "Выберите действие: ";
}

// Главное меню Директора
void showMenuBoss() {
    cout << "\n========== СИСТЕМА УПРАВЛЕНИЯ СОТРУДНИКАМИ ==========" << endl;
    cout << "1. Показать свои данные" << endl;
    cout << "2. Показать сотрудников своего отдела" << endl;
    cout << "3. Редактировать свой ключ доступа" << endl;
    cout << "4. Показать всех сотрудников" << endl;
    cout << "5. Добавить сотрудника" << endl;
    cout << "6. Удалить сотрудника" << endl;
    cout << "7. Редактировать данные сотрудника (по id)" << endl;
    cout << "8. Выгрузить отчёт о данных сотрудников (JSON,CSV,XML,YAML)" << endl;
    cout << "9. Выход" << endl;
    cout << "Выберите действие: ";
}

// Система авторизации
int tpid = 0;

int LogIn(sqlite3* db, int enkey) {
    int res = 0;
    string enkF = "SELECT id, otdel, position, isBoss FROM Employees WHERE EnterKey = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, enkF.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, enkey);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            tpid = (int)sqlite3_column_int(stmt, 0);
            const char* otd = (const char*)sqlite3_column_text(stmt, 1);
            const char* pos = (const char*)sqlite3_column_text(stmt, 2);
            const char* isb = (const char*)sqlite3_column_text(stmt, 3);
            if (string(pos) == "Директор") { 
                res = 1;
            }
            else if (string(otd) == "Бухгалтерия" && string(isb) == "да") {
                res = 2;
            }
            else if (string(otd) == "Бухгалтерия" && string(isb) == "нет") {
                res = 3;
            }
            else if (string(otd) != "Бухгалтерия" && string(isb) == "нет") {
                res = 4;
            }
            else if (string(otd) != "Бухгалтерия" && string(isb) == "да") {
                res = 5;
            }
        } else {
            cout << "Неверный ключ доступа!\n";
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
    int res1 = LogIn(db, enkey);
    // Директор
    if (res1 == 1) {
        cout << "Вы успешно авторизованы!" << endl;
        int choice;
        string otdl;
        do {
            string getotd = "SELECT otdel FROM Employees WHERE EnterKey = ?";
            sqlite3_stmt* stmt;
            if (sqlite3_prepare_v2(db, getotd.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, enkey);
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    const char* temp_otd = (const char*)sqlite3_column_text(stmt, 0);
                    if (temp_otd != nullptr) {
                        otdl = temp_otd;
                    }
                }
                sqlite3_finalize(stmt);
            }
            showMenuBoss();
            cin >> choice;
            if (cin.fail()) {
                cin.clear(); 
                cin.ignore(10000, '\n');
                cout << "\nВведено неверное значение! Попробуйте ещё раз." << endl;
                continue; 
            }

            switch (choice) {
            case 1:
                ShowU(db, tpid);
                break;
            case 2:
                ShowUrOtdel(db, otdl);
                break;
            case 3:
                EditEnterKey(db, enkey, tpid);
                break;
            case 4:
                showAllEmployees(db);
                break;
            case 5:
                addEmployee(db);
                break;
            case 6:
                deleteEmployee(db);
                break;
            case 7:
                EditData(db);
                break;
            case 8:
                ExportManager::exportTableToAllFormats(db, "Employees");
                break;
            case 9:
                cout << "Выход из программы..." << endl;
                tpid = 0;
                break;
            default:
                cout << "\nНеверный выбор! Попробуйте снова." << endl;
            }

        } while (choice != 9);
    } 
    // Неверный ключ
    else if (res1 == -1) {
        int choice1 = 0;
        do {choice1 += 1;} while (choice1 != 1);
    }
    // Главный бухгалтер
    else if (res1 == 2) {
        cout << "Вы успешно авторизованы под аккаунтом!" << endl;
        int choice;
        do {
            string otdl = "Бухгалтерия";
            showMenuGlBuh();
            cin >> choice;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "\nВведено неверное значение! Попробуйте ещё раз." << endl;
                continue; 
            }

            switch (choice) {
            case 1:
                ShowU(db, tpid);
                break;
            case 2:
                ShowUrOtdel(db, otdl);
                break;
            case 3:
                EditEnterKey(db, enkey, tpid);
                break;
            case 4:
                showAllEmployeesBuh(db);
                break;
            case 5:
                EditSalary(db);
                break;
            case 6:
                cout << "Выход из программы..." << endl;
                tpid = 0;
                break;
            default:
                cout << "\nНеверный выбор! Попробуйте снова." << endl;
            }
        } while (choice != 6);
    }
    // Бухгалтер
    else if (res1 == 3) {
        cout << "Вы успешно авторизованы!" << endl;
        int choice;
        do {
            string otdl = "Бухгалтерия";
            showMenuBuh();
            cin >> choice;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "\nВведено неверное значение! Попробуйте ещё раз." << endl;
                continue; 
            }

            switch (choice) {
            case 1:
                ShowU(db, tpid);
                break;
            case 2:
                ShowUrBoss(db, otdl);
                break;
            case 3:
                EditEnterKey(db, enkey, tpid);
                break;
            case 4:
                showAllEmployeesBuh(db);
                break;
            case 5:
                EditSalary(db);
                break;
            case 6:
                cout << "Выход из программы..." << endl;
                tpid = 0;
                break;
            default:
                cout << "\nНеверный выбор! Попробуйте снова." << endl;
            }
        } while (choice != 6);
    }
    // Обычный работяга
    else if (res1 == 4) {
        cout << "Вы успешно авторизованы!" << endl;
        int choice;
        string otd;
        do {
            string getotd = "SELECT otdel FROM Employees WHERE EnterKey = ?";
            sqlite3_stmt* stmt;
            if (sqlite3_prepare_v2(db, getotd.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, enkey);
                if (sqlite3_step(stmt) == SQLITE_ROW) { 
                    const char* temp_otd = (const char*)sqlite3_column_text(stmt, 0);
                    if (temp_otd != nullptr) {
                        otd = temp_otd;
                    }
                }
                sqlite3_finalize(stmt);
            }
            showMenuEmpl();
            cin >> choice;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "\nВведено неверное значение! Попробуйте ещё раз." << endl;
                continue;
            }

            switch (choice) {
            case 1:
                ShowU(db, tpid);
                break;
            case 2:
                ShowUrBoss(db, otd);
                break;
            case 3:
                EditEnterKey(db, enkey, tpid);
                break;
            case 4:
                cout << "Выход из программы..." << endl;
                tpid = 0;
                break;
            default:
                cout << "\nНеверный выбор! Попробуйте снова." << endl;
            }
        } while (choice != 4);
    }
    // Начальник отдела
    else if (res1 == 5) {
        cout << "Вы успешно авторизованы!" << endl;
        int choice;
        string otd;
        do {
            string getotd = "SELECT otdel FROM Employees WHERE EnterKey = ?";
            sqlite3_stmt* stmt;
            if (sqlite3_prepare_v2(db, getotd.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, enkey);
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    const char* temp_otd = (const char*)sqlite3_column_text(stmt, 0);
                    if (temp_otd != nullptr) {
                        otd = temp_otd;
                    }
                }
                sqlite3_finalize(stmt);
            }
            showMenuBossEmpl();
            cin >> choice;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "\nВведено неверное значение! Попробуйте ещё раз." << endl;
                continue;
            }

            switch (choice) {
            case 1:
                ShowU(db, tpid);
                break;
            case 2:
                ShowUrOtdel(db, otd);
                break;
            case 3:
                EditEnterKey(db, enkey, tpid);
                break;
            case 4:
                cout << "Выход из программы..." << endl;
                tpid = 0;
                break;
            default:
                cout << "\nНеверный выбор! Попробуйте снова." << endl;
            }
        } while (choice != 4);
    }

    // Закрываем базу данных
    sqlite3_close(db);
    cout << "База данных закрыта. До свидания!" << endl;

    return 0;
}