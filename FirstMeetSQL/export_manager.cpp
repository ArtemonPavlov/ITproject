#include <iostream>
#include "export_manager.h"
#include "sqlite/sqlite3.h"
#include <fstream>
#include <filesystem>
#include <sstream>

using namespace std;

// Для создания папки
#ifdef _WIN32
    #include <direct.h>
    #include <errno.h>
    #define CREATE_DIR(path) _mkdir(path.c_str())
#else
    #include <sys/stat.h>
    #include <errno.h>
    #define CREATE_DIR(path) mkdir(path.c_str(), 0755)
#endif

// Создает папку 'out' если её нет
bool ExportManager::ensureOutputDirectory() {
    const string dir = "out";
    int result = CREATE_DIR(dir);
    return (result == 0 || errno == EEXIST);
}

// Получает названия колонок таблицы
vector<string> ExportManager::getTableColumns(sqlite3* db, const string& tableName) {
    vector<string> columns;
    string sql = "PRAGMA table_info(" + tableName + ");";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* columnName = (const char*)sqlite3_column_text(stmt, 1);
            if (columnName) {
                columns.push_back(columnName);
            }
        }
        sqlite3_finalize(stmt);
    }

    return columns;
}

// Получает все данные из таблицы
vector<map<string, string>> ExportManager::getTableData(sqlite3* db, const string& tableName) {
    vector<map<string, string>> data;
    auto columns = getTableColumns(db, tableName);

    if (columns.empty()) {
        return data;
    }

    string sql = "SELECT * FROM " + tableName + ";";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            map<string, string> row;

            for (size_t i = 0; i < columns.size(); i++) {
                const char* value = (const char*)sqlite3_column_text(stmt, i);
                row[columns[i]] = value ? value : "NULL";
            }

            data.push_back(row);
        }
        sqlite3_finalize(stmt);
    }

    return data;
}

// Функции экранирования (для правильного форматирования)

string ExportManager::escapeJSON(const string& input) {
    string output;
    for (char c : input) {
        switch (c) {
        case '"':  output += "\\\""; break;
        case '\\': output += "\\\\"; break;
        case '\b': output += "\\b"; break;
        case '\f': output += "\\f"; break;
        case '\n': output += "\\n"; break;
        case '\r': output += "\\r"; break;
        case '\t': output += "\\t"; break;
        default:   output += c; break;
        }
    }
    return output;
}

string ExportManager::escapeCSV(const string& input) {
    if (input.find('"') != string::npos ||
        input.find(',') != string::npos ||
        input.find('\n') != string::npos) {
        return "\"" + input + "\"";
    }
    return input;
}

string ExportManager::escapeXML(const string& input) {
    string output;
    for (char c : input) {
        switch (c) {
        case '&':  output += "&amp;"; break;
        case '<':  output += "&lt;"; break;
        case '>':  output += "&gt;"; break;
        case '"':  output += "&quot;"; break;
        case '\'': output += "&apos;"; break;
        default:   output += c; break;
        }
    }
    return output;
}

// Экспорт в JSON
bool ExportManager::exportToJSON(const vector<map<string, string>>& data, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;

    file << "[\n";
    for (size_t i = 0; i < data.size(); i++) {
        file << "  {";
        size_t j = 0;
        for (const auto& entry : data[i]) {
            file << "\"" << entry.first << "\": \"" << escapeJSON(entry.second) << "\"";
            if (++j < data[i].size()) file << ", ";
        }
        file << "}";
        if (i < data.size() - 1) file << ",";
        file << "\n";
    }
    file << "]\n";

    file.close();
    return true;
}

// Экспорт в CSV
bool ExportManager::exportToCSV(const vector<map<string, string>>& data, const vector<string>& columns, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;

    // Заголовок
    for (size_t i = 0; i < columns.size(); i++) {
        file << escapeCSV(columns[i]);
        if (i < columns.size() - 1) file << ",";
    }
    file << "\n";

    // Данные
    for (const auto& row : data) {
        for (size_t i = 0; i < columns.size(); i++) {
            auto it = row.find(columns[i]);
            if (it != row.end()) {
                file << escapeCSV(it->second);
            }
            if (i < columns.size() - 1) file << ",";
        }
        file << "\n";
    }

    file.close();
    return true;
}

// Экспорт в XML
bool ExportManager::exportToXML(const vector<map<string, string>>& data, const string& tableName, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;

    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<" << tableName << "_data>\n";

    for (const auto& row : data) {
        file << "  <" << tableName << ">\n";
        for (const auto& entry : row) {
            file << "    <" << entry.first << ">" << escapeXML(entry.second) << "</" << entry.first << ">\n";
        }
        file << "  </" << tableName << ">\n";
    }

    file << "</" << tableName << "_data>\n";
    file.close();
    return true;
}

// Экспорт в YAML
bool ExportManager::exportToYAML(const vector<map<string, string>>& data,
    const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;

    for (size_t i = 0; i < data.size(); i++) {
        file << "- \n";
        for (const auto& entry : data[i]) {
            file << "  " << entry.first << ": " << entry.second << "\n";
        }
        if (i < data.size() - 1) file << "\n";
    }

    file.close();
    return true;
}

// Главная функция экспорта
bool ExportManager::exportTableToAllFormats(sqlite3* db, const string& tableName) {
    if (!ensureOutputDirectory()) {
        cout << "Ошибка создания папки 'out'!" << endl;
        return false;
    }

    cout << " Экспорт таблицы '" << tableName << "'..." << endl;

    auto data = getTableData(db, tableName);
    auto columns = getTableColumns(db, tableName);

    if (data.empty()) {
        cout << "Таблица '" << tableName << "' пуста или не существует!" << endl;
        return false;
    }

    bool success = true;

    // Экспорт во все форматы
    if (!exportToJSON(data, "out/data.json")) {
        cout << "Ошибка экспорта в JSON" << endl;
        success = false;
    }
    else {
        cout << "JSON экспортирован: out/data.json" << endl;
    }

    if (!exportToCSV(data, columns, "out/data.csv")) {
        cout << "Ошибка экспорта в CSV" << endl;
        success = false;
    }
    else {
        cout << "CSV экспортирован: out/data.csv" << endl;
    }

    if (!exportToXML(data, tableName, "out/data.xml")) {
        cout << "Ошибка экспорта в XML" << endl;
        success = false;
    }
    else {
        cout << "XML экспортирован: out/data.xml" << endl;
    }

    if (!exportToYAML(data, "out/data.yaml")) {
        cout << "❌ Ошибка экспорта в YAML" << endl;
        success = false;
    }
    else {
        cout << "YAML экспортирован: out/data.yaml" << endl;
    }

    if (success) {
        cout << "Все форматы успешно экспортированы в папку 'out'!" << endl;
    }

    return success;
}