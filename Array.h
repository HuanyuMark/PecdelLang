#pragma once
#include <vector>
#include <list>
#include <initializer_list>
#include <iostream>
#include <string.h>
#include "Object.h"
#include "Exception.h"
#include "SharedPtr.h"
#include "Optional.h"
using namespace std;

typedef unsigned long long ull;
typedef long long ll;

template<typename T>
class Iterable : public Object {
		T next();
		bool hasNext();
		T* begin();
		T* end();
		T* rbegin();
};

template<typename T>
class Array;

template<typename T>
class ArrayBuilder : public Object {
		vector<T> menbers;
	public:
		ArrayBuilder push(const T& el) {
			menbers.push_back(el);
			return *this;
		}

		ArrayBuilder pop() {
			menbers.erase(menbers.end(), 1);
			return *this;
		}

		inline operator<<(const T& el) {
			return push(el);
		}

		Array<T> build();
};
#define ARRAY_COMMOM_CONSTRUCTOR(typeName) \
	Array(typeName<T> container): _length(container.size()) { \
		T* result = getEmptyMemory(_length * sizeof(T));\
		int i = 0;\
		for (auto it = container.begin(); it != container.end(); it++) {\
			result[i++] = *it;\
		}\
		data = newData(result);\
	}
#define ARRAY_ALLOCATOR_CONSTRUCTOR(typeName)\
	Array(typeName<T> container, T (*allocator)(T)): _length(container.size()) { \
		T* result = getEmptyMemory(_length * sizeof(T));\
		int i = 0;\
		for (auto it = container.begin(); it != container.end(); it++) {\
			result[i++] = allocator(*it);\
		}\
		data = newData(result);\
	}

class String;

template<typename T>
class Array : public Object {
		SharedPtr<T*> data;

		function<void(T*)> innerDellocator = [this](T* t) {
			logger.trace([t]() {
				cout << "free address: " << (int*)t << endl;
			});
			for (int i = 0; i < _length; i++) {
				/**
				 * 若data[i]的父类有虚析构
				 * 则在data[i]解引用后必须强转为(T)
				 * 给编译器足够的类型信息,选择使用哪个析构
				 * 否则会发生未定义错误(崩溃)
				 */
				((T)this->data[i]).~T();
			}
			/**
			 * -我还是担心这里有没有将t*数组释放干净
			 * -不会的, 因为malloc申请内存时,会在指针前的定长的一段空间里存储该指针申请
			 * 的空间大小等信息, free就会找到这个malloc时记录的信息,按照这个空间大小信息,释放
			 */
			free(t);
		};
		// 包装一下用户传递的自定义dellocator, 并返回
		function <
		function<void(T*)>(function<void(T*, Array&)>)
		> innerDellocatorFactory = [this](function<void(T*, Array&)> customDellocator) {
			return [this, &customDellocator](T * t) {
				logger.trace([t]() {
					cout << "customDellocator. address: " << (int*)t << endl;
				});
				cout<<"ok?"<<endl;
				customDellocator(t, *this);
				logger.trace([&](){
					cout<<"after cusdelloc"<<endl;
				});
			};
		};

		//function<void(T*, Array& thisArray)> _dellocator;

		inline SharedPtr<T*> newData(T data[] = nullptr) {
			logger.trace([]() {
				cout << "new Data" << endl;
			});
			return SharedPtr<T*>(data, innerDellocator);
		}
		inline void checkIndex(ll index) {
			if (index < 0 || index >= _length) {
				throw OutOfBoundException(index, 0, _length - 1);
			}
		}
		/**
		 * @brief 得到全清空的堆内存
		 */
		T* getEmptyMemory(size_t size) {
			/**
			 * 分配并全部初始化为0, 为什么要全置0?
			 * 里面可能会有SharedPtr, SharePtr在调用拷贝构造时,会调用dec()
			 * 来比较被赋值指针与赋值指针是狗相同,若不同(这里必不同),则会将被赋值指针引用计数-1
			 * 若被赋值指针引用计数为0,若pointer不为nullptr时,则触发delete,
			 * 在这里,若不全置0,则在强转后,SharedPtr的pointer必不为nullptr, 这样就会
			 * 触发delete,错误释放内存, 造成程序崩溃
			 * char* result = new char[size]();
			 * 与
			 * void* result =  malloc(size);
			 * memset(result,0,size);
			 * 等效, 但是不确定free和new配合会不会出bug
			 */
			void* result = malloc(size);
			if (result == nullptr) {
				logger.error([&]() {
					cout << "cannot allocate " << size << "Byte(s) heap memory" << endl;
				});
				throw bad_alloc();
			}
			memset(result, 0, size); // 初始化
			logger.trace([result]() {
				cout << "malloc address: " << (int*) result << endl;
			});
			return (T*) result;
		}

		static Logger logger;
	public:
		~Array() {
			logger.trace([this]() {
				cout << "destructor, this: " << this << endl;
			});
		}

		class Iterator {
				ll _cursor;
				Array* arr;
			public:
				Iterator(ll cursor, Array* arr);
				inline ll cursor();
				inline ll operator++();
				inline ll operator++(int);
				inline ll operator--();
				inline ll operator--(int);
				inline T operator*() const;
				inline bool operator!=(const Iterator& it);
		};
		int _length;
		inline int length() {return _length;}
		/**
		 * @brief
		 * 之所以还要将dellocator暴露出去, 是在一些情境下, 释放得自定义:
		 * 比如但T为指针类型时, data就是一个指针数组, delete [] data, 只会释放
		 * **指针类型固定占用的空间**, 至于指针所指向的空间则无法释放,这一块最好提供出来, 允许自定义释放
		 */
		Array dellocator(function<void(T*, Array& thisArray)> _dellocator) {
			innerDellocator = innerDellocatorFactory(_dellocator);
			data = newData(data.get());
//			data.dellocator(innerDellocator);
			return *this;
		}

		Array(T data[], ll length): _length(length) {
			this->data = newData(data /*innerDellocatorFactory([&](T p[]){

			  })*/ );
		}

		Array(ll length = 0): _length(length) {
			T* result = getEmptyMemory(length * sizeof(T)); //(T*) malloc();
			data = newData(result);
		}

		// 直接将vec[i]搬到堆中
		Array(vector<T> container): _length(container.size()) {
			//malloc分配数组空间,不调用构造
			T* result = getEmptyMemory(_length * sizeof(T));// (T*) malloc(length * sizeof(T));
			int i = 0;
			for (auto it = container.begin(); it != container.end(); it++) {
				result[i++] = *it;//拷贝构造
			}
			data = newData(result);
		}

		Array(initializer_list<T> container): _length(container.size()) {
			T* result = getEmptyMemory(_length * sizeof(T));;
			int i = 0;
			for (auto it = container.begin(); it != container.end(); it++) {
				// 换上自定义的sharedPtr就行
				result[i++] = *it;
			}
			data = newData(result);
		}

//		ARRAY_COMMOM_CONSTRUCTOR(initializer_list)
		ARRAY_COMMOM_CONSTRUCTOR(list)
		ARRAY_ALLOCATOR_CONSTRUCTOR(list)
		ARRAY_ALLOCATOR_CONSTRUCTOR(vector)

		static ArrayBuilder<T> builder();

		String toString();

		// 如果替换的是一个指针, 则被替换的那个指针所指向的空间可能就泄露了
		inline T& operator[](ll index) {
			checkIndex(index);
			return data[index];
		}

		inline T& at(ll index) {
			return operator[](index);
		}
	
		T read(ll index) {
			checkIndex(index);
			return data[index];
		}
	
		// 顺序起点
		Iterator begin() {
			return Iterator(0, this);
		}
		// 顺序终点
		const Iterator& end() {
			const static Iterator _end = Iterator(_length, this);
			return _end;
		}
		// 逆序起点
		Iterator rbegin() {
			return Iterator(_length - 1, this);
		}
		// 逆序终点
		const Iterator& rend() {
			const static Iterator _rend = Iterator(-1, this);
			return _rend;
		}

		Array map(function<T(T&)> mapper) {
			Array result = Array(_length);
			T* arr = data.get();
			for (int i = 0; i < _length; i++) {
				result[i] = mapper(arr[i]);
			}
			return result;
		}

		Array& map(function<T(T&, int)> mapper) {
			Array result = Array(_length);
			T* arr = data.get();
			for (int i = 0; i < _length; i++) {
				result[i] = mapper(arr[i], i);
			}
			return result;
		}

		Array& foreach (function<void(T&)> executor) {
			T* arr = data.get();
			for (int i = 0; i < _length; i++) {
				executor(arr[i]);
			}
			return *this;
		}

		Array& foreach (function<void(T&, int)> executor) {
			T* arr = data.get();
			for (int i = 0; i < _length; i++) {
				executor(arr[i], i);
			}
			return *this;
		}

		Array& filter(function<bool(T&)> condition) {
			T* arr = data.get();
			ArrayBuilder<T> builder;
			for (int i = 0; i < _length; i++) {
				if (!condition(arr[i])) {
					continue;
				}
				builder.push(arr[i]);
			}

			return builder.build();
		}

		Array& filter(function<bool(T&, int)> condition) {
			T* arr = data.get();
			ArrayBuilder<T> builder;
			for (int i = 0; i < _length; i++) {
				if (!condition(arr[i], i)) {
					continue;
				}
				builder.push(arr[i]);
			}

			return builder.build();
		}

		Optional<T>& find(function<bool(T&)> condition) {
			int index = findIndex(condition);

			if (index != -1) {
				return Optional<T>::of(data[index]);
			}

			return Optional<T>::empty();
		}

		Optional<T>& find(function<bool(T&, int)> condition) {
			T* arr = data.get();
			for (int i = 0; i < _length; i++) {
				if (!condition(arr[i], i)) {continue;}
				return Optional<T>::of(arr[i]);
			}

			return Optional<T>::empty();
		}

		ll findIndex(function<bool(T&)> condition) {
			T* arr = data.get();
			for (int i = 0; i < _length; i++) {
				if (!condition(arr[i])) {continue;}
				return i;
			}

			return -1;
		}

};
template<typename T>
Logger Array<T>::logger = Logger::createLogger([](function<void()> operation) {
	cout << "(Array) ";
	operation();
});

template<typename T>
Array<T> ArrayBuilder<T>::build() {
	return Array<T>(menbers);
}

template<typename T>
ArrayBuilder<T> Array<T>::builder() {
	return ArrayBuilder<T>();
}

template<typename T>
Array<T>::Iterator::Iterator(ll cursor, Array* arr):
	_cursor(cursor),
	arr(arr) {}

template<typename T>
ll Array<T>::Iterator::cursor() {
	return _cursor;
}

template<typename T>
ll Array<T>::Iterator::operator++() {
	++_cursor;
	return _cursor;
}

template<typename T>
ll Array<T>::Iterator::operator++(int delta) {
	ll res = _cursor;
	++_cursor;
	return res;
}

template<typename T>
ll Array<T>::Iterator::operator--() {
	--_cursor;
	return _cursor;
}

template<typename T>
ll Array<T>::Iterator::operator--(int delta) {
	ll res = _cursor;
	--_cursor;
	return res;
}

template<typename T>
T Array<T>::Iterator::operator*() const {
	return arr->at(_cursor);
}

template<typename T>
bool Array<T>::Iterator::operator!=(const Iterator& it) {
	// cout << "this cursor: " << _cursor << " comp cursor: " << it._cursor << endl;
	// [错误] comparison between distinct pointer types 'Array<Dog>*' and 'Array<Dog>::Iterator*' lacks a cast
	return it._cursor != _cursor || it.arr != arr;
}
