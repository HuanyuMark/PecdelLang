#include <iostream>
#include "String.h"
using namespace std;
int main() {
	{
		// 自动转换,调用了对应参数的构造相当于 String s(555);
		String s = 555;
		// 也可以这样,这样会自动做字符串转换
		//	s = 55555;
		// 在s的末尾追加字符, 这个不是新创建一个字符串然后再覆盖的, 是基于原本的串进行动态拼接的,效率高一些
		//	s += "000";
		//	s.append("000");
		cout << "initial: " << s.toChars() << " realLength: " << s.length() << endl;

//		String insertOne = "666666";

		s.insert("778899", 2, true);

		cout << "insert: " << s.toChars() << " realLength: " << s.length() << endl;
		// 不会改变原串
		String sliceString = s.slice(2, 6);

		// 改变原串
		// s.splice(2, 6); s.slice(2,6,true)

		cout << "sliceString: " << s.toChars() << " realLength: " << s.length() << endl;

//		system("pause");
	}

	{
		String testOperator = "123456";
		cout << "[]Operator: " << testOperator.toChars() << "[]Operator [2]: " << testOperator[2] << endl;
		testOperator[2] = '9';
		cout << "[]Operator: " << testOperator.toChars() << "[]Operator [3]: " << testOperator[2] << endl;
//		system("pause");
	}

	{
		String trimTarget = "   eeeee  ";
		trimTarget.trimSelf(String::RIGHT);
		cout << "trimTarget.trim(): ____" << trimTarget.length() << "____" << endl;
//		system("pause");
	}

	{
		String plain = "xxx";
//		String().join(8).join(9).;
		StringBuilder builder = plain.builder();
//		builder + 9;
		builder + 9 + "_" + "oopp";
		builder * 2;
//		builder/0;
//		cout<<"before print"<<endl;
		builder.build().println();
		// 性能更佳,效率最高,空间复杂度比直接拼串好 最推荐这种方式 StringBuilder 的这种方式拼串
//		(plain.builder()+8+9+'*'+"&&&&"+220.36 + "  ").build().println();
//		(plain.builder().build().println());
//		cout<<<<endl;
//		cout<<(plain + 8 + 9 + '*'+"&&&&"+220.36 + "  ").toChars()<<endl;
//		delete &builder;
//		system("pause");
	}

	{
		String splitTarget = "123,456,789";
		vector<String> result = splitTarget.split(',');
		for (int i = 0; i < result.size(); i++) {
			cout << "el[" << result[i].toChars() << "]" << endl;
		}
		String joinString = String::join(result, '*');
		cout << joinString.toChars() << " len: " << joinString.length() << endl;
		cout << joinString.indexOf('p') << endl;
		String js2 = result;
		js2.println();
//		system("pause");
	}
	{
//		int* table = getPrefixTable("aaca");

//		String test = "abaacababcac";
//		cout<<" substring: "<<test.indexOf("aac")<<endl;
//		getSuffixTable("fabaa");
		String tests = "fadaadaac  ";
		tests.trimSelf();
		cout<<"trim result: "
		<<tests.toChars()
		<<"______"
		<<tests.length()
		<<endl;
//		tests.replaceFirst("aa", "", true);
//		cout << "replaced result: "
//		<< tests.toChars()
//		<< " "
//		<< tests.length()
//		<< " "
//		<< tests.toChars()
//		<<endl;
//		cout<< "replaced result: "<<tests.indexOfthe("aa",1);
//		tests.replaceFirst("aa").println();
		tests.replacer()
		.replaceIndex(1)
		.search("aa")
		.merge(true)
		.replace()
		.println();
		cout<<"original: "<<tests.toChars()<<" upper:"<<tests.toUpperCase().toChars()<<endl;
//		tests.reverse(true).println();
//		cout<<" substring last index of: "<<tests.lastIndexOf("aa");
//		cout<<" substring      index of: "<<tests.indexOf("aa");
//		system("pause");
	}
	system("pause");
	return 0;
}

int mainv() {
//	int res = test();

//	cout<<" program state: "<<res<<endl;

	return 0;
}
