#include "Set.h"
#include <iostream>
#include <string>
#include <stack>
#include <cctype>
#include <cassert>

using namespace std;

int precedence(char ch);
bool validIn(string infix, string& postfix);
int evaluate(string infix, const Set& trueValues, const Set& falseValues, string& postfix, bool& result);

int main()
{
	string trueChars = "tywz";
	string falseChars = "fnx";
	Set trues;
	Set falses;
	for (int k = 0; k < trueChars.size(); k++)
		trues.insert(trueChars[k]);
	for (int k = 0; k < falseChars.size(); k++)
		falses.insert(falseChars[k]);

	string pf;
	bool answer;
	assert(evaluate("w| f", trues, falses, pf, answer) == 0 && pf == "wf|" &&  answer);
	assert(evaluate("y|", trues, falses, pf, answer) == 1);
	assert(evaluate("n t", trues, falses, pf, answer) == 1);
	assert(evaluate("nt", trues, falses, pf, answer) == 1);
	assert(evaluate("()", trues, falses, pf, answer) == 1);
	assert(evaluate("y(n|y)", trues, falses, pf, answer) == 1);
	assert(evaluate("t(&n)", trues, falses, pf, answer) == 1);
	assert(evaluate("(n&(t|7)", trues, falses, pf, answer) == 1);
	assert(evaluate("", trues, falses, pf, answer) == 1);
	//assert(evaluate("f  |  !f & (t&n) ", trues, falses, pf, answer) == 0 && pf == "ff!tn&&|" && !answer);
	assert(evaluate(" x  ", trues, falses, pf, answer) == 0 && pf == "x" && !answer);
	trues.insert('x');
	assert(evaluate("((x))", trues, falses, pf, answer) == 3);
	falses.erase('x');
	assert(evaluate("((x))", trues, falses, pf, answer) == 0 && pf == "x"  &&  answer);
	trues.erase('w');
	assert(evaluate("w| f", trues, falses, pf, answer) == 2);
	falses.insert('w');
	assert(evaluate("w| f", trues, falses, pf, answer) == 0 && pf == "wf|" && !answer);
	
	cout << "Passed all tests" << endl;
}

int evaluate(string infix, const Set& trueValues, const Set& falseValues, string& postfix, bool& result)
{
	//after this check, postfix will have been set to the correct postfix string
	if (!validIn(infix, postfix))
		return 1;

	stack<bool> valueStack;

	//check for overlap/non-existence in true/false sets
	for (int i = 0; i < postfix.size(); i++)
	{
		char ch = postfix[i];
		if (islower(ch))
		{
			if (!trueValues.contains(ch) && !falseValues.contains(ch))
				return 2;
			else if (trueValues.contains(ch) && falseValues.contains(ch))
				return 3;
		}
	}

	for (int j = 0; j < postfix.size(); j++)
	{
		char temp = postfix[j];
		if (islower(temp))
		{
			if (trueValues.contains(temp))
				valueStack.push(true);
			else
				valueStack.push(false);
		}
		else
		{
			bool resultBool, tf1 = valueStack.top();
			valueStack.pop();
			if (temp == '!')
				resultBool = !tf1;
			else
			{
				bool tf2 = valueStack.top();
				valueStack.pop();
				if (temp == '|')
					resultBool = (tf1 || tf2);
				else if (temp == '&')
					resultBool = (tf1 && tf2);
			}
			valueStack.push(resultBool);
		}
	}

	result = valueStack.top();
	return 0;
}

//returns true if infix string is syntactically valid, false otherwise
bool validIn(string infix, string& postfix)
{
	stack<char> ops;
	string temp;
	string tempPostfix;
	bool letters = false; 
	int consLetters = 0;
	int consAndOr = 0;
	//removes spaces to simplify string
	for (int i = 0; i < infix.size(); i++)
	{
		if (infix[i] != ' ')
			temp += infix[i];
	}

	if (infix == "")
		return false;

	//check for binary operators at beginning/end, as well as specific operators
	char begin = temp[0], end = temp[temp.size() - 1];

	if (end == '|' || end == '&' || end == '(' || begin == ')' || begin == '|' || begin == '&')
		return false;

	//create postfix string
	for (int j = 0; j < temp.size(); j++)
	{
		char ch = temp[j];
		//check if all chars in string are either lowercase or operands
		if (!(ch == '!' || ch == '&' || ch == '|' || ch == '(' || ch == ')' || islower(ch)))
			return false;
		//check if any letters are preceded by any other letters
		if (j != 0 && islower(ch) && islower(temp[j - 1]))
			return false;
		//check if any binary operators arent preceded or succeeded by other binary operators. dont have to check for edge cases cuz begin and end cases are already ruled out
		if ((ch == '&' || ch == '|') && (temp[j - 1] == '&' || temp[j - 1] == '|' || temp[j + 1] == '&'
			|| temp[j + 1] == '|' || temp[j + 1] == ')' || temp[j - 1] == '('))
			return false;
		if (ch == '|' || ch == '&')
		{
			consAndOr++;
			if (consLetters > 1)
				consLetters--;
		}
		if (islower(ch))
		{
			letters = true;
			consLetters++;
			if (consAndOr > 0)
				consAndOr--;
		}
		if (consLetters > 1)
			return 1;
		if (consAndOr > 1)
			return false;
		if (j == infix.length() - 1 && consAndOr != 0 || j == infix.length() - 1 && ch == '!')
			return 1;
	}

	for (int k = 0; k < infix.size(); k++)
	{
		char kh = infix[k];
		//after checking syntax violations, add each index to tempPostfix or ops stack
		switch (kh)
		{
		case '(':
			ops.push(kh);
			break;
		case ')':
			if (!ops.empty())
			{
				while (ops.top() != '(')
				{
					tempPostfix += ops.top();
					ops.pop();
					if (ops.empty())
						return false;
				}
				ops.pop();
			}
			break;
		case '!':
		case '&':
		case '|':
			while (!ops.empty() && ops.top() != '(' && precedence(kh) <= precedence(ops.top()))
			{
				tempPostfix += ops.top();
				ops.pop();
			}
			ops.push(kh); 
			break;
		default:
			if (islower(kh))
				tempPostfix += kh;
			break;
		}
	}
	//add remaining operators on stack to postfix expression
	while (!ops.empty())
	{
		tempPostfix += ops.top();
		ops.pop();
	}
	postfix = tempPostfix;
	return (true && letters);
}

int precedence(char ch)
{
	switch (ch)
	{
	case '!':
		return 2;
	case '&':
		return 1;
	case '|':
		return 0;
	}
	return -1;
}