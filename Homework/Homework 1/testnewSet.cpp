#include "newSet.h"
#include <iostream>
#include <cassert>
using namespace std;

void test()
{
	Set set;
	assert(set.insert("abc"));
	assert(set.insert("def"));
	assert(set.size() == 2);
	ItemType x = "";
	assert(set.get(0, x) && (x == "def" || x == "abc"));
	Set set2 = set, set3(5);
	assert(set2.size() == 2 && set3.size() == 0);
	set2.insert("adv");
	set3 = set2;
	assert(set3.size() == 3);
}

int main()
{
	test();
	Set a(1000);   // a can hold at most 1000 distinct items
	Set b(5);      // b can hold at most 5 distinct items
	Set c;         // c can hold at most DEFAULT_MAX_ITEMS distinct items
	ItemType v[6] = { "six", "distinct", "values", "of", "the", "appropriate type" };

	// No failures inserting 5 distinct items into b
	for (int k = 0; k < 5; k++)
		assert(b.insert(v[k]));

	// Failure if we try to insert a sixth distinct item into b
	assert(!b.insert(v[5]));

	// When two Sets' contents are swapped, their capacities are swapped
	// as well:
	a.swap(b);
	assert(!a.insert(v[5]) && b.insert(v[5]));
}