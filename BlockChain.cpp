// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <ctime> //Functions to manipulate date time objects.
#include <vector> //Data structure similar to an array but there is no need to allocate memory at design time.
//The size of the vector can be increased at runtime.
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

//Need Transaction data. Transaction data goes into the block class and every block would have multiple transactions but in this case we just have
//one transaction per block. 
struct TransactionData {
	double amount;
	string senderKey;
	string receiverKey;
	time_t timestamp;
};
// Block class 
class Block
{
private:
	int index;
	size_t blockHash; //size_t is an unsigned integer type and this is what get returned when you use standard library hash function.
	size_t previousHash;
	size_t generateHash();

public:
	//Constructor
	Block(int idx, TransactionData d, size_t prevHash);
	//Get original Hash
	size_t getHash();
	//Get prev Hash
	size_t getPreviousHash(); // To compare later in case if the TransactionData is maliciously modified.
							  //Transaction data member
	TransactionData data;
	//Validate Hash
	bool isHashValid();
};

//Constructor with parameters
Block::Block(int idx, TransactionData d, size_t prevHash)
{
	index = idx;
	data = d;
	previousHash = prevHash;
	blockHash = generateHash();
}

//private functions
size_t Block::generateHash()
{
	hash<string> hash1;
	hash<size_t> hash2;
	hash<size_t> finalHash;
	string toHash = to_string(data.amount) + data.receiverKey + data.senderKey + to_string(data.timestamp);

	return finalHash(hash1(toHash) + hash2(previousHash));
}

//public functions
size_t Block::getHash()
{
	return blockHash;
}


size_t Block::getPreviousHash()
{
	return previousHash;
}

bool Block::isHashValid()
{
	return generateHash() == blockHash;
}


// Block chain class. This will contain every block. 
class BlockChain
{
private:
	Block createGenesisBlock();
public:
	//Public chain
	vector<Block> chain;
	//Constructor
	BlockChain();

	//Public Functions
	void addBlock(TransactionData data);
	bool isChainValid();

	Block *getLatestBlock(); //This is a pointer so that we can manipulate the data inside 

};


//Blockchain constructor
BlockChain::BlockChain()
{
	Block genesis = createGenesisBlock();
	chain.push_back(genesis);

}

//create Genesis Block is the first genesis block, so it doesn't need anything special as far as data.
Block BlockChain::createGenesisBlock()
{
	time_t current;
	TransactionData d;
	d.amount = 0;
	d.receiverKey = "None";
	d.senderKey = "None";
	d.timestamp = time(&current);

	hash<int> hash1;
	Block genesis(0, d, hash1(0));

	return genesis;
}

//This is not a good practice. Ordinarily, it needs to be done without a pointer, and we should return a block of the chain. But using the
// pointer we can manipulate the data inside that address. 
Block *BlockChain::getLatestBlock()
{
	return &chain.back();
}

void BlockChain::addBlock(TransactionData d)
{
	int index = (int)chain.size() - 1; //This would return the index of the 1st block
	Block newBlock(index, d, getLatestBlock()->getHash()); //Create a new block with the new index, data d, and the has value of the latest block


}

bool BlockChain::isChainValid()
{
	vector<Block>::iterator it; //an iterator that iterates over the vector of blocks.
	int chainLen = (int)chain.size();

	for (it = chain.begin(); it != chain.end(); ++it) {
		Block currentBlock = *it;
		if (!currentBlock.isHashValid()) {
			return false; //If we return false, it means, someone hacked into the chain.
		}
		if (chainLen > 1) {
			Block previousBlock = *(it - 1); //Check if previousHash equals the one in the current block.
			if (currentBlock.getPreviousHash() != previousBlock.getHash()) {
				return false; //If we return false, it means, someone hacked into the chain and modified the transaction data in the block.
			}
		}
	}
}

int main() {
	//start a blockchain 
	BlockChain AwesomeCoin;

	//Data for first added block
	TransactionData data1;
	time_t data1Time;
	data1.amount = 1.5;
	data1.receiverKey = "Joe";
	data1.senderKey = "Sally";
	data1.timestamp = time(&data1Time);

	AwesomeCoin.addBlock(data1);

	cout << "Is chain valid?" << endl
		<< AwesomeCoin.isChainValid() << endl;
}
