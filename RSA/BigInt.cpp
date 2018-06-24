#include "stdafx.h"
#include "BigInt.h"
#include <algorithm>

#define MaxUlong 0xFFFFFFFF
BigInt Zero((unsigned long)0);

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
	return temp;
}

//高位在后 低位在前
BigInt operator + (const BigInt &a, const BigInt &b){
	BigInt c;
	//确定符号,保证两数同号  
	if (a.Is_Negative == b.Is_Negative){
		c.Is_Negative = a.Is_Negative;
	}
	//+ - +
	else if (a.Is_Negative == true){
		return  b - Abs(a);
	}
	else if (b.Is_Negative == true){
		return a - Abs(b);
	}
	//补齐0使位数相同
	BigInt big, small;
	if (a.data.size() >= b.data.size()){
		big = a;
		small = b;
	}
	else{
		big = b;
		small = a;
	}
	//位数不同 用0补齐
	for (size_t i=0; i < big.data.size() - small.data.size(); i++){
		small.data.push_back(0);
	}
	vector<unsigned long>::const_iterator iter1, iter2;
	//进位
	int carry = 0;
	//判断是否超过0xFFFFFFFF 
	__int64 temp = 0;
	for (iter1 = big.data.begin(), iter2 = small.data.begin(); iter1 != big.data.end(); iter1++, iter2++){
		temp = (__int64)*iter1 + (__int64)*iter2 + carry;
		if (temp>MaxUlong){
			carry = 1;
			temp = temp - MaxUlong;
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
	return c;
}

BigInt operator - (const BigInt &a, const BigInt &b){
	BigInt c;
	BigInt big, small;
	//同号 判断正负 转化为 大+ - 小+
	if (a.Is_Negative == b.Is_Negative){
		if (Abs(a) < Abs(b)){
			big = Abs(b); small = Abs(a);
			c.Is_Negative = !a.Is_Negative;
		}
		else if (Abs(a) == Abs(b)){
			c = (unsigned long)0;
			return c;
		}
		else if (Abs(b) < Abs(a)){
			big = a; small = b;
			c.Is_Negative = a.Is_Negative;
		}
	}
	//- - +   -->    - + -  || + - -   ->    + + +
	else{
		return a + Negate(b);
	}
	//补全0使位数相同
	//借位
	int carry=0;
	__int64 result;
	vector<unsigned long>::iterator iter1 = big.data.begin(), iter2 = small.data.begin();
	for (size_t i = 0; i < big.data.size(); i++){
		if (big.data.size() != small.data.size()){
			small.data.push_back(0);
		}
		result = (__int64)*iter1 - (__int64)*iter1 - carry;
		if (result < 0){
			result += 0x100000000;
			carry = 1;
		}
		else{
			carry = 0;
		}
		c.data.push_back((unsigned long)result);
	}
	return c;
}

//	 A B C 
// D 1 2 3
// E 2 3 5
// F 3 4 5
BigInt operator * (const BigInt &a, const BigInt &b){
	BigInt c;
	if (a.Is_Negative == b.Is_Negative){
		c.Is_Negative = false;
	}
	else{
		c.Is_Negative = true;
	}
	//每一位之和
	unsigned __int64 sum_result = 0;
	unsigned __int64 sum_carry = 0;
	vector<unsigned long>::const_iterator iter1=a.data.begin(), iter2=b.data.begin();
	for (size_t i = 0; i <= a.data.size() + b.data.size() - 2; i++){
		//先加上之前的进位
		sum_result += sum_carry;
		for (size_t j = 0; j <= i; j++){
			unsigned __int64 result;
			result = (unsigned __int64)*(iter1 + j)*(unsigned __int64)*(iter2 + i - j);
			//小进位 long 32位
			sum_carry += result >> 32;
			//取后32位
			sum_result += result & MaxUlong;
		}
		//第i位上的总进位和结果
		if (sum_result > MaxUlong){
			sum_carry += sum_result >> 32;
			sum_result = sum_carry & MaxUlong;
		}
		c.data.push_back((unsigned long)sum_result);
	}
	while (sum_carry>0){
		c.data.push_back((unsigned long)sum_carry & MaxUlong);
		sum_carry = sum_carry >> 32;
	}
	return c;
}

// a/b
BigInt operator / (const BigInt &a, const BigInt &b){
	//判断正负
	BigInt c, big = Abs(a), small = Abs(b);
	if (a.Is_Negative == b.Is_Negative){
		c.Is_Negative = false;
	}
	else{
		c.Is_Negative = true;
	}
	//a<b
	if (big < small){
		return Zero;
	}
	//a=b
	else if (big == small){
		c.data.push_back(1);
		return c;
	}
	//a>b
	else{
		//以0填充C,假设有big-small+1位
		for (size_t i = 0; i < big.data.size() - small.data.size() + 1; i++){
			c.data.push_back(0);
		}
		//
		unsigned __int64 result;
		int len;
		vector<unsigned long>::reverse_iterator iter1, iter2 ;
		while (small < big){
			iter1 = big.data.rbegin();
			iter2 = small.data.rbegin();
			if (*iter1>*iter2){
				//高位相除
				result = (unsigned __int64)(*iter1) / ((unsigned __int64)(*iter2) + 1);
				len = big.data.size() - small.data.size();
			}
			else if (big.data.size() > small.data.size()){
				len = big.data.size() - small.data.size() - 1;
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
			c.data[len] = (unsigned long)result;
			big = big - small*result;
		}
		if (big == small){
			c = c + 1;
		}
	}
	c.Remove_Zero();
	return c;
}

BigInt operator % (const BigInt &a, const BigInt &b){
	//the same to /
	BigInt big = Abs(a), small = Abs(b);
	//a<b
	if (big < small){
		return big;
	}
	//a=b
	else if (big == small){
		return Zero;
	}
	//a>b
	else{
		//
		unsigned __int64 result;
		int len;
		vector<unsigned long>::reverse_iterator iter1, iter2;
		while (small < big){
			iter1 = big.data.rbegin();
			iter2 = small.data.rbegin();
			if (*iter1>*iter2){
				//高位相除
				result = (unsigned __int64)(*iter1) / ((unsigned __int64)(*iter2) + 1);
				len = big.data.size() - small.data.size();
			}
			else if (big.data.size() > small.data.size()){
				len = big.data.size() - small.data.size() - 1;
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
			//位数相同 且 高位相同 999/900 99 结束
			else{
				big = big - small;
				break;
			}
			big = big - small*result;
		}
		if (big == small){
			return Zero;
		}
	}
	big.Remove_Zero();
	return big;
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
			vector<unsigned long>::const_reverse_iterator iter1=a.data.rbegin(), iter2=b.data.rbegin();
			for (size_t i = 0; i < a.data.size(); i++){
				//-9 < -1,  T  9 <1 F
				if (*iter1>*iter1){
					return a.Is_Negative;
				}
				//-1<-9  F, 1<9 T
				else if (*iter1 < *iter1){
					return !a.Is_Negative;
				}
			}
			//遍历结束，相等
			return false;
		}
	}
	else{
		return b.Is_Negative;
	}
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
	vector<unsigned long>::const_reverse_iterator iter=this->data.rbegin();
	while (*iter == 0 && this->data.size() > 1){
		data.pop_back();
		iter = this->data.rbegin();
	}
}

bool CopyFromHexStr(BigInt &dst, const string &str){
	BigInt temp;
	temp.data.clear();
	const char *p = str.c_str();
	if (*p == '-'){
		temp.Is_Negative = true;
		p++;
	}
	else if (*p == '0' && (*p == 'x' || *p == 'X')){
		p = p + 2;
	}
	unsigned long num = 0;
	//第i位
	int len;
	//HEX 0:30  A:41 a:61  long:FFFFFFFF
	for (size_t i = 0; i < str.length(); i++){
		len = i % 8;
		if (*p >= '0'&&  *p <= '9'){
			num += (*p - '0') << (4 * len);
		}
		else if (*p >= 'a'&&*p <= 'z'){
			num += (*p - 0x37) << (4 * len);
		}
		else if (*p >= 'A'&&*p <= 'Z'){
			num += (*p - 0x57) << (4 * len);
		}
		else{
			return false;
		}
		//每八个字符为一组
		if (len == 0 && i > 0){
			temp.data.push_back(num);
			num = 0;
		}
	}
	reverse(temp.data.begin(), temp.data.end());
	dst = temp;
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