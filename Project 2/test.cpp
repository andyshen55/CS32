/*Test 3
#include "Set.h"
#include <iostream>
#include <cassert>
using namespace std;

void test()
{
	Set uls;
	assert(uls.insert(10));
	assert(uls.insert(20));
	assert(uls.size() == 2);
	assert(uls.contains(20));
	ItemType x = 30;
	assert(uls.get(0, x) && x == 10);
	assert(uls.get(1, x) && x == 20);
}

int main()
{
	test();
	cout << "Passed all tests" << endl;
}
*/

/*Test 2
#include "Set.h"
#include <iostream>
#include <cassert>
using namespace std;

void test()
{
	Set ss;
	assert(ss.insert("roti"));
	assert(ss.insert("pita"));
	assert(ss.size() == 2);
	assert(ss.contains("pita"));
	ItemType x = "laobing";
	assert(ss.get(0, x) && x == "pita");
	assert(ss.get(1, x) && x == "roti");
}

int main()
{
	test();
	cout << "Passed all tests" << endl;
}
*/

/*Test 1
#include "Set.h"
#include <type_traits>

#define CHECKTYPE(f, t) { auto p = static_cast<t>(f); (void)p; }

static_assert(std::is_default_constructible<Set>::value,
	"Set must be default-constructible.");
static_assert(std::is_copy_constructible<Set>::value,
	"Set must be copy-constructible.");

void thisFunctionWillNeverBeCalled()
{
	CHECKTYPE(&Set::operator=, Set& (Set::*)(const Set&));
	CHECKTYPE(&Set::empty, bool (Set::*)() const);
	CHECKTYPE(&Set::size, int  (Set::*)() const);
	CHECKTYPE(&Set::insert, bool (Set::*)(const ItemType&));
	CHECKTYPE(&Set::erase, bool (Set::*)(const ItemType&));
	CHECKTYPE(&Set::contains, bool (Set::*)(const ItemType&) const);
	CHECKTYPE(&Set::get, bool (Set::*)(int, ItemType&) const);
	CHECKTYPE(&Set::swap, void (Set::*)(Set&));
	CHECKTYPE(unite, void(*)(const Set&, const Set&, Set&));
	CHECKTYPE(subtract, void(*)(const Set&, const Set&, Set&));
}

int main()
{}
*/

//My test cases
#include "Set.h"
#include <iostream>
#include <cassert>
using namespace std;

int main()
{
	Set a;				//testing default constructor
	assert(a.size() == 0);		//testing size function on empty set
	assert(a.empty());		//testing empty function on empty set
	assert(a.insert("a"));		//testing insert function
	assert(a.size() == 1);		//testing size function on non empty set
	assert(!a.empty());		//testing empty function on non empty set
	assert(!a.erase("b"));		//testing erase to ensure values that don’t match b don’t get erased
	assert(!a.insert("a"));		//testing insert not adding an already present value
	assert(a.erase("a"));		//testing erase to make sure present values are erased
	assert(a.insert("a"));
	assert(a.contains("a"));		//testing contains function
	assert(!a.contains("b"));		//testing contains function for nonexistent value
	a.insert("b");
	assert(a.contains("b"));	//making sure contains works for b that was added
	Set b(a);			//testing copy constructor
	assert(b.contains("a") && b.contains("b") && b.size() == 2); 	//making sure a was copied into b
	Set c;
	Set d(c);			//testing copy constructor for empty set
	assert(!d.size());			//making sure d is empty
	assert(d.empty());
	string ab = "";
	assert(a.get(1, ab));		//testing get function
	assert(ab == "b");		//b is greater than one value
	assert(a.get(0, ab) && ab == "a");	//a is smallest value
	assert(!d.get(0, ab));		//testing get return value (should return false for empty set)
	assert(!a.get(2, ab)); 	//testing get return value for out of bounds case (2 > m_size – 1, should return false)
	Set e;
	swap(e, d);			//testing swap with two empty sets
	swap(a, d);			//testing swap function with empty set and non empty set
	assert(a.size() == 0 && d.size() == 2 && d.contains("a") && d.contains("b")); //making sure values were swapped
	e = c = d;				//testing assignment operator
	assert(c.size() == 2 && c.contains("b"));	//ensuring values are consistent between c and d
	assert(e.size() == 2 && e.contains("b"));	//ensuring values are consistent between e and c
	a.insert("f");
	a.insert("as");
	unite(a, c, d);			//testing unite function with two non empty sets
	assert(d.size() == 4 && d.contains("as"));	//ensuring result set is accurate
	subtract(d, a, c);		//testing subtract function with two non empty sets
	assert(c.size() == 2 && c.contains("a") && c.contains("b"));	//ensuring that the two values leftover are consistent with the requirements for subtract
	Set f;
	a = b = f;			//check assignment operator with empty sets
	assert(a.empty() && b.empty());//check to make sure they are empty
	unite(a, b, f);			//testing unite function with empty sets
	assert(f.empty());
	subtract(a, b, f);			//testing subtract function with empty sets
	assert(f.empty());
	a.insert("a");
	b.insert("b");
	unite(a, b, a);			//testing unite with aliasing, ie when result set == s1 or s2
	assert(a.size() == 2 && a.contains("b"));	//making sure the result set was correctly modified
	subtract(b, a, f);		//testing subtract when set2 is bigger than set1 and has all of set1’s values
	assert(f.empty());
	a = f;
	f.insert("a");
	f.insert("b");
	a.insert("c");
	a.insert("d");
	a.insert("e");
	subtract(a, f, f);			//Check if subtract function works if result and arguments are the same
	assert(f.size() == 3 && f.contains("c"));		//since a and f didn’t have any values in common, f
	assert(f.contains("d") && f.contains("e"));	//should contain everything a has
	cout << "Passed all tests\n";
}