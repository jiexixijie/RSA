// RSA.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>

using namespace std;

int exGcd(int a, int b, int &x, int &y);
//n越大确定为质数的可能性越大 
bool Is_Valid_Prime(unsigned long prime, int counts = 5);
//***************************************************************************
//首先这个待测数N一定是一个奇数
//所以N−1是一个偶数，而且一定可以写成2^s*d的形式，
//选取一个介于[1, N−1]的整数a作为底。
//把N−1写成了2^s*d的形式，得到了s和d，如果a^dmodN = 1，或者存在一个r∈[0, s−1]
//使得a^(2rd)modN = −1（和a^(2rd)modN = N−1是等价的)
//那么我们就称N通过了以a为底的Miller - Rabin素性测试，也就是说N有很大可能是素数。
//*****************************************************************************
//Miller-Rabin素性测试
//True-是合数 False--为质数 a为底数
bool Miller_Rabin(unsigned long N, unsigned long a, unsigned long s, unsigned long d);


int _tmain(int argc, _TCHAR* argv[])
{
	int x, y;
	std::cout << exGcd(8, 9, x, y) << endl;
	std::cout << x << y;
	
	system("pause");
	return 0;
}


bool Is_Valid_Prime(unsigned long prime, int counts){
	//偶数或2
	if (prime == 2){
		return true;
	}
	if (prime < 2 && prime % 2 == 0){
		return false;
	}
	//2^s *d
	unsigned long s = 0, d = prime - 1;
	while (d % 2 == 0){
		s++;
		d = d / 2;
	}
	/*printf("%d=2^%d*%d+1", prime,s, d);*/
	srand((unsigned)time(NULL));
	for (int i = 0; i < counts; i++){
		unsigned long a = 1 + rand() % (prime - 2);
		if (Miller_Rabin(prime, a, s, d)){
			return false;
		}
	}
	return true;
}


bool Miller_Rabin(unsigned long N, unsigned long a, unsigned long s, unsigned long d){
	//a^dmodN = 1
	unsigned long result = (unsigned long)powl(a, d);
	result = result%N;
	if (result == 1){
		return false;
	}
	//a^(2^r*d)modN = −1（和a^(2^r*d)modN = N−1
	for (int i = 0; i < s; i++){
		result = powl(2, i)*d;
		result = (unsigned long)powl(a, result);
		result = result%N;
		if (result == N - 1){
			return false;
		}
	}
	return true;
}

int exGcd(int a, int b, int &x, int &y)
{
	if (b == 0)
	{
		x = 1;
		y = 0;
		return a;
	}
	int r = exGcd(b, a % b, x, y);
	int t = x;
	x = y;
	y = t - a / b * y;

	return r;
}