#ifndef _SQLITE_MANAGER_H
#define _SQLITE_MANAGER_H

#include "stdafx.h"

#include "sqlite3.h"

class SQLiteManager{
public:
	~SQLiteManager();
	int Open();
	void Close();
	void CreateTable();
	void ExecuteSQL(const char *sql);
	void Update(const char *info_hash, char *name, char *type, char *size, int hot, char *url);

	sqlite3 *db = nullptr;
	char *filename = "p2psearch.db";
	char *tablename = "resource";
};

int CheckTableExistsCallback(void *user_data, int argc, char **argv, char **azColName);

#endif