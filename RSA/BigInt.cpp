#include "stdafx.h"

#include "BigInt.h"
#include <algorithm>
#include <time.h>
#include <Windows.h>


#define MaxUlong 0xFFFFFFFF

static BigInt Zero((unsigned long)0);
static BigInt One((unsigned long)1);
static BigInt Two((unsigned long)2);

BigInt::BigInt(){
	data.push_back(0);
	Is_Negative = false;
}

BigInt::BigInt(unsigned long number){
	data.clear();
	Is_Negative = false;
	data.push_back(number);
	this->Remove_Zero();
}

BigInt::BigInt(unsigned __int64 number){
	data.clear();
	Is_Negative = false;
	//先低位再高位
	data.push_back((unsigned long)(number & 0xFFFFFFFF));
	data.push_back((unsigned long)(number >> 32));
	this->Remove_Zero();
}

BigInt::BigInt(const string &str, bool is_fail){
	is_fail = !CopyFromHexStr(*this, str);
	this->Remove_Zero();
}

BigInt::BigInt(const BigInt &a){
	data.clear();
	Is_Negative = a.Is_Negative;
	//去除多余的0
	data = a.data;
	this->Remove_Zero();
}

BigInt::~BigInt(){
	data.clear();
}

BigInt Abs(const BigInt &a){
	BigInt temp(a);
	if (a.Is_Negative){
		temp.Is_Negative = false;
	}
	return temp;
}

BigInt Negate(const BigInt &a){
	BigInt temp(a);
	if (a.Is_Negative){
		temp.Is_Negative = false;
	}
	else
	{
		temp.Is_Negative = true;
	}
	return Remove_Zero(temp);
}

//高位在后 低位在前
BigInt operator + (const BigInt &a, const BigInt &b){
	BigInt c;
	c.data.clear();
	//确定符号,保证两数同号  
	if (a.Is_Negative == b.Is_Negative){
		c.Is_Negative = a.Is_Negative;
	}
	//+ - +
	else if (a.Is_Negative == true){
		return  Remove_Zero(b - Abs(a));
	}
	else if (b.Is_Negative == true){
		return Remove_Zero(a - Abs(b));
	}
	//补齐0使位数相同
	BigInt big, Small;
	if (a.data.size() >= b.data.size()){
		big = a;
		Small = b;
	}
	else{
		big = b;
		Small = a;
	}
	//位数不同 用0补齐
	int len = big.data.size() - Small.data.size();
	for (int i = 0; i < len; i++){
		Small.data.push_back(0);
	}
	vector<unsigned long>::const_iterator iter1, iter2;
	//进位
	int carry = 0;
	//判断是否超过0xFFFFFFFF 
	__int64 temp = 0;
	for (iter1 = big.data.begin(), iter2 = Small.data.begin(); iter1 != big.data.end(); iter1++, iter2++){
		temp = (__int64)*iter1 + (__int64)*iter2 + carry;
		if (temp>MaxUlong){
			carry = 1;
			temp = temp & MaxUlong;
		}
		else{
			carry = 0;
		}
		c.data.push_back((unsigned long)temp);
	}
	//存在进位
	if (carry > 0){
		c.data.push_back(carry);
	}
	return Remove_Zero(c);
}

BigInt operator - (const BigInt &a, const BigInt &b){
	BigInt c;
	c.data.clear();
	BigInt Big, Small;
	//同号 判断正负 转化为 大+ - 小+
	if (a.Is_Negative == b.Is_Negative){
		if (Abs(a) < Abs(b)){
			Big = Abs(b); Small = Abs(a);
			c.Is_Negative = !a.Is_Negative;
		}
		else if (Abs(a) == Abs(b)){
			return Zero;
		}
		else if (Abs(b) < Abs(a)){
			Big = a; Small = b;
			c.Is_Negative = a.Is_Negative;
		}
	}
	//- - +   -->    - + -  || + - -   ->    + + +
	else{
		BigInt temp = a + Negate(b);
		return Remove_Zero(temp);
	}
	//补全0使位数相同
	while (Big.data.size() != Small.data.size()){
		Small.data.push_back(0);
	}
	//借位
	int carry = 0;
	//需要判断正负
	__int64 result;
	vector<unsigned long>::iterator iter1 = Big.data.begin(), iter2 = Small.data.begin();
	for (size_t i = 0; i < Big.data.size(); i++){
		result = (__int64)*iter1 - (__int64)*iter2 - carry;
		if (result < 0){
			result += 0x100000000;
			carry = 1;
		}
		else{
			carry = 0;
		}
		c.data.push_back((unsigned long)result);
		iter1++;
		iter2++;
	}
	return Remove_Zero(c);
}

//	 A B C 
// D 1 2 3  0,0  0,1 0,2 
// E 2 3 4  1,0
// F 3 4 5	2,0
// G 4 5 6  3,0  3,1 3,2
BigInt operator * (const BigInt &A, const BigInt &B){
	BigInt c,a(A),b(B);
	c.data.clear();
	if (a.Is_Negative == b.Is_Negative){
		c.Is_Negative = false;
	}
	else{
		c.Is_Negative = true;
	}
	//每一位之和
	unsigned __int64 sum_result = 0;
	unsigned __int64 sum_carry = 0;
	//防止sum_carry超出下下一位
	unsigned __int64 sum_carry2 = 0;
	for (size_t i = 0; i < a.data.size() + b.data.size() - 1; i++){
		//复原
		sum_result = sum_carry;
		sum_carry = sum_carry2;
		sum_carry2 = 0;
		for (size_t j = 0; j <= i; j++){
			unsigned __int64 result;
			if (j > a.data.size() - 1){
				continue;
			}
			else if (i - j> b.data.size() - 1){
				continue;
			}
			else{
				unsigned __int64 temp1 = a.data[j];
				unsigned __int64 temp2 = b.data[i - j];
				result = temp1*temp2;//小进位 long 32位
				sum_carry += result >> 32;
				if (sum_carry > MaxUlong){
					sum_carry2 += sum_carry >> 32;
					sum_carry = sum_carry&MaxUlong;
				}
				//取后32位
				sum_result += result & MaxUlong;
			}
		}
		//第i位上的总进位和结果
		if (sum_result > MaxUlong){
			sum_carry += sum_result >> 32;
			sum_result = sum_result & MaxUlong;
		}
		c.data.push_back((unsigned long)sum_result);

	}
	while (sum_carry>0){
		c.data.push_back((unsigned long)sum_carry & MaxUlong);
		sum_carry = sum_carry >> 32;
	}
	return Remove_Zero(c);
}

// a/b
BigInt operator / (const BigInt &a, const BigInt &b){
	//判断正负
	BigInt c, big = Abs(a), Small = Abs(b);
	c.data.clear();
	if (a.Is_Negative == b.Is_Negative){
		c.Is_Negative = false;
	}
	else{
		c.Is_Negative = true;
	}
	//a<b
	if (big < Small){
		return Zero;
	}
	//a=b
	else if (big == Small){
		c.data.push_back(1);
		return Remove_Zero(c);
	}
	//a>b
	else{
		//以0填充C,假设有big-Small+1位
		for (size_t i = 0; i < big.data.size() - Small.data.size() + 1; i++){
			c.data.push_back(0);
		}
		//
		unsigned __int64 result;
		int len;
		vector<unsigned long>::reverse_iterator iter1, iter2;
		while (Small < big){
			iter1 = big.data.rbegin();
			iter2 = Small.data.rbegin();
			if (*iter1>*iter2){
				//高位相除
				result = (unsigned __int64)(*iter1) / ((unsigned __int64)(*iter2) + 1);
				len = big.data.size() - Small.data.size();
			}
			else if (big.data.size() > Small.data.size()){
				len = big.data.size() - Small.data.size() - 1;
				//B高两位/S高一位
				result = *iter1;
				result = (result << 32) + *(iter1 + 1);
				//0xFFFFFFFF + 1 
				if (*iter2 == MaxUlong){
					result = result >> 32;
				}
				else{
					result = result / ((unsigned __int64)(*iter2) + 1);
				}
			}
			//位数相同 且 高位相同 999/900 +1 结束
			else{
				c = c + 1;
				break;
			}
			c.data[len] += (unsigned long)result;
			//result *位数
			BigInt temp;
			temp.data.clear();
			while (temp.data.size() != len){
				temp.data.push_back(0);
			}
			temp.data.push_back((unsigned long)result);
			big = big - Small*temp;
		}
		if (big == Small){
			c = c + 1;
		}
	}
	return Remove_Zero(c);
}

BigInt operator % (const BigInt &a, const BigInt &b){
	//the same to /
	//除数默认为正
	BigInt big = Abs(a), Small = Abs(b);
	//a<b
	if (big < Small){
		//a<0 
		if (big.Is_Negative){
			return a + b;
		}
		else{
			return big;
		}
	}
	//a=b
	else if (big == Small){
		return Zero;
	}
	//a>b
	else{
		unsigned __int64 result;
		int len;
		vector<unsigned long>::reverse_iterator iter1, iter2;
		while (Small < big){
			iter1 = big.data.rbegin();
			iter2 = Small.data.rbegin();
			if (*iter1>*iter2){
				//高位相除
				result = (unsigned __int64)(*iter1) / ((unsigned __int64)(*iter2) + 1);
				len = big.data.size() - Small.data.size();
			}
			else if (big.data.size() > Small.data.size()){
				len = big.data.size() - Small.data.size() - 1;
				//B高两位/S高一位
				result = *iter1;
				result = (result << 32) + *(iter1 + 1);
				//0xFFFFFFFF + 1 
				if (*iter2 == MaxUlong){
					result = result >> 32;
				}
				else{
					result = result / ((unsigned __int64)(*iter2) + 1);
				}
			}
			//位数相同 且 高位相同 999/900 +1 结束
			else{
				break;
			}
			//result *位数
			BigInt temp;
			temp.data.clear();
			while (temp.data.size() != len){
				temp.data.push_back(0);
			}
			temp.data.push_back((unsigned long)result);
			big = big - Small*temp;
		}
		if (Small == big){
			return Zero;
		}
	}
	return Remove_Zero(big);
}

bool operator < (const BigInt &a, const BigInt &b){
	if (a.Is_Negative == b.Is_Negative){
		//-11<-9 T  11<9  F
		if (a.data.size()>b.data.size()){
			if (a.Is_Negative){
				return true;
			}
			else{
				return false;
			}
		}
		//-9<-11 F 9<11 T
		else if (a.data.size() < b.data.size()){
			if (!a.Is_Negative){
				return true;
			}
			else{
				return false;
			}
		}
		else{
			//反向遍历
			vector<unsigned long>::const_reverse_iterator iter1 = a.data.rbegin(), iter2 = b.data.rbegin();
			for (size_t i = 0; i < a.data.size(); i++){
				//-9 < -1,  T  9 <1 F
				if (*iter1 > *iter2){
					return a.Is_Negative;
				}
				//-1<-9  F, 1<9 T
				else if (*iter1 < *iter2){
					return !a.Is_Negative;
				}
				iter1++;
				iter2++;
			}
			//遍历结束，相等
			return false;
		}
	}
	//异号  + < -  false   - < +  true
	else{
		return a.Is_Negative;
	}
}

bool operator <= (const BigInt &a, const BigInt &b){
	if (a < b || a == b){
		return true;
	}
	return false;
}

bool operator == (const BigInt &a, const BigInt &b){
	if (a.data.size() == b.data.size()){
		vector<unsigned long>::const_iterator iter1 = a.data.begin(), iter2 = b.data.begin();
		for (size_t i = 0; i < a.data.size(); i++){
			if (*iter1 != *iter2){
				return false;
			}
		}
		return true;
	}
	return false;
}

bool operator != (const BigInt &a, const BigInt &b){
	return !(a == b);
}

void BigInt::Remove_Zero(){
	if (this->data.size() > 0){
		vector<unsigned long>::const_reverse_iterator iter = this->data.rbegin();
		while (*iter == 0 && this->data.size() > 1){
			data.pop_back();
			iter = this->data.rbegin();
		}
	}
}

BigInt Remove_Zero(const BigInt &a){
	BigInt temp(a);
	temp.Remove_Zero();
	return temp;
}


bool CopyFromHexStr(BigInt &dst, const string &str){
	BigInt temp;
	temp.data.clear();
	const char *p = str.c_str();
	if (*p == '-'){
		temp.Is_Negative = true;
		p++;
	}
	else if (*p == '0' && (*(p + 1) == 'x' || *(p + 1) == 'X')){
		p = p + 2;
	}
	//需要进行字符串补全方便反转 8位 1234567 81234567--> 01234567 81234567
	string data = Fill_Zero(p);
	unsigned long num = 0;
	p = data.c_str();
	//第i位
	int len;
	//HEX 0:30  A:41 a:61  long:FFFFFFFF
	for (size_t i = 0; i < data.length(); i++){
		unsigned long a = 0;
		len = i % 8;
		if (*p >= '0'&&  *p <= '9'){
			a = *p - '0';
		}
		else if (*p >= 'a'&&*p <= 'f'){
			a = *p - 'a' + 10;
		}
		else if (*p >= 'A'&&*p <= 'F'){
			a = *p - 'A' + 10;
		}
		else{
			return false;
		}
		num += a << (28 - len * 4);
		//每八个字符为一组 
		if (len == 7){
			temp.data.push_back(num);
			num = 0;
		}
		p++;
	}
	//进行反转 temp:高位再前 低位在后
	dst.data.assign(temp.data.rbegin(), temp.data.rend());
	dst.Is_Negative = temp.Is_Negative;
	return true;
}

//待写
bool PasteHexStr(string &str, const BigInt &a){
	str = "";
	char Hex[] = { 0x00, };
	for (vector<unsigned long>::const_reverse_iterator iter = a.data.rbegin(); iter != a.data.rend(); iter++){
		unsigned temp = *iter;

	}
	return true;
}

void BigInt::ShowData(){
	if (Is_Negative){
		printf("-");
	}
	printf("0x");
	vector<unsigned long>::reverse_iterator iter = data.rbegin();
	for (iter; iter != data.rend(); iter++){
		if (iter == data.rbegin()){
			printf("%X", *iter);
		}
		else{
			printf("%08X", *iter);
		}
	}
	printf("\n");
}



string Fill_Zero(const string &str){
	string result = str;
	int num = result.length() % 8;
	//=0
	if (num){
		//补全0的个数
		num = 8 - num;
		char *temp = new char[num + 1];
		memset(temp, '0', num);
		temp[num] = '\0';
		//开头补全0
		result.insert(0, temp);
	}
	return result;
}

// this * y mod a =1
BigInt Euc(BigInt &a,BigInt &b,BigInt &x,BigInt &y){
	if (b == Zero){
		x = One;
		y = Zero;
		return Remove_Zero(a);
	}
	BigInt r=Euc(b, a%b, x, y);
	BigInt flag = x;
	x = y;
	y = flag - ((a / b) * y);
	x=Remove_Zero(x);
	y=Remove_Zero(y);
	//r为最大公约数
	return Remove_Zero(r);
}

bool inverse(const BigInt &A, const BigInt &B, BigInt &result){
	BigInt x, y, a(A), b(B);
	Euc(a, b, x, y);
	if (Euc(a, b, x, y) == One){
		if (x.Is_Negative){
			x = x + b;
		}
		result = x;
		return true;
	}
	printf("两数最大公约数不为1\n");
	return false; 
}


BigInt Mon(const BigInt &C, const BigInt &A, const BigInt &B){
	BigInt x(One), y(C), z(A);
	while (z.data.size() != 1 || z.data[0])
	{
		if (z.data[0] & 1)
		{
			z = z - One;
			x = x*y%B;
		}
		else
		{
			z = z / 2;
			y = y*y%B;
		}
	}
	return x;
}

RSA::RSA(){
	//
	CopyFromHexStr(N, "10001");
	success = false;
}

//P,Q为len/2位 /4 二进制转16进制
void RSA::Init(int len){
	if (len % 8 != 0){
		success = false;
	}
	Length = len / 8;
	//P!=Q
	while (P == Q){
		CreateKey();
	}
	printf("Create key success!\n");
	P.ShowData();
	Q.ShowData();
	success = true;
}

RSA::~RSA(){

}

bool RSA::CreateKey(){
	//P
	P=RandomString(Length);
	CopyFromHexStr(P, "1171F25658D3173A1A370B63210B2AEF");
	do{
		P.ShowData();
		P = P + Two;
	} while (!Is_Valid_Prime(P));
	//Q
	Q=RandomString(Length);
	do{
		Q = Q + Two;
	} while (!Is_Valid_Prime(Q));
	return true;
}

BigInt RSA::RandomString(int lenth){
	if (lenth == 1){
		string str("1");
		return str;
	}
	char HexString[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	char *temp = new char[lenth + 1];
	temp[lenth] = '\0';
	//保证生成奇数
	temp[lenth - 1] = '1';
	//首位不为0
	srand((unsigned)time(NULL));
	temp[0] = HexString[rand() % 15 + 1];
	for (size_t i = 0; i < unsigned long(lenth - 1); i++){
		temp[i] = HexString[rand() % 16];
	}
	BigInt result(temp);
	return result;
}

//生成小于a的数
BigInt RSA::RandomString(const BigInt &a, int lenth){
	BigInt result;
	do
	{
		unsigned long high = a.data[a.data.size() - 1];
		//bool success = false;
		////随机数最高位
		high = rand() % high;
		//在一个long内
		if (lenth <= 8){
			result = high;
		}
		else{
			BigInt temp = RandomString(Length - 8);
			temp.data.push_back(high);
			result = temp;
		}
	} while (result==Zero);
	return result;
}

bool RSA::Is_Valid_Prime(BigInt prime, int counts){
	//偶数或2
	if (prime == Two){
		return true;
	}
	if (prime < Two && prime % Two == 0){
		return false;
	}
	//2^s *d
	BigInt s = Zero, d = prime - 1;
	while (d % 2 == 0){
		s = s + 1;
		d = d / 2;
	}
	/*printf("%d=2^%d*%d+1", prime,s, d);*/
	srand((unsigned)time(NULL));
	for (int i = 0; i < counts; i++){
		BigInt a = RandomString(prime, Length);
		if (Miller_Rabin(prime, a, s, d)){
			return false;
		}
	}
	return true;
}

bool RSA::Miller_Rabin(const BigInt &N, const BigInt &a, const BigInt &s, const BigInt &d){
	//a^dmodN = 1
	BigInt result = Mon(a, d, N);
	if (result == One){
		return false;
	}
	//a^(2^r*d)modN = −1（和a^(2^r*d)modN = N−1
	BigInt pow_2_i = One;
	for (BigInt i = Zero; i < s; i = i + 1){
		/*   powl(2, i)*d;*/
		result = pow_2_i*d;
		pow_2_i = pow_2_i*Two;
		result = Mon(a, result, N);
		result = result%N;
		if (result == N - 1){
			return false;
		}
	}
	return true;
}

