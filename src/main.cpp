#include <iostream>
#include <thread>
#include <chrono>

#include "DataStore.hpp"
#include "KafkaClient.hpp"
#include "PostgresClient.hpp"

// If using Crow as a single-header library, just #include "crow_all.h"
#include "crow_all.h"


int main() {
    // 1. Initialize Kafka
    std::string brokers = "localhost:9092";
    std::string topic = "distributed_kv_store";
    KafkaClient kafkaClient(brokers, topic);

    // Start consumer for replication
    kafkaClient.startConsumer("distributed_group");

    // 2. Initialize Postgres
    PostgresClient pgClient("dbname=test user=postgres password=secret host=localhost");
    pgClient.initTable("kv_store");

    // 3. Initialize DataStore
    DataStore dataStore;

    // 4. Launch a thread to poll Kafka messages (replication/fault tolerance)
    std::thread consumerThread([&]() {
        while (true) {
            kafkaClient.pollMessages();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });

    // 5. Create a Crow HTTP server for CRUD
    crow::SimpleApp app;

    // PUT /key/<key>?value=<value>
    CROW_ROUTE(app, "/key/<string>").methods(crow::HTTPMethod::Put)([&](const crow::request& req, crow::response& res, std::string key){
        auto value = req.url_params.get("value");
        if (!value) {
            res.code = 400;
            res.write("Missing 'value' parameter");
            res.end();
            return;
        }

        // 5a. Update DataStore
        dataStore.put(key, value);
        // 5b. Produce to Kafka
        kafkaClient.produce(key, value);
        // 5c. Insert/Update Postgres
        pgClient.put("kv_store", key, value);

        res.write("PUT success");
        res.end();
    });

    // GET /key/<key>
    CROW_ROUTE(app, "/key/<string>").methods(crow::HTTPMethod::Get)([&](const crow::request& req, crow::response& res, std::string key){
        bool found = false;
        auto val = dataStore.get(key, found);
        if (!found) {
            // If not in memory, try Postgres
            if (pgClient.get("kv_store", key, val)) {
                // Sync with memory for faster subsequent access
                dataStore.put(key, val);
                found = true;
            }
        }
        if (!found) {
            res.code = 404;
            res.write("Key not found");
        } else {
            res.write(val);
        }
        res.end();
    });

    // DELETE /key/<key>
    CROW_ROUTE(app, "/key/<string>").methods(crow::HTTPMethod::Delete)([&](const crow::request& req, crow::response& res, std::string key){
        dataStore.del(key);
        kafkaClient.produce(key, "DELETE"); // Indicate a delete
        pgClient.del("kv_store", key);

        res.write("DELETE success");
        res.end();
    });

    // SCAN /scan
    // Return all keys in JSON (not recommended for large data sets!)
    CROW_ROUTE(app, "/scan").methods(crow::HTTPMethod::Get)([&](const crow::request& req, crow::response& res){
        auto map = dataStore.scan();
        crow::json::wvalue result;
        for (auto& kv : map) {
            result[kv.first] = kv.second;
        }
        res.write(result.dump());
        res.end();
    });

    // Run Crow server on port 8080
    app.port(8080).multithreaded().run();

    consumerThread.join();
    return 0;
}
