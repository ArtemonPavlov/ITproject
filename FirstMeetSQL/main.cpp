#include "sqlite/sqlite3.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <Windows.h>

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
        "    name TEXT NOT NULL,"
        "    position TEXT NOT NULL"
        ");";

    // Таблица зарплат
    string createSalaries =
        "CREATE TABLE IF NOT EXISTS Salaries ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    employee_id INTEGER,"
        "    salary REAL NOT NULL,"
        "    date TEXT NOT NULL,"
        "    FOREIGN KEY (employee_id) REFERENCES Employees(id)"
        ");";

    // Включить внешние ключи
    string enableFK = "PRAGMA foreign_keys = ON;";

    executeSQL(db, createEmployees);
    executeSQL(db, createSalaries);
    executeSQL(db, enableFK);

    cout << "База данных готова!" << endl;
}

// Функция для добавления сотрудника
void addEmployee(sqlite3* db) {
    string name, position;

    cout << "\n=== Добавление нового сотрудника ===" << endl;

    cout << "Введите имя сотрудника: ";
    cin.ignore();
    getline(cin, name);

    cout << "Введите должность: ";
    getline(cin, position);

    // Создаем SQL запрос с параметрами
    sqlite3_stmt* stmt;
    string sql = "INSERT INTO Employees (name, position) VALUES (?, ?);";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, position.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Сотрудник успешно добавлен!" << endl;
        }
        else {
            cout << "Ошибка при добавлении сотрудника!" << endl;
        }

        sqlite3_finalize(stmt);
    }
}

// Функция для добавления зарплаты
void addSalary(sqlite3* db) {
    int employee_id;
    double salary;
    string date;

    cout << "\n=== Добавление зарплаты ===" << endl;

    // Сначала покажем всех сотрудников
    showAllEmployees(db);

    cout << "Введите ID сотрудника: ";
    cin >> employee_id;

    cout << "Введите зарплату: ";
    cin >> salary;

    cout << "Введите дату (гггг-мм-дд): ";
    cin >> date;

    sqlite3_stmt* stmt;
    string sql = "INSERT INTO Salaries (employee_id, salary, date) VALUES (?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, employee_id);
        sqlite3_bind_double(stmt, 2, salary);
        sqlite3_bind_text(stmt, 3, date.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Зарплата успешно добавлена!" << endl;
        }
        else {
            cout << "Ошибка при добавлении зарплаты!" << endl;
        }

        sqlite3_finalize(stmt);
    }
}

// Функция для показа всех сотрудников
void showAllEmployees(sqlite3* db) {
    cout << "\n=== Список всех сотрудников ===" << endl;

    sqlite3_stmt* stmt;
    string sql = "SELECT id, name, position FROM Employees ORDER BY id;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        cout << left << setw(5) << "ID"
            << setw(20) << "Имя"
            << setw(15) << "Должность" << endl;
        cout << "----------------------------------------" << endl;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char* name = sqlite3_column_text(stmt, 1);
            const unsigned char* position = sqlite3_column_text(stmt, 2);

            cout << setw(5) << id
                << setw(20) << name
                << setw(15) << position << endl;
        }

        sqlite3_finalize(stmt);
    }
}

// Функция для показа всех зарплат с именами сотрудников
void showAllSalaries(sqlite3* db) {
    cout << "\n=== Зарплаты всех сотрудников ===" << endl;

    sqlite3_stmt* stmt;
    // JOIN - объединяем две таблицы!
    string sql =
        "SELECT e.name, s.salary, s.date, s.employee_id "
        "FROM Salaries s "
        "JOIN Employees e ON s.employee_id = e.id "
        "ORDER BY s.date DESC, e.name;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        cout << left << setw(20) << "Сотрудник"
            << setw(10) << "Зарплата"
            << setw(12) << "Дата"
            << setw(10) << "ID" << endl;
        cout << "----------------------------------------------" << endl;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* name = sqlite3_column_text(stmt, 0);
            double salary = sqlite3_column_double(stmt, 1);
            const unsigned char* date = sqlite3_column_text(stmt, 2);
            int emp_id = sqlite3_column_int(stmt, 3);

            cout << setw(20) << name
                << setw(10) << salary
                << setw(12) << date
                << setw(10) << emp_id << endl;
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

    // Сначала удаляем зарплаты сотрудника (из-за внешнего ключа)
    string deleteSalaries = "DELETE FROM Salaries WHERE employee_id = " + to_string(id) + ";";
    executeSQL(db, deleteSalaries);

    // Затем удаляем самого сотрудника
    string deleteEmployee = "DELETE FROM Employees WHERE id = " + to_string(id) + ";";

    if (executeSQL(db, deleteEmployee)) {
        cout << "Сотрудник и его зарплаты удалены!" << endl;
    }
}

// Функция для удаления записи о зарплате
void deleteSalary(sqlite3* db) {
    int employee_id;
    string date;

    cout << "\n=== Удаление записи о зарплате ===" << endl;
    showAllSalaries(db);

    cout << "Введите ID сотрудника: ";
    cin >> employee_id;

    cout << "Введите дату (гггг-мм-дд): ";
    cin >> date;

    string sql = "DELETE FROM Salaries WHERE employee_id = ? AND date = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, employee_id);
        sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Запись о зарплате удалена!" << endl;
        }
        else {
            cout << "Ошибка при удалении!" << endl;
        }

        sqlite3_finalize(stmt);
    }
}

// Главное меню
void showMenu() {
    cout << "\n========== СИСТЕМА УПРАВЛЕНИЯ СОТРУДНИКАМИ ==========" << endl;
    cout << "1. Показать всех сотрудников" << endl;
    cout << "2. Добавить сотрудника" << endl;
    cout << "3. Удалить сотрудника" << endl;
    cout << "4. Показать все зарплаты" << endl;
    cout << "5. Добавить зарплату" << endl;
    cout << "6. Удалить запись о зарплате" << endl;
    cout << "7. Выход" << endl;
    cout << "Выберите действие: ";
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    sqlite3* db;

    // Открываем базу данных
    if (sqlite3_open("company.db", &db) != SQLITE_OK) {
        cerr << "Ошибка открытия базы данных: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    cout << "База данных успешно открыта!" << endl;

    // Инициализируем таблицы
    initializeDatabase(db);

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
            showAllSalaries(db);
            break;
        case 5:
            addSalary(db);
            break;
        case 6:
            deleteSalary(db);
            break;
        case 7:
            cout << "Выход из программы..." << endl;
            break;
        default:
            cout << "Неверный выбор! Попробуйте снова." << endl;
        }

    } while (choice != 7);

    // Закрываем базу данных
    sqlite3_close(db);
    cout << "База данных закрыта. До свидания!" << endl;

    return 0;
}