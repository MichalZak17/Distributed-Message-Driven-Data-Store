cmake_minimum_required(VERSION 3.10)
project(DistributedMessageDrivenDataStore CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Find and link librdkafka
find_package(PkgConfig REQUIRED)
pkg_check_modules(RDKAFKA REQUIRED rdkafka++)

# Find and link libpqxx
find_package(PostgreSQL REQUIRED)
find_package(Threads REQUIRED)

include_directories(${RDKAFKA_INCLUDE_DIRS})
include_directories(${PostgreSQL_INCLUDE_DIRS})

# Add source files
set(SOURCES
    src/main.cpp
    src/DataStore.cpp
    src/KafkaClient.cpp
    src/PostgresClient.cpp
)

add_executable(kafka_postgres_app ${SOURCES})

set(CMAKE_POSITION_INDEPENDENT_CODE OFF)


target_compile_definitions(kafka_postgres_app PRIVATE CROW_ENABLE_STATIC_FILE_INFO)


target_link_libraries(kafka_postgres_app
    ${RDKAFKA_LIBRARIES}
    pqxx
    pq
    Threads::Threads
)
