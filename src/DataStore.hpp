#pragma once

#include <string>
#include <unordered_map>
#include <mutex>

// Simple thread-safe in-memory key-value store.
class DataStore {
public:
    bool put(const std::string& key, const std::string& value);
    bool del(const std::string& key);
    std::string get(const std::string& key, bool& found);
    // For demonstration, returns all keys (SCAN).
    std::unordered_map<std::string, std::string> scan();

private:
    std::unordered_map<std::string, std::string> store_;
    std::mutex mtx_;
};
