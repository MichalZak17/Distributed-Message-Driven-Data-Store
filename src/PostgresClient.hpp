#pragma once

#include <string>
#include <pqxx/pqxx>

class PostgresClient {
public:
    // Adjust connection string accordingly
    PostgresClient(const std::string& conn_str = "dbname=test user=postgres password=secret host=localhost");
    ~PostgresClient() = default;

    // Creates table if doesn't exist
    void initTable(const std::string& table_name);

    // Insert or update a key-value pair
    bool put(const std::string& table_name, const std::string& key, const std::string& value);

    // Get a value by key
    bool get(const std::string& table_name, const std::string& key, std::string& value);

    // Delete a value by key
    bool del(const std::string& table_name, const std::string& key);

private:
    std::string conn_str_;
};
