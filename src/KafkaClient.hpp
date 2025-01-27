#pragma once

#include <string>
#include <librdkafka/rdkafkacpp.h>

// A simple wrapper for Kafka producer/consumer logic.
class KafkaClient {
public:
    KafkaClient(const std::string& brokers, const std::string& topic);
    ~KafkaClient();

    // Produce message to the topic.
    bool produce(const std::string& key, const std::string& value);

    // Start consuming updates for replication/fault tolerance.
    bool startConsumer(const std::string& group_id);
    // Poll for messages and process them (call applyUpdate).
    void pollMessages();

    // Graceful shutdown.
    void close();

    // Called when a message is consumed - integrate with local DataStore
    // or Postgres, for example.
    void applyUpdate(const std::string& key, const std::string& value);

private:
    std::string brokers_;
    std::string topic_;

    RdKafka::Producer* producer_ = nullptr;
    RdKafka::Topic* producerTopic_ = nullptr;

    RdKafka::KafkaConsumer* consumer_ = nullptr;

    RdKafka::Conf* conf_ = nullptr;
    RdKafka::Conf* tconf_ = nullptr;
};
