#include <stdio.h>
#include <vector>

using namespace std;

//因为存在Int64 所以最大乘法可以是long*long,选择0X100000000进制

class BigInt
{
public:
	//直接构造 data为空 is_Negative=True  
	BigInt();
	//重载构造函数
	BigInt(unsigned long number);
	BigInt(unsigned __int64 number);
	BigInt(const BigInt &a);
	BigInt(const string &str, bool is_fail=false);
	~BigInt();
	//用于存储大数 1024=32*long  
	//高位在后低位在前，方便进位
	vector<unsigned long> data;
	//是否为负数 True:-   False:+/0
	bool Is_Negative;

	//字符串16进制(只能包含0-F)转BigInt
	//字符串为正常大小 高位在前,低位在后 
	friend bool CopyFromHexStr(BigInt &dst, const string &str);
	//BigInt 转 str
	friend bool PasteHexStr(string &str, const BigInt &a);

	friend BigInt Abs(const BigInt &a);
	//取反
	friend BigInt Negate(const BigInt &a);

	//重载运算符 BigInt 和 BigInt
	friend BigInt operator + (const BigInt &a, const BigInt &b);
	friend BigInt operator - (const BigInt &a, const BigInt &b);
	friend BigInt operator * (const BigInt &a, const BigInt &b);
	friend BigInt operator / (const BigInt &a, const BigInt &b);
	//返回 Abs(a)% Abs(b)
	friend BigInt operator % (const BigInt &a, const BigInt &b);

	friend bool operator < (const BigInt &a, const BigInt &b);
	friend bool operator == (const BigInt &a, const BigInt &b);
	friend bool operator != (const BigInt &a, const BigInt &b);
	friend bool operator <= (const BigInt &a, const BigInt &b);

	//重载运算符 BigInt 和 long
	friend BigInt operator + (const BigInt &a, const unsigned long b){ BigInt temp(a), temp2(b); return temp + temp2; }
	friend BigInt operator - (const BigInt &a, const unsigned long b){ BigInt temp(a), temp2(b); return temp - temp2; }
	friend BigInt operator * (const BigInt &a, const unsigned long b){ BigInt temp(a), temp2(b); return temp * temp2; }
	friend BigInt operator * (const BigInt &a, const unsigned __int64 b){ BigInt temp(a), temp2(b); return temp * temp2; }
	friend BigInt operator / (const BigInt &a, const unsigned long b){ BigInt temp(a), temp2(b); return temp / temp2; }
	friend BigInt operator % (const BigInt &a, const unsigned long b){ BigInt temp(a), temp2(b); return temp % temp2; }

	//重载比较符
	friend bool operator < (const BigInt &a, const unsigned  long b){ BigInt temp(a), temp2(b); return temp < temp2; }
	friend bool operator == (const BigInt &a, const unsigned long b){ BigInt temp(a), temp2(b); return temp == temp2; }
	friend bool operator != (const BigInt &a, const unsigned long b){ BigInt temp(a), temp2(b); return temp != temp2; }
	friend bool operator <= (const BigInt &a, const unsigned long b){ BigInt temp(a), temp2(b); return temp <= temp2; }


	//将字符串进行补全 不足八位0补全 1234567 12345678-> 01234567 12345678
	friend string Fill_Zero(const string &str);
	//XYmodA=1 求x关于a的乘法逆元
	friend bool inverse(const BigInt &A, const BigInt &B, BigInt &result);
	//扩展欧几里得算法
	friend BigInt Euc(BigInt &a,BigInt &b,BigInt &x,BigInt &y);

	//蒙哥马利算法求：Y=Mon(X,A,B) -> X^A mod B=Y
	friend BigInt Mon(const BigInt &x, const BigInt &a, const BigInt &b);
	void ShowData();
	//去除高位为0 00123->123
	void Remove_Zero();
private:

};

BigInt Remove_Zero(const BigInt &a);


class RSA
{
public:
	RSA();
	~RSA();
	//判断是否有公私钥生成成功
	bool success = false;
	//RSA生成的公私密钥 =生成16进制位数的一半 安全性较高
	void Init(int len);
	//P和Q字符串长度
	int Length;
	//void ShowPri();
	//void ShowPub();
	bool Is_Valid_Prime(BigInt prime, int counts = 5);
private:
	bool CreateKey();
	//***************************************************************************
	//首先这个待测数N一定是一个奇数
	//所以N−1是一个偶数，而且一定可以写成2^s*d的形式，
	//选取一个介于[1, N−1]的整数a作为底。
	//把N−1写成了2^s*d的形式，得到了s和d，如果a^dmodN = 1，或者存在一个r∈[0, s−1]
	//使得a^(2rd)modN = −1（和a^(2rd)modN = N−1是等价的)
	//那么我们就称N通过了以a为底的Miller - Rabin素性测试，也就是说N有很大可能是素数。
	//*****************************************************************************
	//bool Is_Valid_Prime(BigInt prime, int counts = 5);
	//Miller-Rabin素性测试
	//True-是合数 False--为质数 a为底数
	bool Miller_Rabin(const BigInt &N, const BigInt &a, const BigInt &s, const BigInt &d);
	//生成lenth位的16进制字符串并转化为BigInt
	BigInt RandomString(int lenth);
	//随机生成小于a的lenth位的16进制字符串并转化为BigInt
	BigInt RandomString(const BigInt &a, int lenth);

	//P,Q为随机生成的两个素数
	BigInt P; 
	BigInt Q;
	//N=P*Q  R=(P-1)*(Q-1)
	BigInt N;
	BigInt R;
	//E一般为0x10001 D: ed=1(mod r) 关于r的乘法逆元
	BigInt E;
	BigInt D;
	void ShowData(const BigInt &a, char p);
};

