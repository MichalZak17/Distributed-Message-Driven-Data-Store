#include "PostgresClient.hpp"
#include <iostream>

PostgresClient::PostgresClient(const std::string& conn_str)
    : conn_str_(conn_str) {
}

void PostgresClient::initTable(const std::string& table_name) {
    try {
        pqxx::connection conn(conn_str_);
        pqxx::work txn(conn);

        std::string create_sql = "CREATE TABLE IF NOT EXISTS " + table_name + R"(
            (
                id SERIAL PRIMARY KEY,
                k TEXT UNIQUE,
                v TEXT,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        )";

        txn.exec0(create_sql);
        txn.commit();
    }
    catch(const std::exception& e) {
        std::cerr << "[Postgres] initTable error: " << e.what() << std::endl;
    }
}

bool PostgresClient::put(const std::string& table_name, const std::string& key, const std::string& value) {
    try {
        pqxx::connection conn(conn_str_);
        pqxx::work txn(conn);

        // Upsert approach
        std::string upsert_sql =
            "INSERT INTO " + table_name + " (k, v) VALUES (" +
            txn.quote(key) + ", " + txn.quote(value) +
            ") ON CONFLICT (k) DO UPDATE SET v = EXCLUDED.v;";

        txn.exec0(upsert_sql);
        txn.commit();
        return true;
    }
    catch(const std::exception& e) {
        std::cerr << "[Postgres] put error: " << e.what() << std::endl;
        return false;
    }
}

bool PostgresClient::get(const std::string& table_name, const std::string& key, std::string& value) {
    try {
        pqxx::connection conn(conn_str_);
        pqxx::work txn(conn);

        std::string query_sql =
            "SELECT v FROM " + table_name + " WHERE k = " + txn.quote(key) + " LIMIT 1;";
        pqxx::result r = txn.exec(query_sql);

        if(!r.empty()) {
            value = r[0][0].as<std::string>();
            return true;
        }
        return false;
    }
    catch(const std::exception& e) {
        std::cerr << "[Postgres] get error: " << e.what() << std::endl;
        return false;
    }
}

bool PostgresClient::del(const std::string& table_name, const std::string& key) {
    try {
        pqxx::connection conn(conn_str_);
        pqxx::work txn(conn);

        std::string del_sql =
            "DELETE FROM " + table_name + " WHERE k = " + txn.quote(key) + ";";

        pqxx::result r = txn.exec(del_sql);
        txn.commit();
        return (r.affected_rows() > 0);
    }
    catch(const std::exception& e) {
        std::cerr << "[Postgres] del error: " << e.what() << std::endl;
        return false;
    }
}
