#include "../SharedPtr.h"


class Student {
public:
	SharedPtr<int*> score;
	
	void display(){
		cout<<"Student: "<<(int*)this<< " score: [";
		for(int i=0;i<3;i++){
			cout<<score[i]<<" ";
		}
		cout<<"]"<<endl;
	}
	
	Student(int* score){
		this->score = SharedPtr<int*>(score,SharedPtr<int*>::arrayDellocator);
		display();
	}
	
//	Student(Student& stu):score(stu.score){
//		cout<<"copy Student"<<endl;
//		display();
//	}
	
	~Student(){
		cout<<"delete Student"<<endl;
	}
};

Student getStu(){
	Student s(new int[]{1,2,3});
	cout<<"s is created"<<endl;
	s = Student(new int[]{4,5,6});
	s = Student(new int[]{4,5,6});
	s = Student(new int[]{4,5,6});
	cout<<"after ="<<endl;
	return s;
}

Student callCopy(Student stu){
	cout<<"callCopy"<<endl;
	stu.display();
	return stu;
}

int main(){
	debug(true);
	callCopy(getStu());

	cout<<"exit"<<endl;
	return 0;
}
