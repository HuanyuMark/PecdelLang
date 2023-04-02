#pragma once
#include <iostream>
#include <functional>
#include <mutex>
#include <atomic>
#include "Logger.h"
using namespace std;

//class E {
//	public:
//		E() {
//			cout << "constructor" << endl;
//		}
//};



template<typename P>
class SharedPtr {
		P  pointer = nullptr;

		int* referenceCount = nullptr;

		function<void(P)> _dellocator = defaultDellocator;

		/**
		 * @brief 这里多线程还是不能正常工作
		 * 因为即使有一个线程持锁将referenceCount置空,其它同pointer的referenceCount
		 * 却不会被置空,导致非法访问
		 * f1:
		 *   将referenceCount升级为二级指针, 有一个线程置空了一级指针,那所有二级指针
		 *   解引用之后,都会是nullptr,从而防止此问题, 但是, 该如何判断什么时候释放二级指针呢?
		 * f2:
		 *	 逆转依赖,多设计一个类变量, 一个Map<pointer*,refCount>, 专门来收集所有SharedPtr的引用计数
		 *   在SHaredPtr析构时,查表, 减少对应key的value, 到0就delete pointer
		 * f3:
		 *   在SharedPtr析构时, 不主动释放控制块(refCount), 转由RefCount类的一个类变量来
		 *   进行控制块的分发和释放
		 */
		inline void dec() {
			if (pointer == nullptr) {
				if (referenceCount == nullptr) {
					return;
				}
				int* temp = referenceCount;
				referenceCount = nullptr;
				delete temp;
				return;
			}

			if (referenceCount == nullptr) {
				return;
			}


			logger.trace([this]() {
				cout << "before dec refcount: " << *referenceCount << endl;
			});
			
			(*referenceCount)--;

			if (*referenceCount > 0) {
				return;
			}

			logger.trace([this]() {
				cout << "permited to delete resource: " << (int*) pointer << endl;
			});

			_dellocator(pointer);

			int* temp = referenceCount;
			referenceCount = nullptr;
			pointer = nullptr;
			delete temp;
		}

		/**
		 * @brief 这个方法用来调整两个共享指针的引用计数
		 * 主动使用该方法时, 若被传入的指针的pointer与本指针的pointer不同
		 * 则将本指针引用计数-1, 传入的引用计数+1 (计数-1必须调用dec()方法,以便于及时释放内存)
		 * 因为, 被"赋值"的本指针, 应当要丢失原本指针的引用
		 * @param other
		 *
		 * @return
		 */
		SharedPtr& adjustReference(const SharedPtr& other) {
//			logger.trace([this, &other]() {
//				cout << "now pointer: " << (int*)pointer << "other pointer" << (int*)other.pointer << endl;
//			});
			// 若pointer指向的不是有效地址, 可能会被错误释放
			// 所以只要涉及到直接用拷贝构造赋值给未清空内存时,
			// 因为被覆盖内存的pointer段不为空,则会触发错误释放, 所以只要涉及到
			// SharedPtr覆盖内存的场景, 务必要先重置一遍内存
			if (pointer != nullptr && other.pointer != pointer) {
				logger.trace([]() {
					cout << "[SharedPtr] dec the replaced one" << endl;
				});
				dec();
			}
			pointer = other.pointer;
			referenceCount = other.referenceCount;
			_dellocator = other._dellocator;
			*(referenceCount) += 1;
			return *this;
		}

		static Logger logger;
	public:
		static void defaultDellocator(P t) {
			logger.trace([t]() {
				cout << "defaultDellocator: " << (int*)t << endl;
			});
			delete t;
		};

		static void arrayDellocator(P t) {
			logger.trace([t]() {
				cout << "arrayDellocator: " << (int*) t << endl;
			});
			delete[] t;
		};


		SharedPtr(P pointer, const function<void(P)> dellocator): pointer(pointer), _dellocator(dellocator) {
			logger.trace([]() {
				cout << "SharedPtr(T pointer, void (*dellocator)(T))" << endl;
			});
			referenceCount = new int(1);
		}

		explicit  SharedPtr(P pointer): pointer(pointer) {
			logger.trace([]() {
				cout << "SharedPtr(T pointer)" << endl;
			});
			referenceCount = new int(1);
		}

		SharedPtr() {
			logger.trace([]() {
				cout << "SharedPtr()" << endl;
			});
			referenceCount = new int(1);
		}

		SharedPtr(const SharedPtr& other) noexcept {
			logger.trace([]() {
				cout << "SharedPtr(const SharedPtr& other)" << endl;
			});
			adjustReference(other);
		}

		SharedPtr& swapPointer(SharedPtr& p1) {
			P  temp = p1.pointer;
			p1.pointer = pointer;
			pointer = temp;
			return *this;
		}

		const P get() const {
			return pointer;
		}

		SharedPtr& dellocator(function<void(P)> _dellocator) {
			this->_dellocator = _dellocator;
			return *this;
		}

		int refCount() const {
			return *referenceCount;
		}

		auto& operator[](long long index) const {
			return pointer[index];
		}

		P operator->() const {
			return pointer;
		}

		auto operator&() const {
			return &pointer;
		}

		auto operator*() const {
			return *pointer;
		}

		SharedPtr& operator=(const SharedPtr& other) {
			logger.trace([]() {
				cout << "operator=(const SharedPtr& other)" << endl;
			});
			adjustReference(other);
			return *this;
		}

		~SharedPtr() {
//			logger.trace([this]() {
//				cout <<"try to delete pointer"<<pointer<<" now rc: " <<*(referenceCount) << endl;
//			});
			dec();
		}
};

template<typename T>
Logger SharedPtr<T>::logger = Logger::createLogger([](function<void()> operation) {
	cout << "(SharedPointer) ";
	operation();
});
