#pragma once

namespace SciMathx
{
	class CMathx
	{
	public:
		CMathx();
		~CMathx();
	private:
		double fact_cache[50];
		double comb_cache[50][50];
		void InitFact();
		void InitComb();
	public:
		double Comb(int n, int m);
		double Fact(int n);
	};
	

}