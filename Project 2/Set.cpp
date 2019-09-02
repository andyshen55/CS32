#include "Set.h"

Set::Set()
	: m_size(0)
{
	m_head = new Node;			//dummy node initializer
	m_head->m_next = m_head;
	m_head->m_prev = m_head;
}

Set::Set(const Set& other)
{
	m_size = other.m_size;
	m_head = new Node;			//dummy node initializer
	m_head->m_next = m_head;
	m_head->m_prev = m_head;

	for (Node* p = other.m_head->m_next; p != other.m_head; p = p->m_next)	//iterate through other linked list
	{
		Node* copyNode = new Node;	 
		copyNode->m_value = p->m_value;		//copy other's value
		copyNode->m_next = m_head->m_next;	//set next pointer in copied node to point to previous first item
		copyNode->m_prev = m_head;			//set prev pointer to head (because node is new first item)
		m_head->m_next->m_prev = copyNode;	//access previous first items m_prev pointer and set it to new node
		m_head->m_next = copyNode;			//set first item to new node via head's next pointer
	}
}

Set& Set::operator=(const Set& other)
{
	if (this != &other)		//check to make sure the same set isnt being assigned to itself
	{
		Set temp = other;	//create temp set with the copy constructor
		swap(temp);			//swap current set with temp set
	}
	return *this;			//for multiple assignment ie. a = b = c
}

Set::~Set()
{
	Node* p;
	Node* temp;
	for (p = m_head->m_next; p != m_head;) //iterate through all nodes that contain values, stop at dummy head node
	{
		temp = p;			//store current node in temp
		p = p->m_next;		//change traversal pointer to point to next node before deleting current node				
		delete temp;
	}
	delete p;				//delete dummy head
}

bool Set::insert(const ItemType& value)
{
	if (m_size) // not an empty list
	{
		for (Node* p = m_head->m_next; p != m_head; p = p->m_next) //check if value already exists
		{
			if (p->m_value == value)
				return false;
		}
	}

	Node* newGuy = new Node;			//create node to be inserted at front of list
	newGuy->m_value = value;
	newGuy->m_next = m_head->m_next;	//set next pointer in new node to point to original first item
	newGuy->m_prev = m_head;			//set prev pointer to head (because node is new first item)
	m_head->m_next->m_prev = newGuy;	//access previous first items m_prev pointer and set it to new node
	m_head->m_next = newGuy;			//set first item to new node via head's next pointer
	m_size++;
	return true;
}

bool Set::erase(const ItemType& value)
{
	Node* p;
	for (p = m_head->m_next; p != m_head && p->m_value != value; p = p->m_next) 
	{}						//check if value already exists. if loop exits, p is either head (no item found) or p points to node with value

	if (p == m_head)		//if p is head, node to be removed wasnt found
		return false;

	p->m_prev->m_next = p->m_next;
	p->m_next->m_prev = p->m_prev;
	delete p;
	m_size--;
	return true;
}

bool Set::get(int i, ItemType& value) const
{
	if (i >= 0 && m_size > i)
	{
		int greater;
		for (Node* p = m_head->m_next; p != m_head; p = p->m_next)		//iterate m_size times for each node in linked list
		{
			greater = 0;												//count of number of m_values > value
			for (Node* q = m_head->m_next; q != m_head; q = q->m_next)	//iterate m_size times for each node in linked list
			{
				if (p->m_value > q->m_value)
					greater++;
			}
			if (greater == i)
			{
				value = p->m_value;
				return true;
			}
		}
	}
	return false;
}

void Set::swap(Set& other)
{
	int tempSize = other.m_size;		//temp variable to swap values
	other.m_size = m_size;
	m_size = tempSize;

	Node* tempHead = other.m_head;		//temp pointer to swap memory addresses
	other.m_head = m_head;
	m_head = tempHead;
}

//Non-member function implementations

void unite(const Set& s1, const Set& s2, Set& result)
{
	int resultSize = result.size(), s1Size = s1.size(),s2Size = s2.size();
	ItemType temp;
	for (int i = 0; i < resultSize; i++) //loop through result set, store each value in variable to call contains on both sets with.
	{
		result.get(i, temp);
		if (!(s1.contains(temp) || s2.contains(temp)))	//if neither set contains the value, erase it.
		{
			if (result.erase(temp))						//if erase is successful, number of iterations (resultSize) and iterator (i) must be changed. 
			{											//(because if the item that is greater than i elements is erased, then the item greater than 
				resultSize--;							//i + 1 elements is now the item greater than i elements) if i is not decremented then the new
				i--;									//ith element will not be checked. 
			}
		}
	}

	//loop through both sets and insert all values (insert should automatically check if value is present
	for (int j = 0; j < s1Size; j++)
	{
		s1.get(j, temp);
		result.insert(temp);
	}

	for (int k = 0; k < s2Size; k++)
	{
		s2.get(k, temp);
		result.insert(temp);
	}
}

void subtract(const Set& s1, const Set& s2, Set& result)
{
	int resultSize = result.size(), s1Size = s1.size();
	ItemType temp;

	//loop through result set and erase all values that arent also present in set1.
	for (int i = 0; i < resultSize; i++)
	{
		result.get(i, temp);
		if (!(s1.contains(temp)))		//if s1 doesnt contain the value in result, erase it
		{
			if (result.erase(temp))		//same logic as in unite() for updating iterator and number of iterations 
			{
				resultSize--;
				i--;
			}
		}
	}

	//loop through s1 and check if s2 contains s1's values. if so, erase the value from the result set. otherwise, add it to result
	for (int j = 0; j < s1Size; j++)
	{
		s1.get(j, temp);
		if (s2.contains(temp))
		{
			if (result.erase(temp))		//same logic as in unite() for updating iterator and number of iterations
			{
				resultSize--;
				j--;
			}
		}
		else
			result.insert(temp);
	}
}