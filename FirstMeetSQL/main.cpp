#include "sqlite/sqlite3.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <Windows.h>

using namespace std;

// ������� ��� ���������� SQL ������ ��� �������� ����������
bool executeSQL(sqlite3* db, const string& sql) {
    char* errorMessage = nullptr;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);

    if (result != SQLITE_OK) {
        cout << "������ SQL: " << errorMessage << endl;
        sqlite3_free(errorMessage);
        return false;
    }
    return true;
}

// ������� ��� ������������� ���� ������ (�������� ������)
void initializeDatabase(sqlite3* db) {

    // ������� �����������
    string createEmployees =
        "CREATE TABLE IF NOT EXISTS Employees ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT NOT NULL,"
        "    position TEXT NOT NULL"
        ");";

    // ������� �������
    string createSalaries =
        "CREATE TABLE IF NOT EXISTS Salaries ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    employee_id INTEGER,"
        "    salary REAL NOT NULL,"
        "    date TEXT NOT NULL,"
        "    FOREIGN KEY (employee_id) REFERENCES Employees(id)"
        ");";

    // �������� ������� �����
    string enableFK = "PRAGMA foreign_keys = ON;";

    executeSQL(db, createEmployees);
    executeSQL(db, createSalaries);
    executeSQL(db, enableFK);

    cout << "���� ������ ������!" << endl;
}

// ������� ��� ���������� ����������
void addEmployee(sqlite3* db) {
    string name, position;

    cout << "\n=== ���������� ������ ���������� ===" << endl;

    cout << "������� ��� ����������: ";
    cin.ignore();
    getline(cin, name);

    cout << "������� ���������: ";
    getline(cin, position);

    // ������� SQL ������ � �����������
    sqlite3_stmt* stmt;
    string sql = "INSERT INTO Employees (name, position) VALUES (?, ?);";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, position.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "��������� ������� ��������!" << endl;
        }
        else {
            cout << "������ ��� ���������� ����������!" << endl;
        }

        sqlite3_finalize(stmt);
    }
}

// ������� ��� ���������� ��������
void addSalary(sqlite3* db) {
    int employee_id;
    double salary;
    string date;

    cout << "\n=== ���������� �������� ===" << endl;

    // ������� ������� ���� �����������
    showAllEmployees(db);

    cout << "������� ID ����������: ";
    cin >> employee_id;

    cout << "������� ��������: ";
    cin >> salary;

    cout << "������� ���� (����-��-��): ";
    cin >> date;

    sqlite3_stmt* stmt;
    string sql = "INSERT INTO Salaries (employee_id, salary, date) VALUES (?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, employee_id);
        sqlite3_bind_double(stmt, 2, salary);
        sqlite3_bind_text(stmt, 3, date.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "�������� ������� ���������!" << endl;
        }
        else {
            cout << "������ ��� ���������� ��������!" << endl;
        }

        sqlite3_finalize(stmt);
    }
}

// ������� ��� ������ ���� �����������
void showAllEmployees(sqlite3* db) {
    cout << "\n=== ������ ���� ����������� ===" << endl;

    sqlite3_stmt* stmt;
    string sql = "SELECT id, name, position FROM Employees ORDER BY id;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        cout << left << setw(5) << "ID"
            << setw(20) << "���"
            << setw(15) << "���������" << endl;
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

// ������� ��� ������ ���� ������� � ������� �����������
void showAllSalaries(sqlite3* db) {
    cout << "\n=== �������� ���� ����������� ===" << endl;

    sqlite3_stmt* stmt;
    // JOIN - ���������� ��� �������!
    string sql =
        "SELECT e.name, s.salary, s.date, s.employee_id "
        "FROM Salaries s "
        "JOIN Employees e ON s.employee_id = e.id "
        "ORDER BY s.date DESC, e.name;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        cout << left << setw(20) << "���������"
            << setw(10) << "��������"
            << setw(12) << "����"
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

// ������� ��� �������� ����������
void deleteEmployee(sqlite3* db) {
    int id;

    cout << "\n=== �������� ���������� ===" << endl;
    showAllEmployees(db);

    cout << "������� ID ���������� ��� ��������: ";
    cin >> id;

    // ������� ������� �������� ���������� (��-�� �������� �����)
    string deleteSalaries = "DELETE FROM Salaries WHERE employee_id = " + to_string(id) + ";";
    executeSQL(db, deleteSalaries);

    // ����� ������� ������ ����������
    string deleteEmployee = "DELETE FROM Employees WHERE id = " + to_string(id) + ";";

    if (executeSQL(db, deleteEmployee)) {
        cout << "��������� � ��� �������� �������!" << endl;
    }
}

// ������� ��� �������� ������ � ��������
void deleteSalary(sqlite3* db) {
    int employee_id;
    string date;

    cout << "\n=== �������� ������ � �������� ===" << endl;
    showAllSalaries(db);

    cout << "������� ID ����������: ";
    cin >> employee_id;

    cout << "������� ���� (����-��-��): ";
    cin >> date;

    string sql = "DELETE FROM Salaries WHERE employee_id = ? AND date = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, employee_id);
        sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "������ � �������� �������!" << endl;
        }
        else {
            cout << "������ ��� ��������!" << endl;
        }

        sqlite3_finalize(stmt);
    }
}

// ������� ����
void showMenu() {
    cout << "\n========== ������� ���������� ������������ ==========" << endl;
    cout << "1. �������� ���� �����������" << endl;
    cout << "2. �������� ����������" << endl;
    cout << "3. ������� ����������" << endl;
    cout << "4. �������� ��� ��������" << endl;
    cout << "5. �������� ��������" << endl;
    cout << "6. ������� ������ � ��������" << endl;
    cout << "7. �����" << endl;
    cout << "�������� ��������: ";
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    sqlite3* db;

    // ��������� ���� ������
    if (sqlite3_open("company.db", &db) != SQLITE_OK) {
        cerr << "������ �������� ���� ������: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    cout << "���� ������ ������� �������!" << endl;

    // �������������� �������
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
            cout << "����� �� ���������..." << endl;
            break;
        default:
            cout << "�������� �����! ���������� �����." << endl;
        }

    } while (choice != 7);

    // ��������� ���� ������
    sqlite3_close(db);
    cout << "���� ������ �������. �� ��������!" << endl;

    return 0;
}