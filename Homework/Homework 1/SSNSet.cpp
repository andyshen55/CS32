#include "SSNSet.h"
#include "Set.h"
#include <iostream>
using namespace std;

SSNSet::SSNSet()
{}

bool SSNSet::add(unsigned long ssn)
{
	return m_set.insert(ssn);
}

int SSNSet::size() const
{
	return m_set.size();
}

void SSNSet::print() const
{
	unsigned long temp;
	for (int i = 0; i < m_set.size(); i++)
	{
		m_set.get(i, temp);
		cout << temp << endl;
	}
}
