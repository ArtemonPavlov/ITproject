#include "sqlite/sqlite3.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <Windows.h>
#include <cstdlib>
#include <ctime>

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
        "    FIO TEXT NOT NULL,"
        "    otdel TEXT NOT NULL,"   
        "    position TEXT NOT NULL,"
        "    salary REAL NOT NULL,"
        "    isBoss TEXT NOT NULL,"
        "    EnterKey INTEGER NOT NULL"  
        ");";

    executeSQL(db, createEmployees);
}

// ������� ��� ���������� ����������
void addEmployee(sqlite3* db) {
    srand(time(nullptr));
    string fio, otdel, position, isboss;
    double salary;
    int enkey;

    cout << "\n=== ���������� ������ ���������� ===" << endl;

    cout << "������� ��� ����������: ";
    cin.ignore();
    getline(cin, fio);

    cout << "������� �����, ���� ����� �������� ���������: ";
    getline(cin, otdel);

    cout << "������� ��������� ����������: ";
    getline(cin, position);

    cout << "������� �������� ����������: ";
    cin >> salary;

    cout << "����� �� ��������� ����������� ������? (��/���): ";
    cin.ignore();
    getline(cin, isboss);

    enkey = rand() % 10000;
    cout << "���� ����������: " << enkey << endl;

    // ������� SQL ������ � �����������
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
            cout << "��������� ������� ��������!" << endl;
        }
        else {
            cout << "������ ��� ���������� ����������!" << endl;
        }

        sqlite3_finalize(stmt);
    }
}

// ������� ��� ������ ���� �����������
void showAllEmployees(sqlite3* db) {
    cout << "\n=== ������ ���� ����������� ===" << endl;

    sqlite3_stmt* stmt;
    string sql = "SELECT id, FIO, otdel, position, salary, isBoss, EnterKey FROM Employees ORDER BY id;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        cout << left << setw(10) << "ID"
            << setw(20) << "���"
            << setw(15) << "�����"
            << setw(15) << "���������"
            << setw(20) << "��������"
            << setw(20) << "���. ������?"
            << setw(20) << "���� �������" << endl;
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

// ������� ��� �������� ����������
void deleteEmployee(sqlite3* db) {
    int id;

    cout << "\n=== �������� ���������� ===" << endl;
    showAllEmployees(db);

    cout << "������� ID ���������� ��� ��������: ";
    cin >> id;

    /*// ������� ������� �������� ���������� (��-�� �������� �����)
    string deleteSalaries = "DELETE FROM Salaries WHERE employee_id = " + to_string(id) + ";";
    executeSQL(db, deleteSalaries);
    */

    // ����� ������� ������ ����������
    string deleteEmployee = "DELETE FROM Employees WHERE id = " + to_string(id) + ";";

    if (executeSQL(db, deleteEmployee)) {
        cout << "��������� �����!" << endl;
    }
}

// ������� ����
void showMenu() {
    cout << "\n========== ������� ���������� ������������ ==========" << endl;
    cout << "1. �������� ���� �����������" << endl;
    cout << "2. �������� ����������" << endl;
    cout << "3. ������� ����������" << endl;
    cout << "4. �����" << endl;
    cout << "�������� ��������: ";
}

int LogInAlfaVer(sqlite3* db, int enkey) {
    int res = 0;
    string enkF = "SELECT position FROM Employees WHERE EnterKey = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, enkF.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, enkey);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* pos = (const char*)sqlite3_column_text(stmt, 0);
            if (pos != nullptr && string(pos) == "��������") { 
                res = 1;
            }
        } else {
            cout << "�������� ���� �������!";
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

    // ��������� ���� ������
    if (sqlite3_open("company.db", &db) != SQLITE_OK) {
        cerr << "������ �������� ���� ������: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    // �������������� �������
    initializeDatabase(db);

    cout << "������� ��� ���� �������: ";
    cin >> enkey;
    int res1 = LogInAlfaVer(db, enkey);
    cout << res1 << endl;
    if (res1 == 1) {
        cout << "�� ������� ������������ ��� ��������� ���������!" << endl;
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
                cout << "����� �� ���������..." << endl;
                break;
            default:
                cout << "�������� �����! ���������� �����." << endl;
            }

        } while (choice != 4);

    } if (res1 == -1) {
        int choice1 = 0;
        do {
            choice1 += 1;
        } while (choice1 != 1);
        //cout << "� ����������..." << endl;
    }

    // ��������� ���� ������
    sqlite3_close(db);
    cout << "���� ������ �������. �� ��������!" << endl;

    return 0;
}