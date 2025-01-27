# Distributed Message-Driven Data Store

## Overview
Distributed Message-Driven Data Store is a key-value store implemented in C++ using Apache Kafka for message brokering and PostgreSQL for data persistence. The project supports distributed transactions, replication, and fault tolerance, making it a robust solution for scalable data storage.

---

## Key Features

### Kafka Integration
- **Message Brokering:** Kafka as the backbone for communication between distributed nodes.
- **Topics:**
  - Write operations: Updates (insert/update/delete) published to a Kafka topic.
  - Replication: Distributes updates to other nodes via Kafka.
  - Failure recovery: Consumes logs to restore state on crashes.

### PostgreSQL Integration
- **Persistent Storage:** Stores key-value pairs with transaction support to ensure ACID compliance.
- **Advanced Queries:** Leverages PostgreSQL indexing for efficient range and complex queries.

### Distributed Features
- **Partitioning:** Consistent hashing for distributing keys across nodes.
- **Replication:** Ensures fault tolerance by replicating updates across nodes.
- **Leader Election:** Uses Kafka with ZooKeeper or Raft for coordination.
- **Fault Tolerance:** Replays Kafka logs to restore state after node failure.

### API
- Exposes a RESTful API for CRUD operations:
  - `PUT /key/<key>?value=<value>`: Insert or update a key-value pair.
  - `GET /key/<key>`: Retrieve the value for a key.
  - `DELETE /key/<key>`: Delete a key-value pair.
  - `GET /scan`: Retrieve all key-value pairs in JSON format.

### Monitoring
- Metrics for:
  - Kafka consumer lag.
  - PostgreSQL query performance.
- Logging and tracing for debugging distributed transactions.

---

## Requirements

### Dependencies

Ensure the following dependencies are installed on your system:
- C++17 compiler
- CMake
- librdkafka and librdkafka++
- libpqxx and PostgreSQL
- Crow (included as a single-header library)
- Asio (networking library for Crow)

#### Install Required Libraries (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install -y libasio-dev libpqxx-dev librdkafka-dev librdkafka++-dev cmake g++
```

#### Install Crow (Header-Only)
Download the Crow single-header file:
```bash
wget https://github.com/CrowCpp/Crow/releases/latest/download/crow_all.h -O src/crow_all.h
```

---

## Build and Run

### Clone the Repository
```bash
git clone https://github.com/MichalZak17/Distributed-Message-Driven-Data-Store.git
cd Distributed-Message-Driven-Data-Store
```

### Build the Project
```bash
mkdir build
cd build
cmake ..
make
```

### Run the Application
Ensure Kafka and PostgreSQL are running on your system:

- **Start Kafka:**
  ```bash
  kafka-server-start.sh /path/to/server.properties
  ```
- **Start PostgreSQL:**
  Ensure the database and credentials in the code match your setup.

Run the application:
```bash
./kafka_postgres_app
```

---

## Usage

### RESTful API Endpoints

#### **PUT /key/<key>?value=<value>**
Insert or update a key-value pair.
```bash
curl -X PUT "http://localhost:8080/key/foo?value=bar"
```

#### **GET /key/<key>**
Retrieve the value for a key.
```bash
curl "http://localhost:8080/key/foo"
```

#### **DELETE /key/<key>**
Delete a key-value pair.
```bash
curl -X DELETE "http://localhost:8080/key/foo"
```

#### **GET /scan**
Retrieve all key-value pairs in JSON format.
```bash
curl "http://localhost:8080/scan"
```

---

## Contribution

Contributions are welcome! To contribute:
1. Fork the repository.
2. Create a feature branch (`git checkout -b feature-name`).
3. Commit your changes (`git commit -m 'Add new feature'`).
4. Push to the branch (`git push origin feature-name`).
5. Create a pull request.

---

## License
This project is licensed under the MIT License. See `LICENSE` for more details.
