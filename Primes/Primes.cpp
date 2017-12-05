// Primes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <math.h>
#include <thread>
#include "primeList.h"
using namespace std;

void primeChecker(primeList* p_list);
char* convertWStringToASCIIString(const wstring input);

int _tmain(int argc, _TCHAR* argv[])
{
	int maxThreads = 0;
	unsigned long long int limit = 0;
	thread* threads = nullptr;
	primeList* p_list = nullptr;
	
	if (argc == 3) {
		//we need the max threads and limit from the arguments
		char* thr;
		char* lim;
		try {
			thr = convertWStringToASCIIString(argv[1]);
			lim = convertWStringToASCIIString(argv[2]);
			maxThreads = atoi(thr);
			if (maxThreads < 1) {
				throw new exception("max threads must be more than 0 and must be numeric!");
			}
			limit = strtoull(lim, NULL, 10);
			if (limit < 1) {
				throw new exception("limit must be more than 0 and must be numeric!");
			}
			if (limit < maxThreads) {
				throw exception("Limit must be more than the number of threads.");
			}
			//the arguments were good, so make a new primelist object, and thread array(pretty much a pool, kinda)
			p_list = new primeList(maxThreads, limit);
			threads = new thread[maxThreads];
			cout << "All set and running..." << endl;
			delete thr;
			delete lim;
		}
		//if the correct number of arguments but incorrect usage occurred
		catch (exception e) {
			cout << e.what() << endl;
			cout << "Correct use of arguments is [max threads] [numeric limit]" << endl;
			cout << "Quitting execution!!" << endl;
			delete thr;
			delete lim;
			return 1;
		}
	}
	//no arguments supplied, run default. mostly for demo purposes
	else if (argc == 1) {
		maxThreads = 4;
		limit = 1000000;
		p_list = new primeList(maxThreads, limit);
		threads = new thread[maxThreads];
		cout << "All set and running default configuration (4 threads, all primes up to 1000000)..." << endl;
	}
	//arguments supplied, but incorrect usage
	else {
		cout << "Correct use of arguments is [max threads] [numeric limit]" << endl;
		cout << "Quitting execution!!" << endl;
		return 1;
	}

	
	
	//start each thread checking candidates
	for (char i = 0; i < maxThreads; ++i) {
		threads[i] = thread(primeChecker, p_list);
	}

	//get the threads joined back together
	for (char i = 0; i < maxThreads; ++i) {
		threads[i].join();
	}

	//output results
	p_list->toString();
	//clean up
	delete p_list;
	delete[] threads;
    return 0;
}

//gotten from Stack Overflow, for going from _TCHAR* to char* for args.
char* convertWStringToASCIIString(const wstring input) {
	char* output;
	size_t outputSize = (input.length() + 1);
	output = new char[outputSize];
	size_t charsConverted = 0;
	const wchar_t* inputW = input.c_str();
	wcstombs_s(&charsConverted, output, outputSize, inputW, input.length());
	return output;
}

//this is the function that does all the work, running until we run out of candidates to check.
//the thread starts and runs this until told to stop (value 0 is returned from the getCandidate() call)
void primeChecker(primeList* p_list) {
	long double squareRoot = static_cast<long double>(0.0);
	unsigned long long int candidate = 0;
	unsigned long long int testValue = 0;
	bool isPrime = true;
	bool notReady = false;
	primeList::link* myLink;
	primeList::link* myCurrent;

	candidate = p_list->getCandidate();
	myCurrent = p_list->getFirst();

	while (candidate > 0) {
		//get the square root. this is expensive, so we store it.
		squareRoot = sqrt(candidate);
		do {
			//make sure that we have enough numbers in the list
			//to actually properly check our number.
			if (squareRoot < p_list->getLargestReady()) {
				//no need to loop back around because we can do all the checking.
				notReady = false;
				//check for primality by dividing by all primes up to the square root
				//of the candidate (prime), or until modulus returns 0 (composite)
				for (; myCurrent->prime <= squareRoot; myCurrent = myCurrent->next) {
					testValue = myCurrent->prime;
					if (candidate % testValue == 0) {
						isPrime = false;
						break;
					}
					else {
						isPrime = true;
					}
				}
			}
			else {
				//loop back around because we weren't ready to check
				notReady = true;
				//give the other threads a little time to add some more links
				//this was a fast and dirty solution. i'm sure
				//I can find something better that isn't time based
				//and may be more efficient. perhaps using a different
				//mutex in the prime list...
				this_thread::sleep_for(chrono::milliseconds(20));
			}
			//if the list was not ready for checking, loop back around to try again.
		} while (notReady);
		//reset our "iterator" to the first element
		myCurrent = p_list->getFirst();
		
		if (isPrime) {
			//make a new link, and add it to the list.
			//remove the candidate from the list of current candidates
			//in a separate step so that we don't have extended locking
			//of the end of the list because we were waiting for the vector lock.
			myLink = new primeList::link();
			myLink->next = nullptr;
			myLink->prime = candidate;
			p_list->insertFound(myLink);
			p_list->removeCandidate(candidate);
		}
		else {
			//composite, just remove the candidate
			p_list->removeCandidate(candidate);
		}
		//get new candidate and try to loop again
		candidate = p_list->getCandidate();
	}
}

