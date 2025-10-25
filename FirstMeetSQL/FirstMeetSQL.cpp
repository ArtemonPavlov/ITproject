#include "sqlite/sqlite3.h"
#include <string>
#include <iostream>
#include <Windows.h>

using namespace std;

// Функция для проверки, есть ли данные в таблице
bool is_table_empty(sqlite3* db) {
	sqlite3_stmt* stmt;
	const char* sql = "SELECT COUNT(*) FROM GenOfRayon;";
	int count = 0;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			count = sqlite3_column_int(stmt, 0);
		}
		sqlite3_finalize(stmt);
	}
	return count == 0;
}

int notmain(){
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	sqlite3* db; //указатель на бд
	sqlite3_stmt* stmt;
	char* err;
	sqlite3_open("myDB.db", &db);

	//Работаем с базой здесь
	const char* create_sql = "CREATE TABLE IF NOT EXISTS GenOfRayon (Id INTEGER PRIMARY KEY AUTOINCREMENT, Nick TEXT NOT NULL, KubkiInBS INTEGER);";//sql-запрос для создания таблицы
	sqlite3_exec(db, create_sql, NULL, NULL, &err);//выполняем sql-запрос
	
	//sql-зап. для присвоения плейсхолдеров на место Ников и Кубокв
	const char* insert_sql = "INSERT INTO GenOfRayon (Nick, KubkiInBS) VALUES (?, ?);";

	//if (is_table_empty(db)) {

		string new_nick, new_nick2;
		cout << "Добавить нового пользователя: ";
		getline(cin, new_nick);
		cout << "Добавь ещё одного к нему: ";
		getline(cin, new_nick2);

		struct Bazza { string nick; int kubki; }; //создали "шаблон" 
		Bazza users[] = {{new_nick, 7777}, {new_nick2, 8888} }; //используем шаблон для массива users[]

		for (const auto& user : users) {
			sqlite3_prepare_v2(db, insert_sql, -1, &stmt, nullptr); //нужная херня
			sqlite3_bind_text(stmt, 1, user.nick.c_str(), -1, SQLITE_STATIC); //заменяем первый "?" на ник (нумерация плейсхолдеров с 1!!!)
			sqlite3_bind_int(stmt, 2, user.kubki);//заменяем второй "?" на число кубков
			sqlite3_step(stmt); //переходим на следующую строчку в таблице
			sqlite3_finalize(stmt); //нужная херня
		}
	//}

	cout << "------ Чуваки года, вот же они -------\n";
	const char* sel_all = "SELECT * FROM GenOfRayon;"; //выбрать всё из нашей таблицы
	sqlite3_prepare_v2(db, sel_all, -1, &stmt, nullptr);

	while (sqlite3_step(stmt) == SQLITE_ROW) { // sqlite3_step() возвращает SQLITE_ROW, пока есть данные для чтения
		int id = sqlite3_column_int(stmt, 0); //Колонка 0 - id (int); Колонки нумеруются с нуля!!!
		const unsigned char* nick = sqlite3_column_text(stmt, 1);//unsigned - здесь, чтобы не упала кодировка, а так можно юзать для только положительных интов (по типу возраста, цены и т.п) | Колонка 1 - Ник (text)
		int kubasi = sqlite3_column_int(stmt, 2);//Колонка 2 - кубасики (int)
		cout << id << " | " << nick << " | " << kubasi << endl;
	}
	sqlite3_finalize(stmt);//Обязательная нужная херня
	//Закрываем базу
	sqlite3_close(db);
	return 0;
}