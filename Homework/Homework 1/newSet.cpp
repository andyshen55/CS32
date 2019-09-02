#include "newSet.h"
using namespace std;

Set::Set(int i)
{
	//arrays cant have negative or zero size
	if (i <= 0)
		exit(1);

	m_max = i;
	m_items = 0;
	m_arr = new ItemType[m_max];
}

Set::~Set()
{
	delete [] m_arr;
}

Set::Set(const Set& other)
{
	m_items = other.m_items;
	m_max = other.m_max;
	m_arr = new ItemType[m_max];
	for (int i = 0; i < m_items; i++)
	{
		m_arr[i] = other.m_arr[i];
	}
}

Set& Set::operator=(const Set& other)
{
	if (this == &other)
		return *this;
	Set temp = other;
	swap(temp);
	return *this;
}

bool Set::empty() const
{
	return !m_items;
}

int Set::size() const
{
	return m_items;
}

bool Set::insert(const ItemType& value)
{
	if (!m_items)
	{
		m_arr[0] = value;
		m_items++;
		return true;
	}

	for (int i = 0; i < m_items; i++)
	{
		if (this->contains(value) || m_items == m_max)
			return false;
		else
		{
			m_arr[m_items] = value;
			m_items++;
			return true;
		}
	}
	return false;
}

bool Set::erase(const ItemType& value)
{
	for (int i = 0; i < m_items; i++)
	{
		if (m_arr[i] == value)
		{
			m_arr[i] = m_arr[m_items - 1];
			m_items--;
			return true;
		}
	}
	return false;
}

bool Set::contains(const ItemType& value) const
{
	bool contains = false;
	for (int i = 0; i < m_items; i++)
	{
		if (m_arr[i] == value)
			contains = true;
	}
	return contains;
}

bool Set::get(int i, ItemType& value) const
{
	int greater;
	if (i >= 0 && m_items > i)
	{
		for (int j = 0; j < m_items; j++)
		{
			greater = 0;
			for (int k = 0; k < m_items; k++)
			{
				if (m_arr[j] > m_arr[k])
					greater++;
			}
			if (greater == i)
			{
				value = m_arr[j];
				return true;
			}
		}
	}
	return false;
}

void Set::swap(Set& other)
{
	int temp = m_items;
	m_items = other.m_items;
	other.m_items = temp;
	temp = m_max;
	m_max = other.m_max;
	other.m_max = temp;

	ItemType* tempPtr = m_arr;
	m_arr = other.m_arr;
	other.m_arr = tempPtr;
}