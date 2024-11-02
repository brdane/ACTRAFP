//=======================================================
// ACTRAFP Random Number Generation Library, By: Brenden Dane (brdane@gmail.com)
//
// ACTRAFP is an acranym which stands for, 'As Close To Random As Fucking Possible', and it represents
// just that. Random number generators are hard to come across in-code because they
// technically are not random. Here, I developed a method (that I am certain I am not
// the first to do it this way) which, instead of choosing a random number based on different
// values/references/seeds, it adds a bunch of ever-changing values up into one large sum, and
// checks a handful of certain conditions, and constructs your final random number. To help make
// things extra random, we use the mysterious, beautiful help from our group of friends, that are
// known to us as prime numbers.
//
// Before you start digging in to this code, I recommend you look at my paper which describes the
// algorithm I developed for this. I think it will help you understand the code-portion more.
//
//
// THINGS TO NOTICE: Yes, it is VERY slow... it is processing a lot in-order to give you a true
// random number. Also, this code uses C++98. Maybe modern 64-bit systems are made to run older runtime
// code in a 64-bit fashion... if so, then speed shouldn't be an issue.
//
// Link to paper: 
//
//=======================================================

#include <tlhelp32.h>
#include <stdio.h>
#include <vector>
#include <algorithm>


//A prime amount of prime numbers. How much more prime can it get?
const int primes[373] = 
{
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53,
    59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 
    127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 
    191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 
    257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 
    331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397,
    401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
    467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557,
    563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619,
    631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701,
    709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787,
    797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863,
    877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953,
    967, 971, 977, 983, 991, 997, 1009, 1013, 1019, 1021, 1031,
    1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091, 1093,
    1097
};

unsigned int primeIndex = 0; //Used to keep track of our index in ^this^ array.

__int64 inc = 0; //An incremental value, which gets manipulated and also thrown into our random-generation digest.


std::vector<unsigned __int64> logged;


//A necessary structure used for grabbing system process information.
typedef struct _PROCESS_MEMORY_COUNTERS 
{
    DWORD cb;                    // Size of the structure
    DWORD PageFaultCount;        // Number of page faults
    SIZE_T PeakWorkingSetSize;   // Peak working set size
    SIZE_T WorkingSetSize;       // Working set size
    SIZE_T QuotaPeakPagedPoolUsage; // Peak paged pool usage
    SIZE_T QuotaPagedPoolUsage;  // Paged pool usage
    SIZE_T QuotaPeakNonPagedPoolUsage; // Peak non-paged pool usage
    SIZE_T QuotaNonPagedPoolUsage; // Non-paged pool usage
    SIZE_T PagefileUsage;        // Page file usage
    SIZE_T PeakPagefileUsage;    // Peak page file usage
} PROCESS_MEMORY_COUNTERS;

//Defining one of these because we are going to point a DLL-function to it.
typedef BOOL (WINAPI *GetProcessMemoryInfoFunc)(HANDLE, PROCESS_MEMORY_COUNTERS*, DWORD);


//Generates the main number, which helps determine the construction of the final number.
unsigned __int64 base();


byte getRandom8(); //random 8-bit value.
unsigned short getRandom16(); //random 16-bit value.
unsigned long getRandom32(); //random 32-bit value.
unsigned __int64 getRandom64(); //random 64-bit value.





//This function develops a random-enough number from NOT time-based values, because time is reoccuring and doesn't change fast enough.
//Here, we use your computer's process resources as the "seeds", which are perfect for this because they change fast enough and they differ system-to-system.
//Now, we use seeds, but after we get a snapshot of the seeds, we modify the hell out of them and do a lot of stuff including prime numbers, addition, modulus
//and more stuff to give us a base number that we will refer to to ACTUALLY generate our random numbers.
unsigned __int64 base()
{
	//We start with a big number variable.
	unsigned __int64 final = 0;

	inc++;

	int p[2];

	//We grab a 'snapshot' of the current state of our process' credentials.
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

	PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

	//Start with the first process...
    if (Process32First(hSnapshot, &pe)) 
	{
        do 
		{
			//Open the process...
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);
			
			if (hProcess) 
			{
				//Load a DLL because we need a VERY useful function from it.
				HMODULE hPsapi = LoadLibrary("Psapi.dll");
				
				if (hPsapi) 
				{
					//The function we need is called GetProcessMemoryInfo().
					GetProcessMemoryInfoFunc pGetProcessMemoryInfo = (GetProcessMemoryInfoFunc)GetProcAddress(hPsapi, "GetProcessMemoryInfo");

					PROCESS_MEMORY_COUNTERS pmc;

					//Just return 0 if we can't find it.... we should, though.
					if (!pGetProcessMemoryInfo)
					{
						return 0;
					}
				
					//Grab the memory-related credentials from the current process in-hand.
					//Grab other changing things, like mouse position, etc.
					if (pGetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) 
					{
						POINT cp;
						GetCursorPos(&cp);

						unsigned long v[10] = {  pe.th32ProcessID,
												pmc.WorkingSetSize,
												pmc.PagefileUsage,
												pmc.PeakPagefileUsage,
												cp.x,
												pe.dwFlags,
												pe.th32ParentProcessID,
												pe.th32ModuleID,
												pe.th32DefaultHeapID,
												cp.y
											};
						
						for (int i=0;i<10;i++)
						{
							//Start by adding all of the process credentials' values to the final number.
							final += v[i];
						}

						for (i=0; i<64; i++)
						{
							final = final | ( ((v[i&8] >> i) & 1) << i);
						}

						//Add our current indexed prime number to the mix.
						final += primes[primeIndex%373];

						//If our current final number is even, then increment the prime index in our array.
						if (final % 2 == 0)
						{
							primeIndex++;
						}
						else //If it is an odd value, go up two values, instead.
						{
							primeIndex += 2;
						}

						//Set one of two variables to whatever our current prime number index is.
						p[0] = primes[primeIndex%373];
						
						//Increment the index again.
						primeIndex += inc;

						//Set the prime index to the modulus of the current big number we have divided by the prime value in the primeindex.
						if (primes[primeIndex%373] != 0) 
						{
							primeIndex = (final % primes[primeIndex%373]);
						}

						//Set our second variable for our current indexed prime number.
						p[1] = primes[primeIndex%373];

						//Multiply both those primes and add that product to the final.
						final += (p[0]*p[1]);

						//Add whatever current indexed prime we have too.
						final += primes[primeIndex%373];


						if (primes[primeIndex%373] != 0) 
						{
							primeIndex = (final % primes[primeIndex%373]);
						}

						//Finally, get the modular from the final number divided by the current prime index.
						if (primes[primeIndex%373] != 0) 
						{
							final += (final % primes[primeIndex%373]);
						}

						if (inc != 0)
						{
							final += (final % (inc + primeIndex));
						}

						if (primes[primeIndex%373] != 0) 
						{
							final *= primes[primeIndex%373];
						}
						
						//Close that process handle.
						CloseHandle(hProcess);
					}
				} 
			}
            
        } //Move on the next process and repeat the process for the remainder of our processes as we add to the big number.
		while (Process32Next(hSnapshot, &pe));
    }

	//After all of that yahoo, we have our final, big number.
	return final;
}


void AddToLog(unsigned __int64 nta)
{
	logged.push_back(nta);
}

bool AlreadyGenerated(unsigned __int64 num)
{
	bool bFound = false;

	if (num != 0)
	{
		unsigned __int64* it = std::find(logged.begin(), logged.end(), num);
		
		if (it != logged.end())
		{
			bFound = true;
		}
	}

	return bFound;
}

__int64 getRandom(byte bitAmount)
{
	__int64 final = 0;

	unsigned __int64 in = base();
	
	for (byte i=0;i<bitAmount;i++)
	{
		
		if ((inc % 2) != 0)
		{
			final = final | ( ((in >> i) & 1) << i);
		}
		else
		{
			final = final | ( ((primes[inc%373] >> i%8) & 1) << i);
		}
	}

	inc++;
	return final;
}
