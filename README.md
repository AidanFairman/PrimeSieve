# PrimeSieve

This was a personal project that I wrote for a hobby and personal challenge. It is the third or fourth prime sieve I've written, and is probably the fastest. 

### Try it out

There is an executable in the root folder of this project. It is an x64 build of the prime sieve. To try it out, just double click the executable (it defaults to 4 threads, finding all primes between 1 and 1,000,000). To use a custom number of threads or your own limit, run the project from the command prompt in the form "Primes.exe \[threadCound] \[upperLimit]"

### What is it?

This program uses a linked list written specifically for this application. The list is thread-safe, allowing the worker threads to iterate over the head of the list without needing to lock the data structure. An area near the tail is locked where the list is incomplete due to candidates being out. Once a candidate comes back and there are no smaller candidates, the locked area is moved to where the list is again incomplete. This allows the locked area to be as small as possible at any given time. The idea was to control the size of n for O(n) operations in the locked area.

### Why?

I chose this approach based on the first prime sieve I ever made (for an assignment). Written in java, it used a single iterator that would in-order traverse a sorted binary tree. The problem with this is that candidates were being added almost in order, and there was only one iterator. The binary tree ended up being almost a linked list (without rebalancing) and each thread wasted a lot of time waiting for the iterator to traverse the tree.

The linked list approach in this project destroys the java tree approach, finding the primes from 0-1,000,000 in less than a second, and finding the first million in about 5 seconds (on my machine here at home). The java project took significantly longer, taking 8+ seconds to find the primes from 0 - 1,000,000. 

### The Future

Eventually I would like to attempt to harness the power of the GPU to try and make a prime sieve, but i'm currently limited on time and ability.
