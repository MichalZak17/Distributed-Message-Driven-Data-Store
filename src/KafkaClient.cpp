#include "KafkaClient.hpp"
#include <iostream>
#include <memory>


// Example event callback
class ExampleEventCb : public RdKafka::EventCb {
public:
    void event_cb(RdKafka::Event &event) override {
        switch (event.type()) {
            case RdKafka::Event::EVENT_ERROR:
                std::cerr << "[Kafka] ERROR: " << RdKafka::err2str(event.err())
                          << " - " << event.str() << std::endl;
                break;
            case RdKafka::Event::EVENT_STATS:
                std::cerr << "[Kafka] STATS: " << event.str() << std::endl;
                break;
            case RdKafka::Event::EVENT_LOG:
                std::cerr << "[Kafka] LOG-"
                          << event.severity() << "-"
                          << event.fac() << ": "
                          << event.str() << std::endl;
                break;
            default:
                std::cerr << "[Kafka] EVENT " << event.type() << " ("
                          << RdKafka::err2str(event.err()) << "): "
                          << event.str() << std::endl;
                break;
        }
    }
};

KafkaClient::KafkaClient(const std::string& brokers, const std::string& topic)
    : brokers_(brokers), topic_(topic) {

    std::string errstr;

    conf_ = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    tconf_ = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    // Set the broker list
    if (conf_->set("bootstrap.servers", brokers_, errstr) != RdKafka::Conf::CONF_OK) {
        std::cerr << "[Kafka] Conf error: " << errstr << std::endl;
    }

    // Set event callback
    ExampleEventCb* eventCb = new ExampleEventCb();
    if (conf_->set("event_cb", eventCb, errstr) != RdKafka::Conf::CONF_OK) {
        std::cerr << "[Kafka] EventCb error: " << errstr << std::endl;
    }

    // Create producer
    producer_ = RdKafka::Producer::create(conf_, errstr);
    if (!producer_) {
        std::cerr << "[Kafka] Failed to create producer: " << errstr << std::endl;
    }

    producerTopic_ = RdKafka::Topic::create(producer_, topic_, tconf_, errstr);
    if (!producerTopic_) {
        std::cerr << "[Kafka] Failed to create topic: " << errstr << std::endl;
    }
}

KafkaClient::~KafkaClient() {
    close();
}

bool KafkaClient::produce(const std::string& key, const std::string& value) {
    if (!producer_ || !producerTopic_) return false;

    RdKafka::ErrorCode resp = producer_->produce(
        producerTopic_,
        RdKafka::Topic::PARTITION_UA,
        RdKafka::Producer::RK_MSG_COPY,
        const_cast<char*>(value.data()),
        value.size(),
        &key, // key
        0,    // timestamp
        nullptr
    );

    if (resp != RdKafka::ERR_NO_ERROR) {
        std::cerr << "[Kafka] Produce failed: "
                  << RdKafka::err2str(resp) << std::endl;
        return false;
    }

    producer_->poll(0); // Poll to handle delivery reports
    return true;
}

bool KafkaClient::startConsumer(const std::string& group_id) {
    std::string errstr;

    // Create separate config for consumer
    RdKafka::Conf* consumer_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

    if (consumer_conf->set("bootstrap.servers", brokers_, errstr) != RdKafka::Conf::CONF_OK) {
        std::cerr << "[Kafka] Consumer conf error: " << errstr << std::endl;
        return false;
    }

    if (consumer_conf->set("group.id", group_id, errstr) != RdKafka::Conf::CONF_OK) {
        std::cerr << "[Kafka] group.id error: " << errstr << std::endl;
        return false;
    }

    consumer_ = RdKafka::KafkaConsumer::create(consumer_conf, errstr);
    if (!consumer_) {
        std::cerr << "[Kafka] Failed to create consumer: " << errstr << std::endl;
        return false;
    }

    // Subscribe to topic
    std::vector<std::string> topics = { topic_ };
    RdKafka::ErrorCode err = consumer_->subscribe(topics);
    if (err) {
        std::cerr << "[Kafka] Subscribe failed: " << RdKafka::err2str(err) << std::endl;
        return false;
    }

    return true;
}

void KafkaClient::pollMessages() {
    if (!consumer_) {
        std::cerr << "[Kafka] Consumer not initialized!" << std::endl;
        return;
    }

    // Poll for messages (timeout in milliseconds)
    std::unique_ptr<RdKafka::Message> msg(consumer_->consume(1000));

    if (msg->err() == RdKafka::ERR_NO_ERROR) {
        // Successfully got a message
        std::string key = (msg->key()) ? *msg->key() : "";
        std::string value(static_cast<const char*>(msg->payload()), msg->len());

        // Apply the update to the system (e.g., in-memory DataStore or PostgreSQL)
        applyUpdate(key, value);
    } else if (msg->err() != RdKafka::ERR__TIMED_OUT) {
        // Handle other errors (e.g., connection issues)
        std::cerr << "[Kafka] Consumer error: " << msg->errstr() << std::endl;
    }
}


void KafkaClient::applyUpdate(const std::string& key, const std::string& value) {
    // TODO: Integrate this with the DataStore or Postgres logic
    // e.g., store->put(key, value) or Postgres insert
    std::cout << "[Kafka] Received update => key: " << key << ", value: " << value << std::endl;
}

void KafkaClient::close() {
    if (producer_) {
        producer_->flush(1000); // flush any outstanding messages
        delete producer_;
        producer_ = nullptr;
    }
    if (producerTopic_) {
        delete producerTopic_;
        producerTopic_ = nullptr;
    }
    if (consumer_) {
        consumer_->close();
        delete consumer_;
        consumer_ = nullptr;
    }
    if (conf_) {
        delete conf_;
        conf_ = nullptr;
    }
    if (tconf_) {
        delete tconf_;
        tconf_ = nullptr;
    }
}
