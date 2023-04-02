#include <iostream>
#include <array>
#include "String.h"
using namespace std;
class MyClass1 : public Object {
	protected:
		int number = 999;
	public:
		virtual ~MyClass1() {
			cout << "~MyClass1" << endl;
		}
	
		MyClass1(){}
	
		MyClass1(const MyClass1& c1){
			cout<<"copy MyClass1"<<endl;
		}

		virtual void display() {
			cout << "MyClass1: " << this << endl;
		};

		virtual void noOverride() {
			cout << "Myclass1 noOverride" << endl;
		}

		ull hashCode() {
			return 1;
		}
};

class MyClass2 : public MyClass1 {
		int num = 0;
	public:
//	MyClass1 mc1;
		MyClass2(){}
	
		MyClass2(const MyClass2& c2){
			cout<<"copy MyClass2"<<endl;
		}
		
		~MyClass2() {
			cout << "~MyClass2()" << endl;
		}

		void display();

		Class getClass() {
			return Class("MyClass2");
		}
};

void MyClass2::display() {
	cout << "MyClass2: " << this << endl;
}
class MyClass3 {
	public:
		void call(const MyClass1& c1) {
			cout << "call" << endl;
			((MyClass1&)c1).display();
		}
};

template<typename T>
class TArray {
	public:
		T** data = nullptr;
		int length = 0;
		TArray(initializer_list<T> list) : length(list.size()) {
			size_t pointerSize = sizeof(void*)*length;
			size_t elSize = sizeof(T);
			void** data = (void**) malloc(pointerSize);
			memset(data, 0, pointerSize);
			int i = 0;
			for (auto it = list.begin(); it != list.end(); it++) {
				void* voidEl = malloc(elSize);
				memset(voidEl, 0, elSize);
				T* el = (T*) voidEl;
				cout << "call = *it copy" << endl;
				*(el) = (T) *it;
				data[i++] = el;
			}

			this->data = (T**) data;
		}

		T& operator[](int index) {
			return *((T*)data[index]);
		}
};

class Father {
public:
	virtual void display(){
		cout<<"Father display()"<<endl;
	}
	virtual ~Father(){}
};

class Son : public Father {
public:
	int num = 999;
	void display(){
		cout<<"Son display()"<<endl;
	}
};

int main() {

	{
		array<Father,3> arr{Son(),Son(),Son()};
		
		for(int i=0;i<arr.size();i++){
			static_cast<Son&>(arr[i]).display();
			cout<<static_cast<Son&>(arr[i]).num<<endl;
		}
	}
	
	return 0;
}
