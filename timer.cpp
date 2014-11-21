/*************************************************************************
	> File Name: c++11/timer.cpp
	> Author: cjj
	> Created Time: 2014年10月28日 14:13:22
 ************************************************************************/
#include "timer.h"
#include <stdio.h>
#include <unistd.h>
Timer * timer = Timer::GetInstance();

void func(){
	int tick = time(NULL);
	printf("callback tick:%d\n", tick);
}

class A
{
public:
	void func1()
	{
		int tick = time(NULL);
		printf("A::callback tick:%d\n", tick);
	}
	
	void func2()
	{
		int tick = time(NULL);
		printf("A::callback func2:%d\n", tick);
	}
	uint32_t setInterval(boost::function<void()> func, int interval){
		return timer->SetInterval(func, interval);
	}
	
	void setTimeout(boost::function<void()> func, int timeOut){
		timer->SetTimeout(func, timeOut);
	}
};


int main()
{
	uint32_t timerId;
	A a;
	timerId = a.setInterval(boost::bind(&A::func1, a), 2);
	a.setTimeout(boost::bind(&A::func2, a), 5);	
	timer->SetInterval(boost::bind(func), 10);
	int cnt = 0;
	while(1){
		int tick = time(NULL);
		timer->Update(tick);		
		printf("tick:%d\n", tick);
		sleep(1);
		if(cnt == 10) {
			timer->ClearInterval(timerId);
		}
		cnt++;
	}
	return 0;
}