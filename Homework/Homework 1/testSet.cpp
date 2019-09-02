#include "Set.h"
#include <iostream>
#include <cassert>
using namespace std;

int main()
{
	Set ss;
	ss.insert("lavash");
	ss.insert("roti");
	ss.insert("chapati");
	ss.insert("injera");
	ss.insert("roti");
	ss.insert("matzo");
	ss.insert("injera");
	assert(ss.size() == 5);  // duplicate "roti" and "injera" were not added
	string x;
	ss.get(0, x);
	assert(x == "chapati");  // "chapati" is greater than exactly 0 items in ss
	ss.get(4, x);
	assert(x == "roti");  // "roti" is greater than exactly 4 items in ss
	ss.get(2, x);
	assert(x == "lavash");  // "lavash" is greater than exactly 2 items in ss
	Set ss1;
	ss1.insert("laobing");
	Set ss2;
	ss2.insert("matzo");
	ss2.insert("pita");
	ss1.swap(ss2);
	assert(ss1.size() == 2 && ss1.contains("matzo") && ss1.contains("pita") &&
		ss2.size() == 1 && ss2.contains("laobing"));
	Set ss3;
	ss3.insert("dosa");
	assert(!ss3.contains(""));
	ss3.insert("tortilla");
	ss3.insert("");
	ss3.insert("focaccia");
	assert(ss3.contains(""));
	ss3.erase("dosa");
	assert(ss3.size() == 3 && ss3.contains("focaccia") && ss3.contains("tortilla") &&
		ss3.contains(""));
	string v;
	assert(ss3.get(1, v) && v == "focaccia");
	assert(ss3.get(0, v) && v == "");
}