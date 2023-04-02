#pragma once
#include <string>
using namespace std;
class StringHelper;

class StringReplacer;

class String;

#define PROPERTY(returnType, propertyName, paramType, paramName) \
	returnType propertyName(paramType paramName){ 		\
		this->paramName = paramName;                        \
		return *this;                                       \
	}                                                     \
	paramType propertyName(){ return paramName; }
#define CONST_PROPERTY(returnType, propertyName, paramType, paramName) \
	returnType propertyName(const paramType paramName){ 		\
		this->paramName = const_cast<paramType>(paramName);   \
		return *this;                                       \
	}                                                     \
	paramType propertyName(){ return paramName; }

class StringHelper {
	protected:
		String* target = nullptr;
		bool _merge = false;
		StringHelper(String* target, const bool& _merge): target(target), _merge(_merge) {};
		StringHelper(String* target): target(target) {};
	public:
};
class StringReplacer : public StringHelper {
	protected:
		string _search = "";
		string _with = "";
		int _replaceIndex = 0;
		int _direction;
		bool _all = false;
		StringReplacer(
			String* target,
		    const bool& _merge,
		    string _search,
		    string _with,
			const int& _replaceIndex,
		    int _direction,
			bool _all):
			StringHelper(target, _merge),
			_search(_search),
			_with(_with),
			_replaceIndex(_replaceIndex),
			_direction(_direction),
			_all(_all){};
	public:
		StringReplacer(String* target);
		PROPERTY(StringReplacer, merge, bool, _merge)
		PROPERTY(StringReplacer, search, string, _search)
		PROPERTY(StringReplacer, with, string, _with)
		PROPERTY(StringReplacer, replaceIndex, int, _replaceIndex)
		PROPERTY(StringReplacer, direction, int, _direction)
		PROPERTY(StringReplacer, all, bool, _all)

		String replace();
};
