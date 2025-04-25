#include <gtest/gtest.h>
#include "blockchain_node.cpp" // Предполагается, что код выше в отдельном файле

TEST(BlockchainTest, BlockHash) {
    Block block("Test data", "0");
    std::string hash = block.calculateHash();
    EXPECT_EQ(hash, block.calculateHash());
}

TEST(BlockchainTest, ChainValidity) {
    Blockchain bc(2);
    bc.addBlock("Block 1");
    EXPECT_TRUE(bc.isValid());
}

TEST(BlockchainTest, InvalidChain) {
    Blockchain bc(2);
    bc.addBlock("Block 1");
    // Манипуляция с данными
    bc.getChain()[1]["data"] = "Tampered";
    EXPECT_FALSE(bc.isValid());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}