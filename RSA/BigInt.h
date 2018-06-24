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
	~BigInt();
	//用于存储大数 1024=32*long  
	//高位在后低位在前，方便进位
	vector<unsigned long> data;
	//是否为负数 True:-   False:+/0
	bool Is_Negative;

	//字符串16进制(只能包含0-F)转BigInt
	//字符串为正常大小 高位在前,低位在后 
	friend bool CopyFromHexStr(BigInt &dst,const string &str);
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
	//重载运算符 BigInt 和 long
	friend BigInt operator + (const BigInt &a, const unsigned long b){ BigInt temp(b); return a + temp; }
	friend BigInt operator - (const BigInt &a, const unsigned long b){ BigInt temp(b); return a - temp; }
	friend BigInt operator * (const BigInt &a, const unsigned long b){ BigInt temp(b); return a * temp; }
	friend BigInt operator * (const BigInt &a, const unsigned __int64 b){ BigInt temp(b); return a * temp; }
	friend BigInt operator / (const BigInt &a, const unsigned long b){ BigInt temp(b); return a / temp; }
	friend BigInt operator % (const BigInt &a, const unsigned long b){ BigInt temp(b); return a % temp; }

	friend bool operator < (const BigInt &a, const unsigned  long b){ BigInt temp(b); return a < temp; };
	friend bool operator == (const BigInt &a, const unsigned long b){ BigInt temp(b); return a == temp; };
	friend bool operator != (const BigInt &a, const unsigned long b){ BigInt temp(b); return a != temp; };

private:
	//去除高位为0 00123->123
	void Remove_Zero();
};


BigInt::BigInt(){
	data.clear();
	Is_Negative = false;
}

BigInt::BigInt(unsigned long number){
	data.clear();
	Is_Negative = false;
	data.push_back(number);
}

BigInt::BigInt(unsigned __int64 number){
	data.clear();
	Is_Negative = false;
	data.push_back((unsigned long)(number >> 32));
	data.push_back((unsigned long)(number & 0xFFFFFFFF));	
}

BigInt::BigInt(const BigInt &a){
	data.clear();
	Is_Negative = a.Is_Negative;
	data.assign(a.data.begin(), a.data.end());
}

BigInt::~BigInt(){
	data.clear();
}