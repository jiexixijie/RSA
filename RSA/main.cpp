// RSA.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include "BigInt.h"

using namespace std;

int exGcd(int a, int b, int &x, int &y);
//n越大确定为质数的可能性越大 
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

void menu();


int _tmain(int argc, _TCHAR* argv[]){
	RSA a;
	//a.Init(256);
	string str = "FE0F4D79381EF597B907FEAEC47DD709";
	BigInt b(str);
	a.Length = str.length();
	if (a.Is_Valid_Prime(b)){
		printf("Prime\n");
	}
	/*string str = "FFFFFFFFF";
	BigInt big(str), small((unsigned long)2);
	(big % small).ShowData();*/

	//while (true)
	//{
	//	menu();
	//	char input ;
	//	cin >> input;
	//	if (input == 'c'){
	//		printf("输入生成的密钥对长度\n");
	//		int len=0;
	//		cin >> len;
	//		cout << "正在初始化" << endl;
	//		a.Init(len);
	//	}
	//	else if (input == 'a'){
	//		if (a.success){
	//		//	a.ShowPub();
	//		}
	//		else{
	//			printf("请先生成密钥对\n");
	//		}
	//	}
	//	else if (input == 'b'){
	//		if (a.success){
	//			//a.ShowPri();
	//		}
	//		else{
	//			printf("请先生成密钥对\n");
	//		}
	//	}
	//	else if (input == 'e'){
	//		string str = "ming.txt";
	//		//a.Enc(str);
	//		printf("对%s签名成功,可查看mi.txt\n",str.c_str());
	//	}
	//	else if (input == 'd'){
	//		string str = "mi.txt";
	//		//a.Dec(str);
	//	}
	//	else if (input == 'q'){
	//		break;
	//	}
	//	else{
	//		printf("请输入正确的命令\n");
	//	}
	//}
	system("pause");
	return 0;
}

void menu(){
	printf("\t\t生成密钥对:c\n");
	printf("\t\t显示公钥:a\n");
	printf("\t\t显示私钥:b\n");
	printf("\t\t加密文件:e\n");
	printf("\t\t解密文件:d\n");
	printf("\t\t退出:q\n");
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