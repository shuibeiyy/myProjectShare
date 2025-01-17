#include<iostream>
#include<thread>
#include<mutex>//��
#include<atomic>//ԭ��
#include"CELLTimestamp.hpp"
using namespace std;
//ԭ�Ӳ���   ԭ�� ���� 
mutex m;
const int tCount = 4;
atomic_int sum = 0;        //原子操作，保证多线程操作中，多个线程对该变量的操作不会出现竞争，，原子操作是不可中断的操作，要么完全执行，要么完全不执行，不存在中间状态
void workFun(int index)
{
	for (int n = 0; n < 20000000; n++)
	{
		//�Խ���
		//lock_guard<mutex> lg(m);
		//�ٽ�����-��ʼ
		//m.lock();
		sum++;
		//m.unlock();
		//�ٽ�����-����
	}//�̰߳�ȫ �̲߳���ȫ
	 //ԭ�Ӳ��� �������������ʱ��С�Ĳ�����λ
	 //cout << index << "Hello,main thread." << n << endl;
}//��ռʽ

int main()
{
	thread t[tCount];
	for (int n = 0; n < tCount; n++)
	{
		t[n] = thread(workFun, n);
	}
	CELLTimestamp tTime;
	for (int n = 0; n < tCount; n++)
	{
		t[n].join();
		//t[n].detach();
	}
	cout << tTime.getElapsedTimeInMilliSec() << ",sum=" << sum << endl;
	sum = 0;
	tTime.update();
	for (int n = 0; n < 80000000; n++)
	{
		sum++;
	}
	cout << tTime.getElapsedTimeInMilliSec() << ",sum=" << sum << endl;
	cout << "Hello,main thread." << endl;
	return 0;
}