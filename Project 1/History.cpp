#include <iostream>

#ifndef HISTORY
#define HISTORY
#include "History.h"
#endif

using std::cout;
using std::endl;

History::History(int nRows, int nCols)
{
	m_cols = nCols;
	m_rows = nRows;
	for (int i = 1; i <= nRows; i++)
	{
		for (int j = 1; j <= nCols; j++)
		{
			deadCount[i][j] = '.';
		}
	}
}

bool History::record(int r, int c)
{
	if (r <= 0 || r > MAXROWS || c <= 0 || c > MAXCOLS)
		return false;
	else
		if (deadCount[r][c] == '.')
			deadCount[r][c] = 'A';
		else if (deadCount[r][c] < 'Z')
			deadCount[r][c]++;
	return true;
}

void History::display() const
{
	clearScreen();
	for (int i = 1; i <= m_rows; i++)
	{
		for (int j = 1; j <= m_cols; j++)
			cout << deadCount[i][j];
		cout << endl;
	}
	cout << endl;
}