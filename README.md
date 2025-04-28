Blockchain
This repository contains a basic implementation of a blockchain in Python, designed as a learning tool to understand the core concepts of blockchain technology. It demonstrates how blocks are created, chained, and validated using cryptographic hashing, with a simple proof-of-work mechanism.
Features

Block Structure: Each block contains an index, timestamp, transactions, proof, and a hash of the previous block.
Proof-of-Work: A simple consensus algorithm requiring miners to find a nonce that produces a hash with a specified number of leading zeros.
Decentralized Network: Basic node registration and consensus algorithm to ensure all nodes agree on the same chain.
HTTP Interface: Uses Flask to provide RESTful endpoints for interacting with the blockchain (e.g., adding transactions, mining blocks, and retrieving the chain).
ERC-20 Compatibility: The blockchain supports token transactions compatible with ERC-20 standards, allowing integration with standard wallets.

Prerequisites

Python 3.6 or higher
pip for installing dependencies
An HTTP client like Postman or curl for testing endpoints

Installation

Clone the repository:git clone https://github.com/Trytonottry/Blockchain.git
cd Blockchain


Install the required Python packages:pip install -r requirements.txt

Ensure you have the following dependencies:
Flask
requests



Usage

Run the Flask server to start a blockchain node:
python app.py

The server will run on http://localhost:5000 by default.

Interact with the blockchain using the following endpoints:

GET /chain: Retrieve the full blockchain.
POST /transactions/new: Add a new transaction. Example payload:{
  "sender": "address1",
  "recipient": "address2",
  "amount": 5
}


GET /mine: Mine a new block by solving the proof-of-work puzzle.
POST /nodes/register: Register new nodes. Example payload:{
  "nodes": ["http://localhost:5001", "http://localhost:5002"]
}


GET /nodes/resolve: Run the consensus algorithm to ensure the node has the longest valid chain.


Test the endpoints using an HTTP client like Postman or curl. For example:
curl -X GET http://localhost:5000/chain



Project Structure

app.py: Main Flask application with blockchain logic and HTTP endpoints.
blockchain.py: Core blockchain class implementing block creation, proof-of-work, and consensus.
requirements.txt: List of Python dependencies.

How It Works

Blocks: Each block is a record containing transactions, a timestamp, and a proof. Blocks are linked via hashes, ensuring immutability.
Proof-of-Work: Miners must find a nonce that, when hashed with the block’s data, produces a hash with a specified number of leading zeros.
Consensus: Nodes resolve conflicts by adopting the longest valid chain in the network, ensuring all nodes agree on the blockchain state.
Decentralization: Nodes communicate via HTTP to share transactions, blocks, and node lists, simulating a decentralized network.

Contributing
Contributions are welcome! Please follow these steps:

Fork the repository.
Create a new branch (git checkout -b feature/your-feature).
Commit your changes (git commit -m "Add your feature").
Push to the branch (git push origin feature/your-feature).
Open a pull request.

License
This project is licensed under the MIT License. See the LICENSE file for details.
Acknowledgments

Inspired by the need for clear, functional blockchain examples.
Built with reference to standard blockchain concepts and ERC-20 token standards.

