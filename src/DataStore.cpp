#include "DataStore.hpp"

bool DataStore::put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mtx_);
    store_[key] = value;
    return true;
}

bool DataStore::del(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx_);
    return store_.erase(key) > 0;
}

std::string DataStore::get(const std::string& key, bool& found) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = store_.find(key);
    if (it != store_.end()) {
        found = true;
        return it->second;
    }
    found = false;
    return "";
}

std::unordered_map<std::string, std::string> DataStore::scan() {
    std::lock_guard<std::mutex> lock(mtx_);
    return store_; // Shallow copy
}
