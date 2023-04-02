#pragma once
#include <cstring>
#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include "Exception.h"
#include "StringExtension.h"
#include "Array.h"
//#include "SharedPtr.h"
#define COPY_STRING_BUFFER_SIZE 64
#define COPY_INT_BUFFER_SIZE 12
//int releaseCount = 0;
//char* lastAddress = nullptr;
/**
 * - 在应对多个字符串之间的操作时
 * 这个类的效率比String类高
 * - 这个类可以帮助拼接字符串, 防止因直接使用
 * String + String 拼接而造成的内存泄漏问题(问题1)
 * - 以上问题(问题1)已大幅缓解, String的相关方法大部分已
 * 采用返回拷贝临时变量的方式实现跨栈需求(临时变量全部都在栈中创建)
 * 进而不需要每次在拼串生成新串时都将其放入堆中
 * 进而避免手动管理堆(delete)带来的问题
 */
class StringBuilder {
		vector<String> menbers;
	public:
		~StringBuilder() {}
		// 拷贝构造, 方便临时变量传参(缺省的就是按字节拷贝,其实也没必要写)
		StringBuilder(const StringBuilder& builder): menbers(builder.menbers) {};

		StringBuilder() {}

		StringBuilder& insert(const String& target, const int index);

		StringBuilder& append(const String& target);
		StringBuilder& add(const String& target) {return append(target);};
		StringBuilder& push(const String& target) {return append(target);};

		// 第一个erase顺序查询,时间复杂度为 O(mn) (m为target长度n为menbers长度)
		StringBuilder& erase(const String& target);
		StringBuilder& erase(const int index);
		StringBuilder& remove(const String& target) {return erase(target);};
		StringBuilder& remove(const int index) {return erase(index);};
		/**
		 * @brief
		 * 构造完字符串后,是否清除builder里缓存的String, 以便于下次继续拼接
		 */
		String build(bool clear = true);

		inline StringBuilder& operator+(const String& target);
		// 与erase(const String& target)等同
		inline StringBuilder& operator-(const String& target);
		// 与erase(const int index)等同
		inline StringBuilder& operator-(const int index);
                            
		inline StringBuilder& operator*(const int repeatCount);
                            
		inline StringBuilder& operator/(const int eraseProportion);
		/**
		 * @brief
		 * 外部可能会将 stringBuilder[index]作为左值进行赋值操作
		 * 故而返回对应索引的引用
		 */
		inline String& operator[](int index) {
			int size = menbers.size();
			if (index < 0 || index >= size) {
				throw OutOfBoundException(index, 0, size - 1);
			}
			return menbers[index];
		}

		inline const size() const {
			return menbers.size();
		}
};



/**
 * String类index的说明:
 * String维护的char[] index 可以用下行表示:
 *   a   b   c
 * 0   1   2   3
 * 所有的index的含义就是以上, 比如使用subString(1,3)截取出的子串就是 "bc"
 */
class String {
		/**
		 * 字符串的长度
		 */
		int _length = 0;
		/**
		 * - 存放字符串的char数组，初始化data为只有一个结束符的空串
		 * - 用共享指针主要原因是: 可以实现String栈变量的自动回收
		 * 在该String依赖的char*没有被所有的String引用时, 就会自动回收位于堆中的char数组
		 * (shared_ptr在构造时将引用计数+1,析构时-1,==0时delete回收, 不同的shared_ptr通过=运算符,将右值
		 * 的引用计数以及维护的指针交由左值,实现普通指针的引用传递)
		 * 解决了这个矛盾:
		 * 1. String在析构时理应 delete char* 释放最占空间的char数组部分
		 * 2. 生成临时String实例的拷贝构造仅仅只是复制了一下char* 指针值, 没有拷贝整个char数组,属于浅拷贝
		 * 3. 临时String在析构时, delete char* 会提前释放外部调用者String所维护的char[],
		 *    导致外部String在调用了会生成临时String的方法后, 无法再访问其维护的char[]
		 * 解决方案:
		 * 1: 在拷贝构造中, 不浅拷贝char 数组, 而是占用新堆内存, 遍历拷贝原char数组
		 *    这样临时变量在析构时,只会释放掉临时变量所对应的char[]数组, 不会提前释放外部String的char[]
		 * 2: 使用shared_ptr来解决
		 * 分析:
		 * --第一个方案很直接, 但是空间时间复杂度都很高,临时变量的产生本就很频繁, 且若在拷贝构造中
		 * 频繁的申请堆内存,在析构中频繁地释放堆内存, 还容易制造出内存碎片
		 * --第二个方案, 解决了第一个方案的所有问题, 但因为拷贝构造中仅仅只对
		 * char* 做了浅拷贝(data=target.data使用了shared_ptr的重载=运算符,会将target.data指向的char*
		 * 赋给this的data,进而实现引用计数功能), 使用临时变量进行操作会影响到外部String的char[]
		 * (毕竟说白了临时String和外部String都用的是同一个char[])
		 */
		// char* data = new char[1] {0};
		SharedPtr<char*> data = getIinitialzedSharedPointer();
//		shared_ptr<char[]> data = getIinitialzedSharedPointer(/*lastAddress*/); //make_shared<char[]>(nullptr); //make_shared<char*>(new char[1] {0});
		//shared_ptr<char[]>(new char[1] {0}, releaser /*[](char*p){delete p;} *//*default_delete<char[]>()*/, allocator<char[]>());

		/**
		 * @brief 将常量池中的const char*字符串复制进堆中的工具方法
		 * @param length 产生的复制字符串长度
		 * @param source 被复制的字符串范本
		 */
		void copyAndReplace(const char* source, int length);
		/**
		 * @brief 对c++内部string类进行一个转换
		 */
		inline void copyAndReplaceByCppString(const string& cppString);
		/**
		 * @brief 计算出模式串的 nextValue数组
		 * @return 前缀表/后缀表 , nextValue数组
		 */
		static int* const getPrefixTable(const String& search);
		static int* const getSuffixTable(const String& search);

		/**
		 * @brief 用来确定非空格字符的起始位置的
		 */
		inline int ensureNotBlankEndIndex(int endIndex);
		inline int ensureNotBlankStartIndex(int startIndex);

		static void releaser(char* p) {
//			cout << "target: " << p << "count: " << ++releaseCount << "address update? " << (p == lastAddress) << endl;
			delete[] p;
		}

		/**
		 * @brief
		 * 工厂方法, 用shared_ptr包装一下传入的char*指针,然后返回
		 */
		inline const static SharedPtr<char*> getIinitialzedSharedPointer(char* pointer = new char[1] {0})  {
//			lastAddress = new char[1] {0};
			return SharedPtr<char*>(
			           pointer,//lastAddress,
			           SharedPtr<char*>::arrayDellocator /*[](char*p){delete p;} *//*default_delete<char[]>()*/
			       );
		}

		static void checkReplaceIndex(int index, function<bool()> prediction, const char* msg);
	public:
		/**
		 * @brief 定义几个与方向相关的静态常量,语义化
		 * 		  在该类里，所有与方向相关的方法都可以用这几个常量
		 */
		const static int LEFT = 1;
		const static int RIGHT = 2;
		const static int BOTH = 3;
		const static int LOWER = 1;
		const static int UPPER = 2;
		String();
		~String();

		String(const String& source);
		String(char* source); // 不拷贝, 直接将source作为data
		String(const char* source);// 拷贝, 将原本不可变的拷入堆中, 方便操作
		String(const char* source, int length);// 指定长度进行拷贝, 长度大于等于字符串长度则只拷贝其原本长度
		String(const string& cppString);// 得到char* 并拷贝

		// 包装了转化方法的构造器
		String(const int num);
		String(const unsigned num);
		String(const long num);
		String(const long long num);
		String(const unsigned long num);
		String(const unsigned long long num);
		String(const float num);
		String(const double num);
		String(const vector<String> strs);
		
		/**
		 * @brief 将目标串插入到本串的指定索引的位置上
		 * @param target 将被插入的目标串
		 * @param position 将要插入该串的索引值
		 * @param merge 返回结果是否影响本串，若不影响，则本串的字符串不会被改变
		 * 				只是创建一个新串来保存返回结果
		 * @return 创建出的新串，或者是该串本身
		 */
		String insert(const String& target, int position, bool merge = false);
		// 返回startIndex到endIndex的子串,endIndex默认-1,意为截取到末尾
		String subString(int startIndex, int endIndex  = -1);

		// 返回第一次出现该字符的索引, 若未出现则返回-1
		int indexOf(const char search);
		/**
		 * @brief 使用 KMP 算法,返回第index次出现的search子串的索引,
		 * index从0开始,意为第一次出现的地方,若未找到则返回-1
		 */
		int indexOfthe(const String& search, int index);
		//默认返回找到的子串从左开始第一次出现的索引
		int indexOf(const String& search);
		int lastIndexOf(const char search);
		int lastIndexOf(const String& search);
		// 返回所有相同子串的索引
		vector<int> allIndexOf(const String& search);


		// 是否包含该字符/子串
		bool contain(const char search);
		inline bool contain(const String& search);

		/**
		 * @brief
		 * 双指针法翻转字符串
		 */
		String reverse(bool merge = false);

		/**
		 * @brief 是否以前缀prefix/后缀suffix开头/结尾
		 */
		bool startsWith(const String& prefix);
		bool endsWith(const String& suffix);

		/**
		 * @brief 作用同startsWith(const String& prefix)与endsWith(const String& suffix)
		 */
		bool hasPrefix(const String& prefix);
		bool hasSuffix(const String& suffix);

		/**
		 * @brief 比较两个串的ASCII码值的大小
		 * @param target 要比较的那个字符串
		 * @return 两串的ASCII码差值
		 * 相等就为0, 不等就非0, 一般串长的ASCII码大些(当两串足够长且各字符出现频率相近时)
		 * 返回不意味着两串是内容相同的串
		 */
		int compareTo(String& target);

		/**
		 * @return 字符串的长度
		 */
		inline const int length() const;
		/**
		 * @brief 切分方法
		 * @param startIndex 从startIndex开始切分
		 * @param charCount 往后切分charCount个字符
		 * @param merge 可以选择是否影响本串(是否将切分结果保存入本串)
		 */
		String slice(int startIndex, int charCount = -1, bool merge = false);
		String slice(int startIndex, bool merge = false); // 默认切分到末尾
		String sliceSelf(int startIndex, int charCount = -1){return splice(startIndex,charCount);}
		inline String splice(int startIndex, int charCount = -1); // 默认切分到末尾, 默认影响本串

		/**
		 * @brief 用with串替换第1/最后一次出现的search子串,若不含有该子串，则不替换
		 * @param search 要被替换的串
		 * @param with 替换成的串
		 * @param merge 该方法的执行是否影响原串
		 * @param direction 替换的方向,可以使用String的枚举值来指定从哪个方向开始替换
		 * @return 完成替换后的串
		 */
		String replace(const String& search, const String& with = "", bool merge = false, int direction = LEFT, int index = 0);
		inline String replaceFirst(const String& search, const String& with = "", bool merge = false);
		inline String replaceLast(const String& search, const String& with = "", bool merge = false);
		// 不填with,效果同deleteAll
		String replaceAll(const String& search, const String& with = "", bool merge = false);
		String replaceByIndex(const String& search, const String& with = "", bool merge = false, int replaceIndex = 0);
		//删除所有匹配的子串
		inline String deleteAll(const String& search, bool merge = false);
		inline String removeAll(const String& search, bool merge = false) {return deleteAll(search, merge);}

		// 以下replaceSelf方法均会影响原串
		inline String replaceSelf(const String& search, const String& with = "", int direction = LEFT);
		inline String replaceSelfFirst(const String& search, const String& with = "");
		inline String replaceSelfLast(const String& search, const String& with = "");
		inline String replaceSelfAll(const String& search, const String& with = "");
		inline String replaceSelfByIndex(const String& search, const String& with = "", int replaceIndex = 0);
		/**
		 * @brief 去除空格
		 * @param trimMode 传入枚举常量 String::LEFT, RIGHT, BOTH 来标识需要删除哪边的空格
		 * @param merge 是否影响本串，否就创新串，是则修改自身并返回自身
		 * @return
		 */
		String trim(int trimMode = BOTH, bool merge = false);
		inline String trimLeft(bool merge = false);
		inline String trimRight(bool merge = false);
		// 以下方法均会影响自身
		inline String trimSelf(int trimMode = BOTH);
		inline String trimSelfLeft();
		inline String trimSelfRight();

		/**
		 * @brief 在字符串前后追加
		 * @param target 需要追加的串
		 * @param merge 含义同上
		 * @return 新串或本身
		 */
		inline String append(const String& target, bool merge = false);// 在后
		inline String prepend(const String& target, bool merge = false);// 在前
		inline String concat(const String& target, bool merge = false);// 同 append(const String& target, bool merge)

		/**
		 * @brief 判断是否是只含空格(空白串)
		 */
		inline bool isBlank();
		inline bool isNotBlank();
		/**
		 * 判断字符串是否是空串(长度为0)
		 */
		inline bool isEmpty();
		inline bool isNotEmpty();

		/**
		 * @brief 以 splitString 为分隔字符串, 分隔this字符串
		 * @return 被分割好的所有子串的集合，不影响本串
		 */
		vector<String> split(const String& splitString);
		vector<String> split(const char symbol);
		// 以symbol为分隔符,将vec中的每一个字符串连接起来
		static String join(vector<String>& vec, const char symbol);
		static String join(String stringArray[], int length, const char symbol);
		static String join(vector<String>& vec, const String& interval);
		
		/**
		 * @brief 大小写转换 caseMode = LOWER | UPPER
		 */
		String toCase(int caseMode, bool = false);
		inline String toLowerCase(bool merge = false);
		inline String toUpperCase(bool merge = false);

		// 获取内部的字符串指针
		inline const char* toChars() const;
		// 获取获取c++的string类型
		inline const string toCppString() const;

		// 以下均为转化为对应数值类型的转化方法
		inline int toInt();
		inline long toLong();
		inline long long toLLong();
		inline float toFloat();
		inline unsigned int toUInt();
		inline unsigned long toULong();
		inline unsigned long long toULLong();
		inline double toDouble();

		/**
		 * @brief
		 * 得到一个 StringBuilder (建造者模式)在应对多个字符串之间的操作时
		 * 这个类的效率比String类高
		 * @param empty 是否将本串预先塞入StringBuider中作为第一个元素被处理
		 */
		inline StringBuilder builder(bool empty = false);
		/*
	     * 获得替换器(建造者模式), 一个辅助进行串替换的工具类
		 */
		inline StringReplacer replacer();

		/**
		 * @brief 简单的打印字符串方法
		 */
		inline const char* print();
		inline const char* println();

		void throwException(RuntimeException e);
		
		bool equalIgnoreCase(String& target);
	
		/**
		 * @brief 以下均为运算符重载方法, 没有必要介绍
		 */
		inline String operator=(const char* charString);
		inline String operator=(const char aChar);
		inline String operator=(const int num);
		inline String operator=(const unsigned num);
		inline String operator=(const long num);
		inline String operator=(const long long num);
		inline String operator=(const unsigned long num);
		inline String operator=(const unsigned long long num);
		inline String operator=(const float num);
		inline String operator=(const double num);

		/**
		 * @deprecated
		 * 以下的重载操作符将被废弃, 因为一个严重的问题: 内存泄漏
		 * 这些方法的实现中,每次拼接都是new出一个新String, 且在
		 * s1+ s2 + s3 中, s1+s2这个运算所生成的对象会丢失引用
		 * 无法在方法外部使用delete主动释放,
		 * 所以: 建议使用StringBuilder类来进行拼接操作
		 * 在该类中, 只有最后一次拼接才会生成新字符串
		 * 且中途只申请一次char*内存, 效率比以下重载方法组合使用高
		 */
		/**
		 * String& operator+(int num);
		 * String& operator+(unsigned num);
		 * String& operator+(long num);
		 * String& operator+(long long num);
		 * String& operator+(unsigned long num);
		 * String& operator+(unsigned long long num);
		 * String& operator+(float num);
		 * String& operator+(double num);
		 * String& operator+(const char* charString);
		 * String& operator+(const String& string);
		 * String& operator+(char aChar);
		 */
		/**
		 * 以下是字符串拼接的改进, 每次都返回一个临时变量
		 * 方法的进退栈都会调用拷贝构造来生成新的临时变量
		 * 进而在效果上达成了引用类型变量的跨栈问题
		 * 且整个过程涉及的变量都是放在栈区, 易于内存管理
		 */
		inline String operator+(int num);
		inline String operator+(unsigned num);
		inline String operator+(long num);
		inline String operator+(long long num);
		inline String operator+(unsigned long num);
		inline String operator+(unsigned long long num);
		inline String operator+(float num);
		inline String operator+(double num);
		inline String operator+(const char* charString);
		inline String operator+(const String& string);
		inline String operator+(char aChar);

		inline String operator+=(const char* charString);
		inline String operator+=(const String& string);

		inline char& operator[](int index);

		inline bool operator==(const char* charString);
		inline bool operator==(const String& target);
	
		inline bool operator!=(const char* charString) {return !operator==(charString);}
		inline bool operator!=(const String& target) {return !operator==(target);}
};



void String::copyAndReplace(const char* source, int length) {
	if (length < 1) {
		return;
	}
//	cout << "copyAndReplace: " << releaseCount << endl;
//	delete data;

	// 如果原字符串的末尾就是结束符
	char* result;
	if (source[length - 1] == 0) {
		result = new char[length];
	} else {
		// 否则, 在末尾添加结束符
		result = new char[length + 1];
		result[length] = 0;
	}
	for (int i = 0; i < length; i++) {
		result[i] = source[i];
	}
	// 实验证明 直接用赋值替换更好, 原shared_ptr拷贝后, 引用计数-1, 若是初始化的shared, 直接变0, 那就可以释放掉
//	data.reset(result);
	data = getIinitialzedSharedPointer(result);
}

void String::copyAndReplaceByCppString(const string& cppString) {
	_length = cppString.length();
	copyAndReplace(cppString.data(), _length);
}

String::String() {}
String::~String() {};//使用共享指针,不需要手动释放char[]
// 拷贝构造, 和默认的拷贝构造相同
String::String(const String& source): _length(source._length), data(source.data) {}

String::String(char* source): _length(strlen(source)) {
	NullPointerException::check(source, "String");
//	data.reset(source);
	data = getIinitialzedSharedPointer(source);
}

String::String(const char* source): _length(strlen(source)) {
	NullPointerException::check(source, "String");
	copyAndReplace(source, _length);
}

String::String(const char* source, int length) {
	NullPointerException::check(source, "String");
	int realLen = strlen(source);
	_length = length > realLen ? realLen : length;
	copyAndReplace(source, _length);
}

String::String(const string& cppString) {
	copyAndReplaceByCppString(cppString);
}

String String::insert(const String& target, int position, bool merge) {
	if (position < 0) {
		position = 0;
	}
	if (position > _length) {
		position = _length;
	}

	int targetLength = target._length;
	int resultLength = targetLength + _length;
	int copyIndex;
	// 多留出一位用来存放 /0 结束符
	char* result = new char[resultLength + 1];
	int cursor = 0;
	for (; cursor < position; cursor++) {
		result[cursor] = data[cursor];
	}
	copyIndex = cursor;
	for (int targetIndex = 0; targetIndex < targetLength;) {
		result[cursor++] = target.data[targetIndex++];
	}
	while (copyIndex < _length) {
		result[cursor++] = data[copyIndex++];
	}
	// 结束字符
	result[resultLength] = 0;
	if (merge) {
//		delete data;
//		data.reset(result);
		data = getIinitialzedSharedPointer(result);
		_length = resultLength;
		return *this;
	} else {
		return String(result);
	}
}

String String::concat(const String& target, bool merge) {
	return insert(target, _length, merge);
};

String String::subString(int startIndex, int endIndex) {
	// 如果不合法,则默认子串开始索引为0
	if (startIndex < 0 || startIndex > _length || startIndex > endIndex) {
		startIndex = 0;
	}

	if (endIndex < startIndex || endIndex > _length) {
		endIndex = _length;
	}
	int resultLength = endIndex - startIndex;
	char* result = new char[resultLength + 1];

	for (int i = 0; i < resultLength; i++) {
		result[i] = data[startIndex++];
	}

	result[resultLength] = 0;

	return String(result);
};

int String::compareTo(String& target) {
	// 会有剩余
	String remainderString;
	int shortStringLength;
	int longStringLength;
	int result = 0;

	if (_length < target._length) {
		shortStringLength = _length;
		longStringLength = target._length;
		remainderString = target;
	} else {
		shortStringLength = target._length;
		longStringLength = _length;
		remainderString = *this;
	}

	int cursor = 0;
	for (; cursor < shortStringLength; cursor++) {
		result += data[cursor] - target.data[cursor];
	}
	// 将剩余的数加入
	for (; cursor < longStringLength; cursor++) {
		result += remainderString.data[cursor];
	}

	return result;
};
/*
const char* String::getCharArray() const {
	return data;
};
const char* String::toChars() const {
	return data;
}
const char* String::c_char() const {
	return data;
}
*/
String String::toCase(int caseMode, bool merge) {
	char* target;
	char* source = data.get();
	if (merge) {
		target = source;
	} else {
		target = new char[_length + 1];
		target[_length] = 0;
	}
	
	if (caseMode == LOWER) {
		for (int i = 0; i < _length; i++) {
			//如果是大写字母
			if(source[i]>=65&&source[i]<=90) {
				target[i] = source[i] | 32;
			} else {
				target[i] = source[i];
			}
		}
	} else {
		for (int i = 0; i < _length; i++) {
			// 如果是小写字母
			if(source[i]>=97&&source[i]<=122){
				target[i] = source[i] & -33;
			} else {
				target[i] = source[i];
			}
		}
	}
	
	if (merge) {
		return *this;
	}
	
	return String(target);
}

String String::toLowerCase(bool merge) {
	return toCase(LOWER,merge);
}
String String::toUpperCase(bool merge) {
	return toCase(UPPER,merge);
}

const char* String::toChars() const {
	return (const char*) data.get();
}

const string String::toCppString() const {
	return string(data.get());
}

const int String::length() const {
	return _length;
};
bool String::isBlank() {
	for (int i = 0; i < _length; i++) {
		if (data[i] != ' ') {
			return false;
		}
	}
	return true;
};
bool String::isNotBlank() {
	return !isBlank();
};
bool String::isEmpty() {
	return _length == 0;
}
bool String::isNotEmpty() {
	return _length != 0;
}

vector<int> String::allIndexOf(const String& search) {
//	vector<int>* result = new vector<int>();
	vector<int> result;
	const int searchLength = search._length;

	if (searchLength == 0) {
		return result;
	}

	const int* prefixTable = getPrefixTable(search);

	// 主串的比较游标
	int compareCursor = 0;
	// 子串的比较游标
	int searchCursor = 0;

	// 若子串还未移动出界
	while (compareCursor + (searchLength - searchCursor) <= _length) {
		// 若不等, 则将比较游标移至前缀表中对应下标的值
		if (search.data[searchCursor] != data[compareCursor]) {
			searchCursor = prefixTable[searchCursor];
			// 若为-1,则说明该位字符在下次比较时可以直接越过与这次compareCursor字符的比较
			if (searchCursor < 0) {
				searchCursor = 0;
				compareCursor++;
			}
			continue;
		}
		// 该处字符相等,让 主串比较游标 右移
		compareCursor++;
		searchCursor++;
		// 全部配对成功，意味着模式串游标searchCursor已刚好超出模式串的末尾下标，即以下条件
		if (searchCursor == searchLength) {
			int matchedIndex = compareCursor - searchLength;
			result.push_back(matchedIndex);
			compareCursor = matchedIndex + searchLength;
			searchCursor = 0; // 重新置0, 从模式串的0下标处重新开始比较
		}
	}
	delete prefixTable;
//	cout << "matched Index: [";
//	for (int i = 0; i < result.size(); i++) {
//		cout << result[i] << " ";
//	}
//	cout << "]";
	
	return result;
}

int String::indexOf(const char search) {
	for (int i = 0; i < _length; i++) {
		if (data[i] == search) {
			return i;
		}
	}
	return -1;
}

int String::indexOfthe(const String& search, const int index) {
	int matchedSubStringCounter = 0;
	const int searchLength = search._length;
	if (searchLength == 0) {
		return 0;
	}

	const int* prefixTable = getPrefixTable(search);

	// 主串的比较游标
	int compareCursor = 0;
	// 子串的比较游标
	int searchCursor = 0;

	// 若子串还未移动出界
	while (compareCursor + (searchLength - searchCursor) <= _length) {
		// 若不等, 则将比较游标移至前缀表中对应下标的值
		if (search.data[searchCursor] != data[compareCursor]) {
			searchCursor = prefixTable[searchCursor];
			// 若为-1,则说明该位字符在下次比较时可以直接越过与这次compareCursor字符的比较
			if (searchCursor < 0) {
				searchCursor = 0;
				compareCursor++;
			}
			continue;
		}
		// 该处字符相等,让 主串比较游标 右移
		compareCursor++;
		searchCursor++;
		// 全部配对成功，意味着模式串游标searchCursor已刚好超出模式串的末尾下标，即以下条件
		if (searchCursor == searchLength) {
			int matchedIndex = compareCursor - searchLength;
			if (matchedSubStringCounter++ == index) {
				delete prefixTable;
				return matchedIndex;
			}
			compareCursor = matchedIndex + searchLength;
			searchCursor = 0; // 重新置0, 从模式串的0下标处重新开始比较
		}
	}
	delete prefixTable;
	//	cout << "matched Index: [";
	//	for (int i = 0; i < result.size(); i++) {
	//		cout << result[i] << " ";
	//	}
	//	cout << "]";

	return -1;
}

int String::indexOf(const String& search) {
	const int searchLength = search._length;

	if (searchLength == 0) {
		return 0;
	} else if (searchLength == 1) {
		return indexOf(search.data.get()[0]);
	}

	return indexOfthe(search, 0);
}

int String::lastIndexOf(const char search) {
	for (int i = _length; i > 0; i--) {
		if (data[i] == search) {
			return i;
		}
	}
	return -1;
}

int String::lastIndexOf(const String& search) {
	int searchLength = search._length;

	if (searchLength == 0) {
		return _length - 1;
	} else if (searchLength == 1) {
		return lastIndexOf(search.data[0]);
	}

	int*  suffixTable = getSuffixTable(search);

	const int serchResetCursor = searchLength - 1;

	int compareCursor = _length - 1;
	int searchCursor = serchResetCursor;
	while (_length - compareCursor + searchCursor <= _length) {
		// 若不等, 则将比较游标移至前缀表中对应下标的值
		if (search.data[searchCursor] != data[compareCursor]) {
			searchCursor = suffixTable[searchCursor];
			// 若为-1,则说明该位字符在下次比较时可以直接越过与这次compareCursor字符的比较
			if (searchCursor < 0) {
				searchCursor = serchResetCursor;
				compareCursor--;
			}
			continue;
		}
		// 模式串游标searchCursor已移至模式串首元素，进行到这说明模式串已全匹配成功，可以返回了
		if (searchCursor == 0) {
			delete suffixTable;
			return compareCursor;
		}

		searchCursor--;
		// 该处字符相等,让主串比较游标右移
		compareCursor--;
	}
	delete suffixTable;
	return -1;
}

int* const String::getPrefixTable(const String& search) {
	/**
	 * 要明确一点，这个前缀表整体右移一位，故第一位对应的前缀值被移至
	 * 第二位， 空出来的那一位(也就是第一位)的前缀值就赋-1
	 * // 称 某子串 的最大前后缀的公共(相等)子串长度 为前缀值
	 */
	const char* compChar = search.toChars();
	const int tableLength = search.length();
	if (tableLength == 0) {
		return new int[0];
	}

	int* table = new int[tableLength];
	// 右移后，第一个前缀值为-1, 空串(长度为0)的前缀值
	table[0] = -1;

	if (tableLength < 2) {
		return table;
	}
	// 优化出nextVal数组
	if (compChar[1] == compChar[0]) {
		table[1] = table[0];
	} else {
		// 只含有一个字母的子串(长度为1)的前缀值肯定为0
		table[1] = 0;
	}

	//prefixLength 生成的比较子串的长度, 以下分别计算出从长度为2的子串开始的各个串的前缀值
	for (int prefixLength = 2; prefixLength < tableLength; prefixLength++) {
		// 最大前后缀公共(相同)子串的最大长度
		int maxEqualLength = 0;
		// 比较,生成公共前缀
		for (int compLength = 1; compLength < prefixLength; compLength++) {
			// 将每次比较的右指针指向后缀子串的开头
			int rightCursor = prefixLength - compLength;
			int leftCursor = 0;
			// cout<<"rightCursor reset: "<<rightCursor<<endl;
			bool isAllEqual = true;
			// leftCursor从0开始,适合与比较子串的长度相比较
			while (leftCursor < compLength) {
				if (compChar[leftCursor++] != compChar[rightCursor++]) {
					isAllEqual = false;
					break;
				}
			}
			// 如果全比较一遍发现两比较子串完全相同
			if (isAllEqual) {
				maxEqualLength = compLength;
			}
		}
		if (compChar[prefixLength] == compChar[maxEqualLength]) {
			// 可以优化成nextVal数组
			table[prefixLength] = table[maxEqualLength];
		} else {
			// 原始的next数组
			table[prefixLength] = maxEqualLength;
		}
	}

//	cout << "prefixTable: ";
//	for (int i = 0; i < tableLength; i++) {
//		cout << table[i] << " ";
//	}
//	cout << endl;
	return table;
}

int* const String::getSuffixTable(const String& search) {
	const char* compChar = search.toChars();
	int tableLength = search.length();
	if (tableLength == 0) {
		return new int[0];
	}

	int* table = new int[tableLength];

	const int rightBound = tableLength - 1;

	// 默认第一个前缀值为-1, 空串(长度为0)的前缀值
	table[rightBound] = -1;

	if (tableLength < 2) {
		return table;
	}

	// 倒数第二个
	int last2 = rightBound - 1;
	if (compChar[last2] == compChar[rightBound]) {
		// 优化出nextVal数组
		table[last2] = table[rightBound];
	} else {
		// 只含有一个字母的子串(长度为1)的后缀值必为最大索引
		table[last2] = rightBound;
	}

	//prefixLength 生成的比较子串的长度, 以下分别计算出从长度为2的子串开始的各个串的前缀值
	for (int suffixLength = 2; suffixLength < tableLength; suffixLength++) {
		// 最大前后缀公共(相同)子串的最大长度 对应的 子串索引值
		int maxEqualLength = rightBound;
		// 比较,生成公共前缀
		const int leftBound = tableLength - suffixLength;
		for (int compLength = 1; compLength < suffixLength; compLength++) {
			// 将每次比较的右指针指向后缀子串的开头
			int rightCursor = rightBound;
			int leftCursor = leftBound + compLength - 1;
			// cout<<"rightCursor reset: "<<rightCursor<<endl;
			bool isAllEqual = true;
			while (rightCursor > (rightBound - compLength)) {
				if (compChar[leftCursor--] != compChar[rightCursor--]) {
					isAllEqual = false;
					break;
				}
			}
			// 如果全比较一遍发现两比较子串完全相同
			if (isAllEqual) {
				// 这个在函数外部就会充当模式串的找下一比对字符的下标
				maxEqualLength = rightBound - compLength;
			}
		}
		const int suffixTableGenerateCursor = rightBound - suffixLength;
		if (compChar[suffixTableGenerateCursor] == compChar[maxEqualLength]) {
			table[suffixTableGenerateCursor] = table[maxEqualLength];
		} else {
			table[suffixTableGenerateCursor] = maxEqualLength;
		}
//		cout << "table index: " << rightBound - suffixLength << endl;
	}

//	cout << "suffixTable: ";
//	for (int i = 0; i < tableLength; i++) {
//		cout << table[i] << " ";
//	}
//	cout << endl;
	return table;
}

void String::checkReplaceIndex(int index, function<bool()>prediction, const char* msg) {
	if (prediction()) {
		throw IllegalArgumentsException((String("param 'direction' is ") + msg
		                                 + " is cash with param 'replaceIndex' value: " + index ).toCppString());
	}
}

String String::replace(const String& search, const String& with, bool merge, int direction, int replaceIndex) {
	int startIndex;
	// 根据方向来选择使用哪种匹配方法
	if (direction == LEFT) {
		checkReplaceIndex(replaceIndex, [replaceIndex]()-> bool {return (replaceIndex < 0) || replaceIndex == INT_MAX;}, "'String::LEFT'");
		startIndex = indexOfthe(search, replaceIndex);
	} else if (direction == RIGHT) {
		checkReplaceIndex(
		    replaceIndex,
		    [replaceIndex]()-> bool {return (replaceIndex != INT_MAX);},
		    "'String::RIGHT'");
		startIndex = lastIndexOf(search);
	} else {
		checkReplaceIndex(replaceIndex, [replaceIndex]()-> bool {return (replaceIndex > 0);}, "'String::BOTH'");
		return replaceAll(search, with, merge);
	}

	if (startIndex == -1) {
		return *this;
	}

	int resultLength = _length - search._length + with._length;
	char* resultChar = new char[resultLength + 1];

	int cursor = 0;
	while (cursor < startIndex) {
		resultChar[cursor] = data[cursor];
		cursor++;
	}

	int withCursor = 0;
	while (withCursor < with._length) {
		resultChar[cursor++] = with.data[withCursor++];
	}

	int dataCursor = startIndex + search._length;
	while (cursor < resultLength) {
		resultChar[cursor++] = data[dataCursor++];
	}

	resultChar[resultLength] = 0;

	if (merge) {
//		delete data;
//		data.reset(resultChar);
		data = getIinitialzedSharedPointer(resultChar);
		_length = resultLength;
		return *this;
	}

	return String(resultChar);
}

String String::replaceByIndex(const String& search, const String& with, bool merge, int replaceIndex) {
	return replace(search, with, merge, LEFT, replaceIndex);
}

String String::deleteAll(const String& search, bool merge) {
	return replace(search, "", merge);
}

String String::replaceFirst(const String& search, const String& with, bool merge) {
	return replace(search, with, merge, LEFT);
}
String String::replaceLast(const String& search, const String& with, bool merge) {
	return replace(search, with, merge, RIGHT);
}
String String::replaceAll(const String& search, const String& with, bool merge) {
	vector<int> allStartIndexes = allIndexOf(search);
	int size = allStartIndexes.size();
	int resultLength = _length - (search._length * size) + (with._length * size);
//	cout<<"replaceAll resultLength: "<<resultLength<<endl;
	char* resultChar = new char[resultLength + 1];

	int cursor = 0;
	int dataCursor = 0;
	char* thisChars = data.get();
	for (int i = 0; i < size; i++) {
		int startIndex = allStartIndexes[i];
		int replaceSelfStringCursor = 0;
		char* replaceSelfChars = with.data.get();
		while (dataCursor < startIndex) {
			resultChar[cursor++] = thisChars[dataCursor++];
		}

		while (replaceSelfStringCursor < with._length) {
			resultChar[cursor++] = replaceSelfChars[replaceSelfStringCursor++];
		}

		dataCursor += search._length;
	}
//	cout<<"before collect remained"<<endl;
	while (cursor < resultLength) {
//		cout<<"collect remained: "<<(int)thisChars[dataCursor]<<endl;
		resultChar[cursor++] = thisChars[dataCursor++];
	}

	resultChar[resultLength] = 0;

//	cout<< "finished replaceAll: "<<resultChar<<endl;
	if (merge) {
//		delete data;
//		data.reset(resultChar);
		data = getIinitialzedSharedPointer(resultChar);
		_length = resultLength;
		return *this;
	}

	return String(resultChar);
}

String String::replaceSelf(const String& search, const String& with, int direction) {
	return replace(search, with, true, direction);
}
String String::replaceSelfFirst(const String& search, const String& with ) {
	return replaceFirst(search, with, true);
}
String String::replaceSelfLast(const String& search, const String& with ) {
	return replaceLast(search, with, true);
}
String String::replaceSelfAll(const String& search, const String& with ) {
	return replaceAll(search, with, true);
}

String String::replaceSelfByIndex(const String& search, const String& with, int replaceIndex) {
	return replaceByIndex(search, with, true, replaceIndex);
}

bool String::contain(const char search) {
	return indexOf(search) != -1;
}
bool String::contain(const String& search) {
	return indexOf(search) != -1;
}

String String::reverse(bool merge) {
	int left = 0, right = _length - 1;

	if (!merge) {
		char* chars = new char[_length + 1];

		const char* thisChars = data.get();

		chars[_length] = 0;

		while (left < _length) {
			chars[left++] = thisChars[right--];
		}

		return String(chars);
	}

	while (left < right) {
		char temp = data[left];
		data[left] = data[right];
		data[right] = temp;
		++left;
		--right;
	}

	// cout << data << endl;
	return *this;
}

bool String::startsWith(const String& prefix) {
	const char* search = prefix.data.get();
	const int searchLength = prefix._length;
	for (int i = 0; i < searchLength; i++) {
		if (data[i] != search[i]) {
			return false;
		}
	}
	return true;
}
bool String::endsWith(const String& suffix) {
	const char* search = suffix.data.get();
	const int searchLength = suffix._length;
	int cursor = _length - 1;
	for (int i = searchLength - 1; i > 0; i--) {
		if (data[cursor--] != search[i]) {
			return false;
		}
	}
	return true;
}

bool String::hasPrefix(const String& prefix) {
	return startsWith(prefix);
}
bool String::hasSuffix(const String& suffix) {
	return endsWith(suffix);
}

int String::toInt() {
	return atoi(data.get());
}
long String::toLong() {
	return atol(data.get());
}
long long String::toLLong() {
	return atoll(data.get());
}
float String::toFloat() {
	return atof(data.get());
}
unsigned int String::toUInt() {
	return atoi(data.get());
}
unsigned long String::toULong() {
	return atol(data.get());
}
unsigned long long String::toULLong() {
	return atoll(data.get());
}
double String::toDouble() {
	return atof(data.get());
}

vector<String> String::split(const String& splitString) {
	return vector<String>();
};

vector<String> String::split(const char symbol) {
	int startIndex = 0;
	int endIndex = 0;
//	vector<String>* result = new vector<String>();
	vector<String> result;
	for (; endIndex < _length; endIndex++) {
		if (data[endIndex] == symbol) {
			result.push_back(subString(startIndex, endIndex));
			startIndex = endIndex + 1;
		}
	}
	// 将剩余的一个字符串装入
	result.push_back(subString(startIndex, endIndex));
		
	return result;
}

String String::join(vector<String>& vec, const char symbol) {
	int size = vec.size();

	if (size == 0) {
		return String();
	}

	int resultLength = 0;

	for (int i = 0; i < size; i++) {
		resultLength += vec[i]._length + 1; // 最后退出循环会多一个位置,但是该位置不用清除, 可以用来放结束符
	}

	char* result = new char[resultLength];

	int cursor = 0;

	for (int i = 0; i < size; i++) {
		for (int charIndex = 0; charIndex < vec[i]._length; charIndex++) {
			result[cursor++] = vec[i].data[charIndex];
		}
		result[cursor++] = symbol;
	}

	result[resultLength - 1] = 0;

	return String(result);
}
String String::join(String stringArray[], int length, const char symbol) {
	int resultLength = 0;

	if (length == 0) {
		return String();
	}

	for (int i = 0; i < length; i++) {
		resultLength += stringArray[i]._length + 1; // 最后退出循环会多一个位置,但是该位置不用清除, 可以用来放结束符
	}

	char* result = new char[resultLength];

	int cursor = 0;

	for (int i = 0; i < length; i++) {
		for (int charIndex = 0; charIndex < stringArray[i]._length; charIndex++) {
			result[cursor++] = stringArray[i].data[charIndex];
		}
		result[cursor++] = symbol;
	}

	result[resultLength - 1] = 0;

	return String(result);
}
String String::join(vector<String>& vec, const String& interval = "") {
	int size = vec.size();

	if (size == 0) {
		return String();
	}

	int resultLength = vec[0]._length;

	for (int i = 1; i < size; i++) {
		resultLength += interval._length + vec[i]._length; // 最后退出循环会多一个位置,但是该位置不用清除, 可以用来放结束符
	}
	// 预留出结束符
	char* resultChar = new char[resultLength + 1];

	int cursor = 0;

	String el = vec[0];
	for (int i = 0; i < el._length; i++) {
		resultChar[cursor++] = el.data[i];
	}

	for (int i = 1; i < size; i++) {
		String el = vec[i];

		for (int i = 0; i < interval._length; i++) {
			resultChar[cursor++] = interval.data[i];
		}

		for (int i = 0; i < el._length; i++) {
			resultChar[cursor++] = el.data[i];
		}
	}

	resultChar[resultLength] = 0;

	return String(resultChar);
}

String String::slice(int startIndex, int charCount, bool merge) {
	if (startIndex < 0) {
		startIndex = 0;
	}
	if (startIndex > _length) {
		startIndex = _length;
	}
	if (charCount < 0) {
		charCount = _length - startIndex;
	}

	if (merge) {
		String temp = subString(startIndex, startIndex + charCount);
		copyAndReplace(temp.data.get(), temp._length);
		data = temp.data;
		_length = temp._length;
		return *this;
	}

	return subString(startIndex, startIndex + charCount);
}
String String::slice(int startIndex, bool merge) {
	return slice(startIndex, -1, merge);
}
String String::splice(int startIndex, int charCount) {
	return slice(startIndex, charCount, true);
}

inline int String::ensureNotBlankEndIndex(int endIndex) {
	if (data[endIndex] == ' ') {
		while (endIndex > 0) {
			if (data[endIndex] != ' ') {
				break;
			}
			endIndex--;
		}
	}
	return endIndex;
}

inline int String::ensureNotBlankStartIndex(int startIndex) {
	if (data[startIndex] == ' ') {
		while (startIndex < _length) {
			if (data[startIndex] != ' ') {
				break;
			}
			startIndex++;
		}
	}
	return startIndex;
}

String String::trim(int trimMode, bool merge) {
	if (isEmpty()) {
		return *this;
	}

	int startIndex = 0;
	int endIndex = _length - 1;

	switch (trimMode) {
		case BOTH:
			endIndex = ensureNotBlankEndIndex(endIndex);
		case LEFT:
			startIndex = ensureNotBlankStartIndex(startIndex);
			break;
		case RIGHT:
			endIndex = ensureNotBlankEndIndex(endIndex);
			break;
	}

	if (merge) {
		String temp = subString(startIndex, endIndex + 1);
//		copyAndReplace(temp.data, temp._length);
//		_length = temp._length;
		data = temp.data;
		_length = temp._length;
		return *this;
	}

	return subString(startIndex, endIndex + 1);
};

String String::trimLeft(bool merge) {
	return trim(LEFT, merge);
}
String String::trimRight(bool merge) {
	return trim(RIGHT, merge);
}

String String::trimSelf(int trimMode) {
	return trim(trimMode, true);
}

String String::trimSelfLeft() {
	return trim(LEFT, true);
}
String String::trimSelfRight() {
	return trim(RIGHT, true);
}

String String::append(const String& target, bool merge) {
	return insert(target, _length, merge);
};

String String::prepend(const String& target, bool merge) {
	return insert(target, 0, merge);
};

const char* String::print() {
	cout << data.get();
	return data.get();
}
const char* String::println() {
	cout << data.get() << endl;
	return data.get();
}

StringBuilder String::builder(bool empty) {
	StringBuilder temp;
	if (!empty) {
		return temp.append(*this);
	}

//	cout << "before return builder" << endl;
	return temp;// 在方法退栈,本地变量释放前,调用拷贝构造,生成新的临时变量返回到返回地址
}

StringReplacer String::replacer() {
	return StringReplacer(this);
}

void String::throwException(RuntimeException e) {
	e.setMessage(data.get());
	throw e;
}

bool String::equalIgnoreCase(String& target){
	return toLowerCase() == target.toLowerCase();
}

String String::operator=(const char* charString) {
	_length = strlen(charString);
	copyAndReplace(charString, _length);
	return *this;
};

String String::operator=(const char aChar) {
//	delete data;
	this->_length = 1;
	char* result = new char[2];
	result[0] = aChar;
	result[1] = 0;
//	data.reset(result);
	data = getIinitialzedSharedPointer(result);
	return *this;
}

String String::operator=(const int num) {
//	delete data;
	string temp = to_string(num);
	copyAndReplace(temp.c_str(), temp.length());
	_length = temp.length();
	return *this;
}
String String::operator=(const unsigned num) {
	string temp = to_string(num);
	copyAndReplace(temp.c_str(), temp.length());
	_length = temp.length();
	return *this;
}
String String::operator=(const long num) {
	string temp = to_string(num);
	copyAndReplace(temp.c_str(), temp.length());
	_length = temp.length();
	return *this;
}
String String::operator=(const long long num) {
	string temp = to_string(num);
	copyAndReplace(temp.c_str(), temp.length());
	_length = temp.length();
	return *this;
}
String String::operator=(const unsigned long num) {
	string temp = to_string(num);
	copyAndReplace(temp.c_str(), temp.length());
	_length = temp.length();
	return *this;
}
String String::operator=(const unsigned long long num) {
	string temp = to_string(num);
	copyAndReplace(temp.c_str(), temp.length());
	_length = temp.length();
	return *this;
}
String String::operator=(const float num) {
	string temp = to_string(num);
	copyAndReplace(temp.c_str(), temp.length());
	_length = temp.length();
	return *this;
}
String String::operator=(const double num) {
	string temp = to_string(num);
	copyAndReplace(temp.c_str(), temp.length());
	_length = temp.length();
	return *this;
}

String::String(const int num) {
	copyAndReplaceByCppString(to_string(num));
}
String::String(const unsigned num) {
	copyAndReplaceByCppString(to_string(num));
}
String::String(const long num) {
	copyAndReplaceByCppString(to_string(num));
}
String::String(const long long num) {
	copyAndReplaceByCppString(to_string(num));
}
String::String(const unsigned long num) {
	copyAndReplaceByCppString(to_string(num));
}
String::String(const unsigned long long num) {
	copyAndReplaceByCppString(to_string(num));
}
String::String(const float num) {
	copyAndReplaceByCppString(to_string(num));
}
String::String(const double num) {
	copyAndReplaceByCppString(to_string(num));
}

String::String(vector<String> strs) {
	String temp = join(strs);
	data = temp.data;
	_length = temp._length;
}

String String::operator+(const char* string) {
	return insert(string, _length);
}
String String::operator+(const String& string) {
	return insert(string, _length);
}
String String::operator+(char aChar) {
	return insert(to_string(aChar), _length);
}
String String::operator+(int num) {
	return insert(to_string(num), _length);
}
String String::operator+(unsigned num) {
	return insert(to_string(num), _length);
}
String String::operator+(long num) {
	return insert(to_string(num), _length);;
}
String String::operator+(long long num) {
	return insert(to_string(num), _length);;
}
String String::operator+(unsigned long num) {
	return insert(to_string(num), _length);;
}
String String::operator+(unsigned long long num) {
	return insert(to_string(num), _length);;
}
String String::operator+(float num) {
	return insert(to_string(num), _length);;
}
String String::operator+(double num) {
	return insert(to_string(num), _length);;
}


String String::operator+=(const char* charString) {
	return insert(charString, _length, true);
};
String String::operator+=(const String& string) {
	return insert(string.data.get(), _length, true);
};

char& String::operator[](int index) {
	if (index < 0 || index >= _length) {
		throw OutOfBoundException(index, 0, _length - 1);
	}
	return data[index];
}

bool String::operator==(const char* charString) {
	return strcmp(data.get(), charString) == 0;
}
bool String::operator==(const String& target) {
	return strcmp(data.get(), target.data.get()) == 0;
}


String StringBuilder::build(bool clear) {
//	cout<<"start building"<<endl;
	int size = menbers.size();
	if (size == 0) {
		return String();
	}
	int resultLength = menbers[0].length();

	for (int i = 1; i < size; i++) {
		resultLength +=  menbers[i].length();
	}
	// 预留出结束符
	char* resultChar = new char[resultLength + 1];

	int cursor = 0;
	for (int i = 0; i < size; i++) {
		String el = menbers[i];

		int length = el.length();
		for (int i = 0; i < length; i++) {
			resultChar[cursor++] = el[i];
		}
	}

	resultChar[resultLength] = 0;
	if (clear) {
		menbers.clear();
		menbers.shrink_to_fit();
	}
//	cout << "builded chars: " << resultChar << "____" << endl;
	return String(resultChar);
}

StringBuilder& StringBuilder::append(const String& target) {
	menbers.push_back(target);
	return *this;
}

StringBuilder& StringBuilder::insert(const String& target, const int index) {
	menbers.insert(menbers.begin() + index, target);
	return *this;
}

StringBuilder& StringBuilder::erase(const String& target) {
	for (vector<String>::iterator it = menbers.begin(); it != menbers.end(); it++) {
		if (*(it) != target) {
			continue;
		}
		menbers.erase(it, it + 1);
		break;
	}

	return *this;
}

StringBuilder& StringBuilder::erase(const int index) {
	if (index < 0 || index >= size()) {
		throw OutOfBoundException(index, 0, size() - 1);
	}
	auto begin = menbers.begin() + index;
	menbers.erase(begin, begin + 1);
	return *this;
}

StringBuilder& StringBuilder::operator+(const String& target) {
	menbers.push_back(target);
	return *this;
}

StringBuilder& StringBuilder::operator-(const String& target) {
	return erase(target);
}

StringBuilder& StringBuilder::operator-(const int index) {
	return erase(index);
}

inline StringBuilder& StringBuilder::operator*(const int repeatCount) {
	if (repeatCount < 0) {
		StringBuilder msg;
		msg + "repeatCount" + repeatCount + " should be >= 0";
		throw IllegalArgumentsException(msg.build().toCppString());
	}
	int size_ = size();
	menbers.reserve(repeatCount * size_);
	for (int i = 1; i < repeatCount; i++) {
		for (int i = 0; i < size_; i++) {
			menbers.push_back(menbers[i]);
		}
	}
	return *this;
}

inline StringBuilder& StringBuilder::operator/(int eraseProportion) {
	if (eraseProportion < 0) {
		StringBuilder msg;
		msg + "eraseProportion" + eraseProportion + " should be >= 0";
		throw IllegalArgumentsException(msg.build().toCppString());
	}
	if (eraseProportion == 0) {
		menbers.clear();
	} else if (eraseProportion == 1) {
		return *this;
	} else {
		menbers.erase(menbers.begin() + size() / eraseProportion, menbers.end());
	}

	menbers.shrink_to_fit();
	return *this;
}

String StringReplacer::replace() {
	return target->replace(_search, _with, _merge, _direction, _replaceIndex);
}

StringReplacer::StringReplacer(String* target) : StringHelper(target), _direction(String::LEFT) {}
