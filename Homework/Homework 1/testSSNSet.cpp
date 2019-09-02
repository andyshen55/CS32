#include "Set.h"
#include "SSNSet.h"
#include <cassert>
#include <iostream>
using namespace std;

int main()
{
	SSNSet ssnset;
	assert(ssnset.size() == 0);
	assert(ssnset.add(102390213) && ssnset.size() == 1); 
	SSNSet ssnset2 = ssnset, ssnset3; 
	assert(ssnset2.size() == 1);
	assert(ssnset2.add(332390213) && ssnset2.size() == 2);
	assert(ssnset3.size() == 0);
	ssnset3 = ssnset2;
	assert(ssnset3.size() == 2 && ssnset3.add(123091203) && ssnset3.size() == 3);
	ssnset.print();
	ssnset2.print();
	ssnset3.print();
}