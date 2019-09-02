#ifndef SET_INCLUDED
#define SET_INCLUDED

#include <string>

using ItemType = std::string;

class Set
{
public:
	Set();								// Create an empty set (i.e., one with no items).
	Set(const Set& other);				// Copy constructor for set
	Set& operator=(const Set& other);	// Assignment operator for set
	~Set();								// Destructor for set

	bool empty() const;  // Return true if the set is empty, otherwise false.
	int size() const;    // Return the number of items in the set.

	bool insert(const ItemType& value);
	// Insert value into the set if it is not already present.  Return
	// true if the value was actually inserted.  Leave the set unchanged
	// and return false if the value was not inserted (perhaps because it
	// was already in the set or because the set has a fixed capacity and
	// is full).

	bool erase(const ItemType& value);
	// Remove the value from the set if present.  Return true if the
	// value was removed; otherwise, leave the set unchanged and
	// return false.

	bool contains(const ItemType& value) const;
	// Return true if the value is in the set, otherwise false.

	bool get(int i, ItemType& value) const;
	// If 0 <= i < size(), copy into value the item in the set that is
	// strictly greater than exactly i items in the set and return true.
	// Otherwise, leave value unchanged and return false.

	void swap(Set& other);
	// Exchange the contents of this set with the other one.

private:
	struct Node
	{
		ItemType m_value;
		Node* m_next;
		Node* m_prev;
	};
	Node*	 m_head;					 // pointer to sentinel node
	int      m_size;                     // number of items in the set
};

// Inline implementations

inline
int Set::size() const
{
	return m_size;
}

inline
bool Set::empty() const
{
	return !m_size;
}

inline
bool Set::contains(const ItemType& value) const
{
	Node* p;				//check if value exists. if loop exits, p is either head (no item found) or p points to node with value
	for (p = m_head->m_next; p != m_head && p->m_value != value; p = p->m_next)
	{}
	return p != m_head;
}

//Non-member functions

void unite(const Set& s1, const Set& s2, Set& result);	
void subtract(const Set& s1, const Set& s2, Set& result);

#endif // SET_INCLUDED
