#pragma once
#include <cstring>
#include <iostream>
#include <vector>
#include "Exception.h"
#define COPY_STRING_BUFFER_SIZE 64
#define COPY_INT_BUFFER_SIZE 12
class String;

int* const getPrefixTable(const String& search);

int* const getSuffixTable(const String& search);

/**
 * 这个类可以帮助拼接字符串, 防止因直接使用
 * String + String 拼接而造成的内存泄漏问题
 */
class StringBuilder {
		vector<String> merbers;
	public:
		~StringBuilder();

		StringBuilder& append(const String& target);

		String build();

		StringBuilder& operator+(const String& target);
	
		String& operator[](int index){
			return merbers[index];
		}
	
		const size() const {
			return merbers.size();
		}
};

class String {
		/**
		 * 字符串的长度
		 */
		int _length = 0;
		/**
		 * 存放字符串的char数组，初始化data为只有一个结束符的空串
		 */
		char* data = new char[1] {0};

		/**
		 * 每个String实例都可以有唯一一个对应的builder,辅助拼接
		 */
		StringBuilder* _builder = nullptr;
		/**
		 * @brief 将常量池中的const char*字符串复制进堆中的工具方法
		 * @param length 产生的复制字符串长度
		 * @param source 被复制的字符串范本
		 * @return 复制好的char*数组
		 */
		char* copyAndReplace(const char* source, int length);
		/**
		 * @brief 对c++内部string类的一个转换
		 */
		void copyAndReplaceByCppString(const string& cppString);


	public:
		/**
		 * @brief 定义几个与方向相关的静态常量,语义化
		 * 		  在该类里，所有与方向相关的方法都可以用这几个常量
		 */
		const static int LEFT = 1;
		const static int RIGHT = 2;
		const static int BOTH = 3;

		String();
		~String();

		String(const String& source);
		String(const char* source);
		String(const char* source, int length);
		String(const string& cppString);

		// 包装了转化方法的构造器
		String(const int num);
		String(const unsigned num);
		String(const long num);
		String(const long long num);
		String(const unsigned long num);
		String(const unsigned long long num);
		String(const float num);
		String(const double num);

		/**
		 * @brief 将目标串插入到本串的指定索引的位置上
		 * @param target 将被插入的目标串
		 * @param position 将要插入该串的索引值
		 * @param merge 返回结果是否影响本串，若不影响，则本串的字符串不会被改变
		 * 				只是创建一个新串来保存返回结果
		 * @return 创建出的新串，或者是该串本身
		 */
		String& insert(const String& target, int position, bool merge = false);
		// 返回startIndex到endIndex的子串,endIndex默认-1,意为截取到末尾
		String& subString(int startIndex, int endIndex  = -1);

		// 返回第一次出现该字符的索引, 若未出现则返回-1
		int indexOf(const char search);
		// 返回第一次出现该字串的索引, 若未出现则返回-1
		// 使用 KMP 算法, 默认返回找到的子串从左开始第一次出现的索引
		int indexOf(const String& sub);
		int lastIndexOf(const char search);
		int lastIndexOf(const String& sub);
		// 返回所有相同子串的索引
		vector<int> allIndexOf(const String& search);


		// 是否包含该字符/子串
		bool contain(const char search);
		bool contain(const String& sub);


		/**
		 * @brief 是否以前缀prefix/后缀suffix开头/结尾
		 */
		bool startWith(const String& prefix);
		bool endWith(const String& suffix);

		/**
		 * @brief 作用同startWith(const String& prefix)与endWith(const String& suffix)
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
		const int length() const;
		/**
		 * @brief 切分方法
		 * @param startIndex 从startIndex开始切分
		 * @param charCount 往后切分charCount个字符
		 * @param merge 可以选择是否影响本串(是否将切分结果保存入本串)
		 */
		String& slice(int startIndex, int charCount = -1, bool merge = false);
		String& slice(int startIndex, bool merge = false); // 默认切分到末尾
		String& splice(int startIndex, int charCount = -1); // 默认切分到末尾, 默认影响本串

		/**
		 * @brief 用with串替换第1/最后一次出现的search子串,若不含有该子串，则不替换
		 * @param search 要被替换的串
		 * @param with 替换成的串
		 * @param merge 该方法的执行是否影响原串
		 * @param direction 替换的方向,可以使用String的枚举值来指定从哪个方向开始替换
		 * @return 完成替换后的串
		 */
		String& replace(const String& search, const String& with = "", bool merge = false, int direction = LEFT);
		String& replaceFirst(const String& search, const String& with = "", bool merge = false);
		String& replaceLast(const String& search, const String& with = "", bool merge = false);
		String& replaceAll(const String& search, const String& with = "", bool merge = false);

		// 以下replaceWith方法均会影响原串
		String& replaceWith(const String& search, const String& with = "", int direction = LEFT);
		String& replaceFirstWith(const String& search, const String& with = "");
		String& replaceLastWith(const String& search, const String& with = "");
		String& replaceAllWith(const String& search, const String& with = "");

		/**
		 * @brief 去除空格
		 * @param trimMode 传入枚举常量 String::LEFT, RIGHT, BOTH 来标识需要删除哪边的空格
		 * @param merge 是否影响本串，否就创新串，是则修改自身并返回自身
		 * @return
		 */
		String& trim(int trimMode = BOTH, bool merge = false);
		String& trimLeft(bool merge = false);
		String& trimRight(bool merge = false);
		// 以下方法均会影响自身
		String& trimSelf(int trimMode = BOTH);
		String& trimSelfLeft();
		String& trimSelfRight();

		/**
		 * @brief 在字符串前后追加
		 * @param target 需要追加的串
		 * @param merge 含义同上
		 * @return 新串或本身
		 */
		String& append(const String& target, bool merge = false);// 在后
		String& prepend(const String& target, bool merge = false);// 在前
		String& concat(const String& target, bool merge = false);// 同 append(const String& target, bool merge)

		/**
		 * @brief 判断是否是只含空格(空白串)
		 */
		bool isBlank();
		bool isNotBlank();
		/**
		 * 判断字符串是否是空串(长度为0)
		 */
		bool isEmpty();
		bool isNotEmpty();

		/**
		 * @brief 以 splitString 为分隔字符串, 分隔this字符串
		 * @return 被分割好的所有子串的集合，不影响本串
		 */
		vector<String>& split(const String& splitString);
		vector<String>& split(const char symbol);
		// 以symbol为分隔符,将vec中的每一个字符串连接起来
		static String& join(vector<String>& vec, const char symbol);
		static String& join(String stringArray[], int length, const char symbol);
		static String& join(vector<String>& vec, const String& interval);

		// 获取内部的字符串指针
		const char* toChars() const;
		// 获取获取c++的string类型
		const string toCppString() const;

		// 以下均为转化为对应数值类型的转化方法
		int toInt();
		long toLong();
		long long toLLong();
		float toFloat();
		unsigned int toUInt();
		unsigned long toULong();
		unsigned long long toULLong();
		double toDouble();

		StringBuilder& builder() {
			if (_builder == nullptr) {
				_builder = new StringBuilder();
			}
			if(_builder->size()==0){
				_builder->append(*this);
			}
			return *(_builder);
		}
	
		void clearBuilder(){
			if(_builder!= nullptr) {
				delete _builder;
				_builder = nullptr;
			}
		}

		/**
		 * @brief 简单的打印字符串方法
		 */
		void print();
		void println();

		/**
		 * @brief 以下均为运算符重载方法, 没有必要介绍
		 */
		String& operator=(const char* charString);
		String& operator=(const char aChar);
		String& operator=(const int num);
		String& operator=(const unsigned num);
		String& operator=(const long num);
		String& operator=(const long long num);
		String& operator=(const unsigned long num);
		String& operator=(const unsigned long long num);
		String& operator=(const float num);
		String& operator=(const double num);

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

		String& operator+=(const char* charString);
		String& operator+=(const String& string);

		char& operator[](int index);

		bool operator==(const char* charString);
		bool operator==(const String& target);

};



char* String::copyAndReplace(const char* source, int length) {
	if (length < 1) {
		return data;
	}

	delete data;
	// 如果原字符串的末尾就是结束符
	if (source[length - 1] == 0) {
		data = new char[length];
	} else {
		// 否则, 在末尾添加结束符
		data = new char[length + 1];
		data[length] = 0;
	}
	for (int i = 0; i < length; i++) {
		data[i] = source[i];
	}
	return data;
}

void String::copyAndReplaceByCppString(const string& cppString) {
	_length = cppString.length();
	copyAndReplace(cppString.data(), _length);
}

String::String() {}
String::~String() {
	if (_builder != nullptr) {
		delete _builder;
	}
	delete data;
};

String::String(const String& source) {
	_length = source._length;
	copyAndReplace(source.data, _length);
}

String::String(const char* source) {
	_length = strlen(source);
	copyAndReplace(source, _length);
}

String::String(const char* source, int length) {
	int realLen = strlen(source);
	_length = length > realLen ? realLen : length;
	copyAndReplace(source, _length);
}

String::String(const string& cppString) {
	copyAndReplaceByCppString(cppString);
}

String& String::insert(const String& target, int position, bool merge) {
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
		delete data;
		data = result;
		_length = resultLength;
		return *this;
	} else {
		return *(new String(result, resultLength));
	}
}

String& String::concat(const String& target, bool merge) {
	return insert(target, _length, merge);
};

String& String::subString(int startIndex, int endIndex) {
	// 如果不合法,则默认子串开始索引为0
	if (startIndex < 0 || startIndex > _length) {
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

	return *(new String(result, resultLength));
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
const char* String::toChars() const {
	return data;
}

const string String::toCppString() const {
	return string(data);
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
	vector<int>* result = new vector<int>();
	const int searchLength = search._length;

	if (searchLength == 0) {
		return *result;
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
			result->push_back(matchedIndex);
			compareCursor = matchedIndex + searchLength;
			searchCursor = 0; // 重新置0, 从模式串的0下标处重新开始比较
		}
	}
//	cout << "matched Index: [";
//	for (int i = 0; i < result->size(); i++) {
//		cout << result->at(i) << " ";
//	}
//	cout << "]";

	return *result;
}

int String::indexOf(const char search) {
	for (int i = 0; i < _length; i++) {
		if (data[i] == search) {
			return i;
		}
	}
	return -1;
}

int String::indexOf(const String& search) {
	int searchLength = search._length;

	if (searchLength == 0) {
		return 0;
	} else if (searchLength == 1) {
		return indexOf(search.data[0]);
	}

	int*  prefixTable = getPrefixTable(search);


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
			delete prefixTable;
			return compareCursor - searchLength;
		}
	}
	delete prefixTable;
	return -1;
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

int* const getPrefixTable(const String& search) {
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
	// 优化nextVal数组
	if(compChar[1]==compChar[0]) {
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
			// 将每次比较的右指针指向后缀字串的开头
			int rightCursor = prefixLength - compLength;
			int leftCursor = 0;
			// cout<<"rightCursor reset: "<<rightCursor<<endl;
			bool isAllEqual = true;
			// leftCursor从0开始,适合与比较字串的长度相比较
			while (leftCursor < compLength) {
				if (compChar[leftCursor++] != compChar[rightCursor++]) {
					isAllEqual = false;
					break;
				}
			}
			// 如果全比较一遍发现两比较字串完全相同
			if (isAllEqual) {
				maxEqualLength = compLength;
			}
		}
		if(compChar[prefixLength] == compChar[maxEqualLength]) {
			// 可以优化成nextVal数组
			table[prefixLength] = table[maxEqualLength];
		} else {
			// 原始的next数组
			table[prefixLength] = maxEqualLength;
		}
	}

	cout << "prefixTable: ";
	for (int i = 0; i < tableLength; i++) {
		cout << table[i] << " ";
	}
	cout << endl;
	return table;
}

int* const getSuffixTable(const String& search) {
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
	if(compChar[last2]==compChar[rightBound]) {
		// 优化nextVal数组
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
			// 将每次比较的右指针指向后缀字串的开头
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
			// 如果全比较一遍发现两比较字串完全相同
			if (isAllEqual) {
				// 这个在函数外部就会充当模式串的找下一比对字符的下标
				maxEqualLength = rightBound - compLength;
			}
		}
		const int suffixTableGenerateCursor = rightBound - suffixLength;
		if(compChar[suffixTableGenerateCursor]==compChar[maxEqualLength]) {
			table[suffixTableGenerateCursor] = table[maxEqualLength];
		} else {
			table[suffixTableGenerateCursor] = maxEqualLength;
		}
//		cout << "table index: " << rightBound - suffixLength << endl;
	}

	cout << "suffixTable: ";
	for (int i = 0; i < tableLength; i++) {
		cout << table[i] << " ";
	}
	cout << endl;
	return table;
}

String& String::replace(const String& search, const String& with, bool merge, int direction) {
	int startIndex;
	// 根据方向来选择使用哪种方式replace
	if (direction == LEFT) {
		startIndex = indexOf(search);
	} else if (direction == RIGHT) {
		startIndex = lastIndexOf(search);
	} else {
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
		delete data;
		data = resultChar;
		_length = resultLength;
		return *this;
	}

	return *(new String(resultChar, resultLength));
}

String& String::replaceFirst(const String& search, const String& with, bool merge) {
	return replace(search, with, merge, LEFT);
}
String& String::replaceLast(const String& search, const String& with, bool merge) {
	return replace(search, with, merge, RIGHT);
}
String& String::replaceAll(const String& search, const String& with, bool merge) {
	vector<int> allStartIndexes = allIndexOf(search);
	int size = allStartIndexes.size();
	int resultLength = _length - (search._length * size) + (with._length * size);
//	cout<<"replaceAll resultLength: "<<resultLength<<endl;
	char* resultChar = new char[resultLength + 1];

	int cursor = 0;
	int dataCursor = 0;
	for (int i = 0; i < size; i++) {
		int startIndex = allStartIndexes[i];
		int replaceWithStringCursor = 0;

		while (cursor < startIndex) {
			resultChar[cursor++] = data[dataCursor++];
		}

		while (replaceWithStringCursor < with._length) {
			resultChar[cursor++] = with.data[replaceWithStringCursor++];
		}

		dataCursor += search._length;
	}

	while (cursor < resultLength) {
		resultChar[cursor++] = data[dataCursor++];
	}

	resultChar[resultLength] = 0;
	
	if (merge) {
		delete data;
		data = resultChar;
		_length = resultLength;
		return *this;
	}

	return *(new String(resultChar, resultLength));
}

String& String::replaceWith(const String& search, const String& with, int direction) {
	return replace(search, with, true, direction);
}
String& String::replaceFirstWith(const String& search, const String& with ) {
	return replaceFirst(search, with, true);
}
String& String::replaceLastWith(const String& search, const String& with ) {
	return replaceLast(search, with, true);
}
String& String::replaceAllWith(const String& search, const String& with ) {
	return replaceAll(search, with, true);
}

bool String::contain(const char search) {
	return indexOf(search) != -1;
}
bool String::contain(const String& sub) {
	return indexOf(sub) != -1;
}
bool String::startWith(const String& prefix) {
	const char* search = prefix.data;
	const int searchLength = prefix._length;
	for (int i = 0; i < searchLength; i++) {
		if (data[i] != search[i]) {
			return false;
		}
	}
	return true;
}
bool String::endWith(const String& suffix) {
	const char* search = suffix.data;
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
	return startWith(prefix);
}
bool String::hasSuffix(const String& suffix) {
	return endWith(suffix);
}

int String::toInt() {
	return atoi(data);
}
long String::toLong() {
	return atol(data);
}
long long String::toLLong() {
	return atoll(data);
}
float String::toFloat() {
	return atof(data);
}
unsigned int String::toUInt() {
	return atoi(data);
}
unsigned long String::toULong() {
	return atol(data);
}
unsigned long long String::toULLong() {
	return atoll(data);
}
double String::toDouble() {
	return atof(data);
}

vector<String>& String::split(const String& splitString) {
	return *(new vector<String>());
};

vector<String>& String::split(const char symbol) {
	int startIndex = 0;
	int endIndex = 0;
	vector<String>* result = new vector<String>();
	for (; endIndex < _length; endIndex++) {
		if (data[endIndex] == symbol) {
			result->push_back(subString(startIndex, endIndex));
			startIndex = endIndex + 1;
		}
	}
	// 将剩余的一个字符串装入
	result->push_back(subString(startIndex, endIndex));

	return *result;
}

String& String::join(vector<String>& vec, const char symbol) {
	int size = vec.size();
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

	return *(new String(result));
}
String& String::join(String stringArray[], int length, const char symbol) {
	int resultLength = 0;

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

	return *(new String(result));
}
String& String::join(vector<String>& vec, const String& interval) {
	int size = vec.size();
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

	return *(new String(resultChar));
}

String& String::slice(int startIndex, int charCount, bool merge) {
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
		copyAndReplace(temp.data, temp._length);
		_length = temp._length;
		delete &temp;
		return *this;
	}

	return subString(startIndex, startIndex + charCount);
}
String& String::slice(int startIndex, bool merge) {
	return slice(startIndex, -1, merge);
}
String& String::splice(int startIndex, int charCount) {
	return slice(startIndex, charCount, true);
}

String& String::trim(int trimMode, bool merge) {
	if (isEmpty()) {
		return *this;
	}

	int startIndex = 0;
	int endIndex = _length - 1;

	switch (trimMode) {
		case BOTH:
			if (data[endIndex] == ' ') {
				while (endIndex > 0) {
					if (data[endIndex] != ' ') {
						break;
					}
					endIndex--;
				}
			}
		case LEFT:
			if (data[startIndex] == ' ') {
				while (startIndex < _length) {
					if (data[startIndex] != ' ') {
						break;
					}
					startIndex++;
				}
			}
			break;
		case RIGHT:
			if (data[endIndex] == ' ') {
				while (endIndex > 0) {
					if (data[endIndex] != ' ') {
						break;
					}
					endIndex--;
				}
			}
			break;
	}

	if (merge) {
		String temp = subString(startIndex, endIndex + 1);
		copyAndReplace(temp.data, temp._length);
		_length = temp._length;
		delete &temp;
		return *this;
	}

	return subString(startIndex, endIndex + 1);
};

String& String::trimLeft(bool merge) {
	return trim(LEFT, merge);
}
String& String::trimRight(bool merge) {
	return trim(RIGHT, merge);
}

String& String::trimSelf(int trimMode) {
	return trim(trimMode, true);
}

String& String::trimSelfLeft() {
	return trim(LEFT, true);
}
String& String::trimSelfRight() {
	return trim(RIGHT, true);
}

String& String::append(const String& target, bool merge) {
	return insert(target, _length, merge);
};

String& String::prepend(const String& target, bool merge) {
	return insert(target, 0, merge);
};

void String::print() {
	cout << data;
}
void String::println() {
	cout << data << endl;
}

String& String::operator=(const char* charString) {
	_length = strlen(charString);
	copyAndReplace(charString, _length);
	return *this;
};

String& String::operator=(const char aChar) {
	delete data;
	this->_length = 1;
	data = new char[2];
	data[0] = aChar;
	data[1] = 0;
	return *this;
}

String& String::operator=(const int num) {
	delete data;
	sprintf(data, "%d", num);
	_length = strlen(data);
	return *this;
}
String& String::operator=(const unsigned num) {
	delete data;
	sprintf(data, "%d", num);
	_length = strlen(data);
	return *this;
}
String& String::operator=(const long num) {
	delete data;
	sprintf(data, "%ld", num);
	_length = strlen(data);
	return *this;
}
String& String::operator=(const long long num) {
	delete data;
	sprintf(data, "%lld", num);
	_length = strlen(data);
	return *this;
}
String& String::operator=(const unsigned long num) {
	delete data;
	sprintf(data, "%lud", num);
	_length = strlen(data);
	return *this;
}
String& String::operator=(const unsigned long long num) {
	delete data;
	sprintf(data, "%llud", num);
	_length = strlen(data);
	return *this;
}
String& String::operator=(const float num) {
	delete data;
	sprintf(data, "%f", num);
	_length = strlen(data);
	return *this;
}
String& String::operator=(const double num) {
	delete data;
	sprintf(data, "%lf", num);
	_length = strlen(data);
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
/*
String& String::operator+(const char* string) {
	return insert(string, _length);
}
String& String::operator+(const String& string) {
	return insert(string, _length);
}
String& String::operator+(char aChar) {
	char* temp = new char[2] {aChar, 0};
	// 使用拷贝构造器构造
	String* result = new String(temp);
	delete temp;
	return insert(*result, _length);
}
String& String::operator+(int num) {
	String* result = &insert(to_string(num).data(), _length);
	return *result;
}
String& String::operator+(unsigned num) {
	char* temp = new char[COPY_INT_BUFFER_SIZE];
	sprintf(temp, "%u", num);
	String* result = &insert(temp, _length);
	delete temp;
	return *result;
}
String& String::operator+(long num) {
	char* temp = new char[COPY_STRING_BUFFER_SIZE];
	sprintf(temp, "%ld", num);
	String* result = &insert(temp, _length);
	delete temp;
	return *result;
}
String& String::operator+(long long num) {
	char* temp = new char[COPY_STRING_BUFFER_SIZE];
	sprintf(temp, "%lld", num);
	String* result = &insert(temp, _length);
	delete temp;
	return *result;
}
String& String::operator+(unsigned long num) {
	char* temp = new char[COPY_STRING_BUFFER_SIZE];
	sprintf(temp, "%lud", num);
	String* result = &insert(temp, _length);
	delete temp;
	return *result;
}
String& String::operator+(unsigned long long num) {
	char* temp = new char[COPY_STRING_BUFFER_SIZE];
	sprintf(temp, "%lld", num);
	String* result = &insert(temp, _length);
	delete temp;
	return *result;
}
String& String::operator+(float num) {
	char* temp = new char[COPY_STRING_BUFFER_SIZE];
	sprintf(temp, "%f", num);
	String* result = &insert(temp, _length);
	delete temp;
	return *result;
}
String& String::operator+(double num) {
	char* temp = new char[COPY_STRING_BUFFER_SIZE];
	sprintf(temp, "%lf", num);
	String* result = &insert(temp, _length);
	delete temp;
	return *result;
}
 */

String& String::operator+=(const char* charString) {
	return insert(charString, _length, true);
};
String& String::operator+=(const String& string) {
	return insert(string.data, _length, true);
};

char& String::operator[](int index) {
	if (index < 0 || index >= _length) {
		throw OutOfBoundException(index, 0, _length - 1);
	}
	return data[index];
}

bool String::operator==(const char* charString) {
	return strcmp(data, charString) == 0;
}
bool String::operator==(const String& target) {
	return strcmp(data, target.data) == 0;
}


String StringBuilder::build() {
	int size = merbers.size();
	int resultLength = merbers[0].length();

	for (int i = 1; i < size; i++) {
		resultLength +=  merbers[i].length();
	}
	// 预留出结束符
	char* resultChar = new char[resultLength + 1];

	int cursor = 0;
	for (int i = 0; i < size; i++) {
		String el = merbers[i];

		int length = el.length();
		for (int i = 0; i < length; i++) {
			resultChar[cursor++] = el[i];
		}
	}

	resultChar[resultLength] = 0;
	merbers.clear();
	merbers.shrink_to_fit();
	return String(resultChar);
}

StringBuilder::~StringBuilder() {}

StringBuilder& StringBuilder::operator+(const String& target) {
	merbers.push_back(target);
	return *this;
}

StringBuilder& StringBuilder::append(const String& target) {
	merbers.push_back(target);
	return *this;
}
