bool anyTrue(const double a[], int n)
{
	if (n <= 0)
		return false;

	return (somePredicate(a[0]) || anyTrue(a + 1, n - 1));
}

int countTrue(const double a[], int n)
{
	if (n <= 0)
		return 0;

	if (somePredicate(a[0]))
		return 1 + countTrue(a + 1, n - 1);
	
	return countTrue(a + 1, n - 1);
}

int firstTrue(const double a[], int n)
{
	if (n <= 0)
		return -1;

	if (somePredicate(a[0]))
		return 0;

	int result = firstTrue(a + 1, n - 1);

	if (result == -1)
		return -1;

	return 1 + result;
}

int positionOfMin(const double a[], int n)
{
	if (n <= 0)
		return -1;

	if (n == 1)
		return 0;

	int temp = positionOfMin(a + 1, n - 1);

	if (a[0] <= a[temp + 1])
		return 0;

	return temp + 1;
}

bool includes(const double a1[], int n1, const double a2[], int n2)
{
	if (n2 == 0)
		return true;
	if (n1 <= 0 || n2 < 0)
		return false;

	if (a1[0] == a2[0])
		return includes(a1 + 1, n1 - 1, a2 + 1, n2 - 1);

	return includes(a1 + 1, n1 - 1, a2, n2);
}