#pragma once
#include <functional>
#include "Exception.h"
template<typename T>
class PointerBox {
	T* content = nullptr;
	PointerBox(){}
	PointerBox(T* c):content(c){}
public:
	static PointerBox& of(T* content){
		NullPointerException::check(content,"of::NullPointerException");
		return PointerBox(content);
	}
	
	static PointerBox& ofNullable(T* content) {
		return PointerBox(content);
	}
	
	static PointerBox& empty(){
		return PointerBox();
	}
	
	T* orElse(T* defaultCont){
		NullPointerException::check(defaultCont,"orElse::NullPointerException");
		return defaultCont;
	}
	
	T* orElseGet(function<T*()> supplier){
		T* result = supplier();
		NullPointerException::check(supplier,"orElseGet::NullPointerException");
		return result;
	}
	
	PointerBox& orElseThrow(RuntimeException& e){
		if(content==nullptr){
			throw e;
		}
		return *this;
	}
	
	void ifPresent(function<void(T*)> operation){
		if(content == nullptr) {return;}
		operation(content);
	}
};

template<typename T>
class Optional {
public:
	enum class OptionState {
		VAILD,
		INVALID
	};
	
	static Optional& of(T content){
		return Optional(content,OptionState::VAILD);
	}
	
	static Optional& ofInvalid(T content) {
		return Optional(content,OptionState::INVALID);
	}
	
	static Optional& empty(){
		static Optional emp = Optional();
		return emp;
	}
	
	T& orElse(T& defaultCont){
		return defaultCont;
	}
	
	T& orElseGet(function<T&()> supplier){
		return supplier();
	}
	
	Optional& orElseThrow(RuntimeException& e){
		if(state==OptionState::INVALID){
			throw e;
		}
		return *this;
	}
	
	void ifPresent(function<void(T&)> operation){
		if(state == OptionState::INVALID) {return;}
		operation(content);
	}
	
private:
	T content;
	OptionState state = OptionState::INVALID;
	Optional():state(OptionState::INVALID){}
	Optional(T c, OptionState state):content(c),state(state){}
};
