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
#define M2C 100 //the number of candidates
#define N2C 100 //how many kinds of ranking

double linearCal(double x, double c)
{
	double util=x/c;
	if(util<0.3333) return x*1;
	else if(util<0.6667) return x*3;
	else if(util<0.9) return x*10;
	else return x*70;
}

#endif