#pragma once
#include <mutex>
#include <atomic>
#include <iostream>

#include "Logger.h"
using namespace std;

template<typename P>
class ReferenceController {
		P _pointer = nullptr;
		atomic<int> referCount = 1;
		atomic<int> threadCount = 0;
		bool willBeDestroyed = false;
		function<void(P)> _dellocator;
		mutex locker;
		static Logger logger;
		ReferenceController() {}
		~ReferenceController() {}
		ReferenceController(P _pointer, function<void(P)> _dellocator): _pointer(_pointer), _dellocator(_dellocator) {}
	public:
		static ReferenceController* createController(P _pointer, function<void(P)> _dellocator) {
			return new ReferenceController(_pointer, _dellocator);
		}

		inline const P pointer() {
			return _pointer;
		}

		ReferenceController* pointer(P _pointer) {
			this->_pointer = _pointer;
			return this;
		}

		ReferenceController* dellocator(function<void(P)> _dellocator) {
			if (_dellocator) {
				this->_dellocator = _dellocator;
			}
			logger.warn([]() {
				cout << "Invalid dellcator" << endl;
			});
			return *this;
		}



		void dec() {
			if (_pointer == nullptr) {
				return;
			}
			// 先统计一下, 有几个线程进来了
			threadCount++;
			// 必须要强制获得锁,因为这次析构,要么要让引用计数减一,要么就资源已被释放,可以直接退出
			locker.lock();
			//可能将要被释放了 - 效率低,因为这里一个一个线程先持锁再释放锁最后返回,不是并发的
			if (willBeDestroyed) {
				locker.unlock();
				threadCount--; // 线程退出, 当然要将计数减一
				return;
			}

			logger.trace([&]() {
				cout << "before dec refcount: " << referCount << endl;
			});

			referCount--;

			if (referCount > 0) {
				locker.unlock();
				threadCount--;
				return;
			}

			logger.trace([&]() {
				cout << "permited to delete resource: " << (int*) _pointer << endl;
			});

			_dellocator(_pointer);
			_pointer = nullptr;
			// 将要销毁本控制块实例
			willBeDestroyed = true;
			logger.trace([&]() {
				cout << "will destroy controller: " << this << endl;
			});

			// 让其它堵塞的线程进来,若willBeDestroy,就会直接返回
			locker.unlock();
			// 一直尝试是否可以释放自身
			while (true) {
				if (threadCount <= 1) {
					logger.trace([&]() {
						cout << "destroy controller: " << this << endl;
					});
					delete this; // 最后一步再删除,在析构中释放锁
					return;
				}
			}
		}

		inline void inc() {
			referCount++;
		}

		inline int refCount() {
			return referCount;
		}
};

template<typename P>
class ConcurrentSharedPtr {
		ReferenceController<P>* controller = nullptr;
		/**
		 * @brief 这个方法用来调整两个共享指针的引用计数
		 * 主动使用该方法时, 若被传入的指针的pointer与本指针的pointer不同
		 * 则将本指针引用计数-1, 传入的引用计数+1 (计数-1必须调用dec()方法,以便于及时释放内存)
		 * 因为, 被"赋值"的本指针, 应当要丢失原本指针的引用
		 * @param other
		 *
		 * @return
		 */
		ConcurrentSharedPtr& adjustReference(const ConcurrentSharedPtr& other) {
			//			logger.trace([this, &other]() {
			//				cout << "now pointer: " << (int*)pointer << "other pointer" << (int*)other.pointer << endl;
			//			});
			// 若pointer指向的不是有效地址, 可能会被错误释放
			// 所以只要涉及到直接用拷贝构造赋值给未清空内存时,
			// 因为被覆盖内存的pointer段不为空,则会触发错误释放, 所以只要涉及到
			if (other.controller == nullptr) {
				return *this;
			}
			// ConcurrentSharedPtr覆盖内存的场景, 务必要先重置一遍内存
			if (controller != nullptr
			    && controller != other.controller) {
				logger.trace([]() {
					cout << "[ConcurrentSharedPtr] dec the replaced one" << endl;
				});
				controller->dec();
			}
			controller = other.controller;
			controller->inc();
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

		ConcurrentSharedPtr(P pointer, const function<void(P)> dellocator) {
			logger.trace([]() {
				cout << "ConcurrentSharedPtr(T pointer, void (*dellocator)(T))" << endl;
			});
			controller = ReferenceController<P>::createController(pointer, dellocator);
		}

		explicit  ConcurrentSharedPtr(P pointer) {
			logger.trace([]() {
				cout << "ConcurrentSharedPtr(T pointer)" << endl;
			});
			controller = ReferenceController<P>::createController(pointer, defaultDellocator);
		}

		ConcurrentSharedPtr() {
			logger.trace([]() {
				cout << "ConcurrentSharedPtr()" << endl;
			});
		}

		ConcurrentSharedPtr(const ConcurrentSharedPtr& other) noexcept {
			logger.trace([]() {
				cout << "ConcurrentSharedPtr(const ConcurrentSharedPtr& other)" << endl;
			});
			adjustReference(other);
		}

		ConcurrentSharedPtr& swapPointer(ConcurrentSharedPtr& other) {
			P  temp = controller->pointer();
			controller->pointer(other.controller->pointer());
			other.controller->pointer(temp);
			return *this;
		}

		const P get() const {
			return controller->pointer();
		}

		ConcurrentSharedPtr& dellocator(function<void(P)> _dellocator) {
			controller->dellocator(_dellocator);
			return *this;
		}

		int refCount() const {
			return controller->refCount();
		}

		auto& operator[](long long index) const {
			return controller->pointer()[index];
		}

		P operator->() const {
			return controller->pointer();
		}

		auto operator*() const {
			return *(controller->pointer());
		}

		ConcurrentSharedPtr& operator=(const ConcurrentSharedPtr& other) {
			logger.trace([]() {
				cout << "operator=(const ConcurrentSharedPtr& other)" << endl;
			});
			adjustReference(other);
			return *this;
		}

		~ConcurrentSharedPtr() {
			logger.trace([this]() {
				cout <<"~ConcurrentSharedPtr(). call controlle dec"<< endl;
			});
			controller->dec();
		}
};

template<typename T>
Logger ReferenceController<T>::logger = Logger::createLogger([](function<void()> operation) {
	cout << "(ReferenceController) ";
	operation();
});


template<typename T>
Logger ConcurrentSharedPtr<T>::logger = Logger::createLogger([](function<void()> operation) {
	cout << "(ConcurrentSharedPtr) ";
	operation();
});



