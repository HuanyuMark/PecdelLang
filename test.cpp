#include <iostream>
#include <vector>
#include<thread>
#include "Array.h"
#include "String.h"
using namespace std;

//class Dog {
//		int _num;
//	public:
//		int num() {
//			return _num;
//		}
//		Dog(int _num): _num(_num) {}
//		~Dog() {
//			cout << "delete a Dog: " << _num << endl;
//		}
//};
//
//Array<int> aMethod() {
////	int* result = new int[3]{1,2,3};
////	ArrayBuilder<int> builder = Array<int>::builder();
//	vector<int> result;
//	result.push_back(1);
//	result.push_back(2);
//	result.push_back(3);
////	builder.push(1);
////	builder.push(2);
////	builder.push(3);
//	/*
//	  static int (*f)(int) = [](int a){return a+8;};
//	  return Array(result,f);//这样
//	 */
////	return Array(result,
////		(int (*)(int)) [](int a)->int{return a+8;}
////		);// 或者这样将lambda显式强转
//	return Array<int>(result,
//	[](int a) {return a + 8;});// 或者这样把模板写上
//}
//Array<Dog> bMethod() {
////	list<Dog> result;
////
////	result.push_back(Dog(1));
////	result.push_back(Dog(2));
////	result.push_back(Dog(3));
//
//	return Array<Dog> {Dog(1), Dog(2), Dog(875)};
//}
//void cMethod() {
//	auto releaser = [](int t[]) {
//		cout << "deleting" << endl;
//		for (int i = 0; i < 2; i++) {
//			cout << "el " << i << t[i] << endl;
//		}
//		cout << "deleted" << endl;
//		delete[] t;
//	};
//
//	shared_ptr<int[]> iii = shared_ptr<int[]>(new int[2], releaser, allocator<int[]>());
//
//	iii.reset();
////	iii = shared_ptr<int[]>(new int[]{1,2}, releaser, allocator<int[]>());
////	iii.reset(new int[]{1,2});
//}

class MyClass : public String {
public:
	String toString(){
		return "MyClass toString()";
	}
};

int main() {
	Logger::level(Logger::LogLevel::TRACE);
//	Array<Array<int>>  twoDint{{1,2,3},{4,5,6},{7,8,9}};
//	array<array<int,3>,3> twoDint{{1,2,3},{4,5,6},{7,8,9}};

//	cMethod();
//	Array<Dog> dogArray = bMethod();

//	dogArray[1] = Dog(99);
//	dogArray.replace(1,Dog(99));

//	for(int i=0;i<dogArray.length;i++){
//		cout<<"Dog: "<<dogArray[i].num()<<endl;
//	}
//	for (Array<Dog>::Iterator it = dogArray.rbegin(); it != dogArray.rend(); it--) {
//		cout << "Dog: " << (*it).num() << endl;
//	}
//	{
////		Array<int> ints{1,2,3};
//		Array<Array<String>> intss{{1, 2, 3}, {4, 555, 6}, {7, 8, 9}};
////		ArrayBuilder<Array<int>> builder;
////		builder.push(Array<int>{1,2,3});
////		builder.push(Array<int>{4,5,6});
////		builder.push(Array<int>{7,8,9});
////		Array intss = builder.build();
//
//
//		cout << "out put, intss.length: " << intss.length() << endl;
//		for (int i = 0; i < intss.length(); i++) {
//			for (int j = 0; j < intss[i].length(); j++) {
//				cout << " " << intss[i][j].toChars();
//			}
//			cout<<endl;
//		}
////		cout<<intss.toString().toChars()<<endl;
//		cout << "exit the code block" << endl;
//	}
	{
		Array<Object> strs{String("12553"),String("abc"),String("xyz")};

		String ss = "issssss";
		
		Object& sss = ss;
		
		cout <<"##########"<<sss.toString().toChars()<<endl;
		
		cout<<"before output"<<endl;
		for(int i=0;i<strs.length();i++){
			cout<<"get temp"<<endl;
			String temp = strs[i];
			cout<<"& operation"<<endl;
			String* tempp = &temp;
			cout<<tempp->toString().toChars()<<endl;
		}
		
//		cout<<strs.toString().toChars()<<endl;
		cout<<"exit code block"<<endl;
	}
//	{
//		Array<String> strs{"123", "456", "789"};
//		Array<String> newStrs = strs.map([](String& str){
//			return str.append("-s--");
//		});
//		//		strs[0] = "666";
//		//		strs[1] = "777";
//		//		strs[2] = "888";
//		for (int i = 0; i < strs.length(); i++) {
//			cout << strs[i].toChars() << endl;
//		}
//
//		for (int i = 0; i < newStrs.length(); i++) {
//			cout << newStrs[i].toChars() << endl;
//		}
//		cout << "exit the code block" << endl;
//	}


//	Array<int> array = aMethod();
//
//	for (int i = 0; i < array.length; i++) {
//		cout << array[i] << endl;
//	}
	cout << "exit" << endl;
	system("pause");
	return 0;
}
