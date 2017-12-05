#include "primeList.h"
#include <exception>
#include <fstream>



primeList::primeList(int threads, unsigned long long int limit)
{
	//seed the list with the first prime
	current = new link();
	current->prime = static_cast<unsigned long long int>(2);
	current->next = nullptr;
	
	//set the limit
	primeLimit = limit;
	//get our list pointers ready
	previous = current;
	first = current;
	last = current;
	largestFoundReady = current;
	//set the vector to have enough space so we don't need to resize later.
	currentCandidates.reserve(threads + 1);
}

//clean up after ourselves
primeList::~primeList()
{
	//delete each item in the list.
	current = previous = first;
	while (current->next != nullptr) {
		previous = current;
		current = current->next;
		delete previous;
	}
	delete current;
	
}

unsigned long long int primeList::getCandidate() {
	//lock the vector at the frontso we don't get the race condition we had initially
	//where a candidate would be generated but not added to the vector
	//and some higher candidates would already be getting returned before
	//we could finally add a lower candidate for checking.
	vectorMutex.lock();
	unsigned long long int toReturn = nextCandidate;
	//if we are at the limit and need to stop searching
	if (toReturn >= (primeLimit+1)) {
		vectorMutex.unlock();
		return 0;
	}
	else {
		//add the candidate to the vector of candidates and send it to the caller		
		currentCandidates.push_back(toReturn);
		nextCandidate += 2;
		vectorMutex.unlock();
		return toReturn;
	}
}

//returns the first link
primeList::link* primeList::getFirst(){
	return first;
}

//returns the value of the largest prime in the completed part of the list.
unsigned long long int primeList::getLargestReady() {
	return largestFoundReady->prime;
}

//remove a candidate from the vector.
void primeList::removeCandidate(const unsigned long long int candidate) {
	//lock the vector so we can do a threadsafe search for the candidate in our
	//vector, and then remove it.
	vectorMutex.lock();
	for (unsigned char i = 0; i < currentCandidates.size(); ++i) {
		if (currentCandidates[i] == candidate) {
			currentCandidates.erase(currentCandidates.begin() + i);
			vectorMutex.unlock();
			return;
		}
	}
	vectorMutex.unlock();
	throw new exception("Could not find candidate to erase in current candidate list.");
}

//insert the found prime to the tail of the list, in numerical order
void primeList::insertFound(link* found) {
	//lock the tail of the list to ensure that we aren't racing and getting
	//links out of order.
	endLinkMutex.lock();
	//get the current start to the unordered tail and the link after
	link* testCurrent = largestFoundReady;
	link* testNext = testCurrent->next;
	//while we have not reached the end of the list
	while(testCurrent != nullptr && testCurrent != NULL) {
		//if our returned prime is larger than the current one, and we're at the end of the list
		if (found->prime > testCurrent->prime && (testNext == nullptr || testNext == NULL)) {
			//add our found to the end of the list
			testCurrent->next = found;
			last = found;
			found->next = nullptr;
			//check to see if we can move our "largest found" sentinal
			//and then increment the number of primes found
			indexLargestFound();
			++numberFound;
			break;
		}
		//if our returned prime is not larger than the next one to check,
		//but larger than the current, insert it here
		else if (found->prime > testCurrent->prime && found->prime < testNext->prime) {
			testCurrent->next = found;
			found->next = testNext;
			//check to see if we can move our "largest found" sentinal
			//and then increment the number of primes found
			indexLargestFound();
			++numberFound;
			break;
		}
		//otherwise move to the next link
		else{
			testCurrent = testNext;
			testNext = testCurrent->next;
		}
	}
	//unlock the tail
	endLinkMutex.unlock();
	
}


bool primeList::isSmallestCandidate(const unsigned long long int candidate) {
	bool isSmallest = false;
	//lock the vector to make sure we don't have any candidates added while we're
	//checking if we are the smallest
	vectorMutex.lock();
	//check all the candidates and break if we find a larger one
	for (unsigned char i = 0; i < currentCandidates.size(); ++i) {
		if (candidate <= currentCandidates[i]) {
			isSmallest = true;
		}
		else {
			isSmallest = false;
			break;
		}
	}
	//unlock and return if the candidate was the smallest
	vectorMutex.unlock();
	return isSmallest;
}

//see if we need to move our sentinel for the complete head part of the list
void primeList::indexLargestFound() {
	//start at the sentinel
	link* nextLargestTest = largestFoundReady;
	bool newLargest = true;
	//while we haven't reached the end of the list (this should only happen if
	//we got a candidate back that allowed us to complete what was missing from
	//the list, and now the tail is complete)
	while (newLargest && nextLargestTest != nullptr) {
		//if there are no smaller candidates
		if (isSmallestCandidate(nextLargestTest->prime)) {
			//move the sentinel up one position
			if (output.is_open()) {
				output << nextLargestTest->prime << endl;
			}
			newLargest = true;
			largestFoundReady = nextLargestTest;
			nextLargestTest = largestFoundReady->next;
		}
		//otherwise there is still a candidate out that stops us from moving up.
		else {
			newLargest = false;
		}
	}
}

//this toString is absolute garbage, but shows the number of primes found, then proceeds
//to list them all out to the console. takes forever.
string primeList::toString() {
	
		cout << "Prime list will be output in 'outputText.txt'" << endl;
		cout << "Number of primes found: " << numberFound << endl;
		//cin >> a; //was a breakpoint so i could verify the number of primes.
		output.open("outputText.txt");
		current = first;
		while (current != nullptr && current != NULL) {
			output << current->prime << endl;
			current = current->next;
		}
		output.close();
	return "h";
}