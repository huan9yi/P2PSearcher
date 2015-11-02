#include "sqlite_manager.h"

SQLiteManager::~SQLiteManager()
{
	Close();
}

int SQLiteManager::Open()
{
	int rc = sqlite3_open(filename, &db);
	if (rc == SQLITE_OK){
		CreateTable();
	}
	else{
		Close();
	}

	return rc;
}

void SQLiteManager::Close()
{
	if (db){
		sqlite3_close(db);
	}
}

void SQLiteManager::CreateTable()
{
	bool table_exists = false;
	std::string check_table_exist_sql = std::string("SELECT COUNT(*) FROM sqlite_master WHERE type='table' and name='").append(tablename).append("'");

	sqlite3_exec(db, check_table_exist_sql.c_str(), CheckTableExistsCallback, &table_exists, NULL);
	if (!table_exists){
		std::string create_table_sql = std::string("CREATE TABLE ").append(tablename);
		create_table_sql.append("(");
		create_table_sql.append("info_hash char(20) PRIMARY KEY,");
		create_table_sql.append("name varchar(255) default(''),");
		create_table_sql.append("type varchar(255) default(''),");
		create_table_sql.append("size varchar(255) default(''),");
		create_table_sql.append("hot int default(1),");
		create_table_sql.append("url varchar default('')");
		create_table_sql.append(")");
		std::string create_index_sql = std::string("CREATE INDEX index_info_hash ON ").append(tablename).append("(info_hash ASC)");

		ExecuteSQL(create_table_sql.c_str());
		ExecuteSQL(create_index_sql.c_str());
	}
}

int CheckTableExistsCallback(void *user_data, int argc, char **argv, char **azColName){
	bool *exist = (bool *)user_data;
	if (argc > 0 && std::stoi(argv[0]) > 0){
		*exist = true;
	}

	// if return non-zero, sqlite3_exec() absorts.
	return 1;
}

void SQLiteManager::ExecuteSQL(const char *sql)
{
	sqlite3_exec(db, sql, NULL, NULL, NULL);
}

void SQLiteManager::Update(const char *info_hash, char *name, char *type, char *size, int hot, char *url)
{
	std::string update_sql = std::string("REPLACE INTO ").append(tablename).append("(info_hash, name, type, size, hot, url) values(")
		.append("'").append(info_hash).append("', ")
		.append("'").append(name).append("', ")
		.append("'").append(type).append("', ")
		.append("'").append(size).append("', ")
		.append(std::to_string(hot)).append(", ")
		.append("'").append(url).append("')");
	ExecuteSQL(update_sql.c_str());
}