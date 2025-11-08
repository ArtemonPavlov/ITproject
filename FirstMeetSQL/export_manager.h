#pragma once

#include "sqlite/sqlite3.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

class ExportManager {
public:
    static bool exportTableToAllFormats(sqlite3* db, const string& tableName);

private:
    static bool ensureOutputDirectory();
    static vector<map<string, string>> getTableData(sqlite3* db, const string& tableName);
    static vector<string> getTableColumns(sqlite3* db, const string& tableName);

    static bool exportToJSON(const vector<map<string, string>>& data,
        const string& filename);
    static bool exportToCSV(const vector<map<string, string>>& data,
        const vector<string>& columns,
        const string& filename);
    static bool exportToXML(const vector<map<string, string>>& data,
        const string& tableName,
        const string& filename);
    static bool exportToYAML(const vector<map<string, string>>& data,
        const string& filename);

    static string escapeJSON(const string& input);
    static string escapeCSV(const string& input);
    static string escapeXML(const string& input);
};