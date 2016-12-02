#ifndef COMMON_H
#define COMMON_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<algorithm>
#include<vector>
#include<queue>
#include<list>
#include<set>
#include<map>
#include<string>
#include<iostream>
#include<fstream>
using namespace std;

#define KC 100
#define M2C 100 //the number of candidates ���ýϴ��ֵ���ܷ�ֹ����Խ�缴��
#define N2C 100 //how many kinds of ranking ���ýϴ��ֵ���ܷ�ֹ����Խ�缴��

//������� 1/(c-x)
double linearCal(double load, double capacity)
{
	double util=load/capacity;
	if(util<0.3333) return load/capacity;
	else if(util<0.6667) return 3*load/capacity - 2.0/3.0;
	else if(util<0.9) return 10*load/capacity - 16.0/3.0;
	else return 70*load/capacity - 178.0/3.0;
}
#endif