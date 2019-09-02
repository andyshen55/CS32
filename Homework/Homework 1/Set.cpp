#include "Set.h"
#include <algorithm>
using namespace std;

Set::Set()
{
	m_items = 0;
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
		if (this->contains(value) || m_items == DEFAULT_MAX_ITEMS)
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

	ItemType tempItem;

	for (int i = 0; i < max(m_items, other.m_items); i++)
	{
		tempItem = m_arr[i];
		m_arr[i] = other.m_arr[i];
		other.m_arr[i] = tempItem;
	}
}