#pragma once
#include <vector>
#include <mutex>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;
class primeList
{
public:
	//our links for the linked list. used a primitive and a pointer in a struct
	//to ensure that I am using as little space per link as possible, since 
	//I expect the list to get really really long, and that will take up a ton
	//of space
	struct link {
		unsigned long long int prime;
		link* next;
	};
	//public methods
	primeList(int threads, unsigned long long int limit);
	void insertFound(link* found);
	unsigned long long int getCandidate();
	unsigned long long int getLargestReady();
	void removeCandidate(const unsigned long long int candidate);
	bool isSmallestCandidate(const unsigned long long int candidate);
	link* getFirst();
	string toString();
	~primeList();
private:
	ofstream output;
	link* first = nullptr;
	link* last = nullptr;
	link* current = nullptr;
	link* previous = nullptr;
	link* largestFoundReady = nullptr;
	//64 bit integer primitive for the limit
	unsigned long long int primeLimit;
	//the number found.
	int numberFound = 1;
	//seeding the next candidate with 3 (the first odd number after 2, the first prime)
	unsigned long long int nextCandidate = static_cast<unsigned long long int>(3);
	//vector to hold the candidates currently being searched.
	//vector was chosen over array because vector already has search and remove
	//built in, and I didn't want to write it for an array
	vector<unsigned long long int> currentCandidates;
	//our locks
	mutex vectorMutex;
	mutex endLinkMutex;
	
	void indexLargestFound();
};

