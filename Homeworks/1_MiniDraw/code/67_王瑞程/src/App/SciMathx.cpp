#include "SciMathx.h"

namespace SciMathx{

	CMathx::CMathx()
	{
		InitFact();
		InitComb();
	}

	CMathx::~CMathx()
	{

	}

	void CMathx::InitFact()
	{
		fact_cache[0] = 1.;
		for (int i = 1; i < 50.; i += 1)
		{
			fact_cache[i] = fact_cache[i - 1] * (double)i;
		}
	}
	void CMathx::InitComb()
	{
		for (int i = 0; i < 50; i++)
		{
			for (int j = 0; j <= i; j++)
			{
				comb_cache[i][j] = fact_cache[i] / (fact_cache[i - j] * fact_cache[j]);
			}
		}
	}
	double CMathx::Comb(int n, int m)
	{
		if (n >= 50 || m > n)
			throw 99;
		return comb_cache[n][m];
	}

	inline double CMathx::Fact(int n)
	{
		if (n >= 50)
			throw 99;
		return fact_cache[n];
	}
}