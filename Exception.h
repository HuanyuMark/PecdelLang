#pragma once
#include <string>
#include <exception>
using namespace std;
// 方便使用的异常基类, 更改msg就可以更改抛异常的信息
class RuntimeException : public exception {
	protected:
		string msg;
	public:
		RuntimeException() {
			msg = "RuntimeException!";
		}

		RuntimeException(string msg) {
			this->msg = msg;
		}

		string getMessage() {
			return msg;
		}

		const char* what() const noexcept  {
			return msg.c_str();
		}
	
		RuntimeException setMessage(const char* meg){
			this->msg = msg;
			return *this;
		}
};
// 数组越界异常
class OutOfBoundException : public RuntimeException {
public:
	OutOfBoundException() {
		msg = "OutOfBoundException!";
	}
	OutOfBoundException(string msg) {
		this->msg = msg;
	}
	OutOfBoundException(int outOfIndex, int minimum, int maximun) {
		string result;
		result.append("index: ");
		result.append(to_string(outOfIndex));
		result.append(" was out of [");
		result.append(to_string(minimum));
		result.append(",");
		result.append(to_string(maximun));
		result.append("]");
		msg = result;
	}
};

class NullPointerException : public RuntimeException {
public:
	NullPointerException() {
		msg = "NullPointerException!";
	}
	NullPointerException(string msg) {
		this->msg = msg;
	}
		
	inline static bool check(void* something, string msg) {
		if(something==nullptr) {
			throw NullPointerException(msg+ " cannnot be null");
		}
		return true;
	}
	
	inline static bool check(const void* something, string msg) {
		if(something==nullptr) {
			throw NullPointerException(msg+ " cannnot be null");
		}
		return true;
	}
	
	static NullPointerException defaultText(string msg) {
		return NullPointerException(msg+ " cannnot be null");
	}
};

class IllegalArgumentsException : public RuntimeException {
public:
	IllegalArgumentsException() {
		msg = "IllegalArgumentsException!";
	}
	IllegalArgumentsException(string msg) {
		this->msg = msg;
	}
};
