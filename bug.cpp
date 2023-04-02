#include <iostream>
using namespace std;

template<typename T>
class Array {
public:
	T* data = nullptr;

	Array(){}
	
	Array(T* pointer) : data(pointer){}
	
	T& operator[](int index){
		return data[index];
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


int main(){
	Array arr(new Son[3]());
	
	for(int i=0;i<3;i++){
		(arr[i]).display();
	}
	
	return 0;
}
