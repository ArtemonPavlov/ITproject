/*case 4:
            showAllSalaries(db);
            break;
        case 5:
            addSalary(db);
            break;
        case 6:
            deleteSalary(db);
            break;
        */
        /*// Функция для удаления записи о зарплате
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
        }*/
        /*// Функция для показа всех зарплат с именами сотрудников
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
        }*/
        /*// Функция для добавления зарплаты
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
        }*/
        /*
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

           executeSQL(db, createSalaries);
           executeSQL(db, enableFK);
           */