FROM ubuntu:20.04

RUN apt-get update && apt-get install -y \
    g++ \
    libboost-all-dev \
    libssl-dev \
    libcpprest-dev \
    libgtest-dev \
    cmake \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY blockchain_node.cpp .
COPY blockchain_test.cpp .

# Компиляция основного приложения
RUN g++ -o blockchain blockchain_node.cpp -lboost_system -pthread -lssl -lcrypto -lcpprest

# Компиляция тестов
RUN cmake -S . -B build && cmake --build build && mv build/blockchain_test .

CMD ["./blockchain", "12345", "8080"]