# ACTRAFP - As Close To Random As F***ing Possible (A random Number Generator)


ACTRAFP (Ack-Raffp) is a very simple library header that I created to provide a very reliable, and simple-to-use random number generator.
I created this header because, surprisingly, I could not find any simple random number generator algorithms that could be implemented into
C/C++ that weren't relying on 'seeds' like system time. My algorithm, instead, relies on ever-changing resources like your computer's 
running programs' memory usage, your cursor's X/Y coordinates, prime numbers and how many times a random number has been generated. These
things, combined are VERY different between systems, which make this work more randomly, even though true-random does not exist in computer
worlds because everything works in such a structured, quick way even though it all seems random to us.


It is so simple to implement this into your code. Include the header, and call the getRandom(int) function. The 'int' parameter represents 
how big your bit-resolution of your random number is. Your number can be up-to 64 bits. It's that simple!
