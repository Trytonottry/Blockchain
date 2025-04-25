// Требования: Boost.Asio, OpenSSL, cpprestsdk, Google Test
// Компиляция: см. Dockerfile
#include <boost/asio.hpp>
#include <openssl/sha.h>
#include <cpprest/http_listener.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>

using boost::asio::ip::tcp;
using web::http::http_request;
using web::http::methods;
using web::http::status_codes;
using nlohmann::json;

// Класс блока
class Block {
public:
    std::string hash;
    std::string prev_hash;
    std::string data;
    uint64_t nonce;
    uint64_t timestamp;
    Block(const std::string& d, const std::string& ph) : data(d), prev_hash(ph), nonce(0), timestamp(time(nullptr)) {
        hash = calculateHash();
    }

    std::string calculateHash() const {
        std::stringstream ss;
        ss << prev_hash << data << nonce << timestamp;
        std::string input = ss.str();
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256((unsigned char*)input.c_str(), input.length(), hash);
        std::stringstream hex;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            hex << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return hex.str();
    }

    void mine(uint32_t difficulty) {
        std::string target(difficulty, '0');
        while (hash.substr(0, difficulty) != target) {
            nonce++;
            hash = calculateHash();
        }
    }

    json toJson() const {
        return {{"hash", hash}, {"prev_hash", prev_hash}, {"data", data}, {"nonce", nonce}, {"timestamp", timestamp}};
    }
};

// Класс блокчейна
class Blockchain {
public:
    Blockchain(uint32_t difficulty = 4) : difficulty_(difficulty) {
        blocks.emplace_back("Genesis Block", "");
        blocks.back().mine(difficulty_);
    }

    void addBlock(const std::string& data) {
        Block new_block(data, blocks.back().hash);
        new_block.mine(difficulty_);
        blocks.push_back(new_block);
    }

    bool isValid() const {
        for (size_t i = 1; i < blocks.size(); ++i) {
            if (blocks[i].prev_hash != blocks[i-1].hash) return false;
            if (blocks[i].hash != blocks[i].calculateHash()) return false;
        }
        return true;
    }

    json getChain() const {
        json chain = json::array();
        for (const auto& block : blocks) chain.push_back(block.toJson());
        return chain;
    }

private:
    std::vector<Block> blocks;
    uint32_t difficulty_;
};

// P2P нода
class P2PNode {
public:
    P2PNode(boost::asio::io_context& io_context, short port, Blockchain& bc)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), blockchain_(bc) {
        startAccept();
    }

    void broadcastBlock(const Block& block) {
        json msg = {{"type", "new_block"}, {"block", block.toJson()}};
        broadcast(msg.dump());
    }

private:
    void broadcast(const std::string& message) {
        std::cout << "Broadcast: " << message << std::endl;
    }

    void startAccept() {
        socket_ = std::make_shared<tcp::socket>(acceptor_.get_executor().context());
        acceptor_.async_accept(*socket_,
            [this](const boost::system::error_code& error) {
                if (!error) std::cout << "Peer connected\n";
                startAccept();
            });
    }

    tcp::acceptor acceptor_;
    std::shared_ptr<tcp::socket> socket_;
    Blockchain& blockchain_;
};

// REST API
class RestServer {
public:
    RestServer(const std::string& address, uint16_t port, Blockchain& bc, P2PNode& node)
        : listener_(address + ":" + std::to_string(port) + "/api"), blockchain_(bc), p2p_node_(node) {
        listener_.support(methods::GET, [this](http_request req) { handleGet(req); });
        listener_.support(methods::POST, [this](http_request req) { handlePost(req); });
        listener_.open().wait();
    }

private:
    void handleGet(http_request req) {
        if (req.request_uri().path() == "/api/chain") {
            req.reply(status_codes::OK, blockchain_.getChain().dump());
        } else {
            req.reply(status_codes::NotFound, "Not found");
        }
    }

    void handlePost(http_request req) {
        if (req.request_uri().path() == "/api/block") {
            req.extract_json().then([this, req](json value) {
                std::string data = value["data"].get<std::string>();
                blockchain_.addBlock(data);
                p2p_node_.broadcastBlock(blockchain_.getChain().back());
                req.reply(status_codes::OK, "Block added");
            }).wait();
        } else {
            req.reply(status_codes::NotFound, "Not found");
        }
    }

    web::http::experimental::listener::http_listener listener_;
    Blockchain& blockchain_;
    P2PNode& p2p_node_;
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <rest_port>\n";
        return 1;
    }
    try {
        boost::asio::io_context io_context;
        Blockchain blockchain;
        P2PNode node(io_context, std::atoi(argv[1]), blockchain);
        RestServer server("http://0.0.0.0", std::atoi(argv[2]), blockchain, node);
        blockchain.addBlock("Block 1");
        node.broadcastBlock(blockchain.getChain().back());
        std::cout << "Node running on port " << argv[1] << ", REST API on port " << argv[2] << "\n";
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}