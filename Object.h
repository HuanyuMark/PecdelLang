#pragma once
#include <iostream>
class String;
class Class;

using namespace std;
class Object {
protected:
	static Class clazz;
public:
	typedef unsigned long long ull;
	typedef long long ll;
	Object(){}
	Object(const Object& obj){
		//cout<<"copy Object"<<endl;
	}
	virtual ~Object(){
		// cout<<"virtual ~Object()"<<endl;
	}
	
	static ull hashCode(const void* pointer) {
		void* temp = (void*) pointer;
		void** pointerAdress = &temp;
		ull* result = (ull*) pointerAdress;
		return *result;
	}
	
	virtual bool operator==(const Object& other) {
		return &other == this;
	}
	
	virtual String toString();

	virtual ull hashCode() {
		void* pointerAdress = this;
		void** pointerAAdress = &pointerAdress;
		ull* ullPointer = (ull*) pointerAAdress;
		return *ullPointer;
	}

	virtual Class getClass();
};

class Class {
	const char* _name;
public:
	Class(const char* _name):
	 _name(_name){}
	String name();
};

Class Object::clazz = Class("Object");

Class Object::getClass(){
	return clazz;
}


