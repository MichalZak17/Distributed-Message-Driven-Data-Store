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
- Exposes gRPC or RESTful API for CRUD operations:
  - PUT, GET, DELETE, SCAN.

### Monitoring
- Metrics for:
  - Kafka consumer lag.
  - PostgreSQL query performance.
- Logging and tracing for debugging distributed transactions.

---

## Contribution
Contributions are welcome! Please submit a pull request or open an issue for feedback and improvements.

---

## License
This project is licensed under the MIT License. See `LICENSE` for more details.
