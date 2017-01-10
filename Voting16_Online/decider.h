#ifndef DECIDER_H
#define DECIDER_H

#include "common.h"
#include "graph.h"
#include "ext.h"

class Decider
{
public:
	int app_id;//APP的编号
	vector<vector<double> > adjMyFlow;//记录属于自己APP的链路上负载
	vector<int> pathRecord;//APP提出的具体方案：路径记录,由于是在线问题，所以就一条路径
	vector<double> judge;//APP对所有方案的评价

	//初始化，只初始化一次，之后其他需求来的时候，
	//只修改之前参数，相当于投票的基础设施只建立一次，剩下的是维护
	Decider(int app_id2, VGraph &gv2)
	{
		app_id = app_id2;
		pathRecord.clear();
		judge.clear();
		adjMyFlow.resize(N);
		for (int i = 0; i < N; i++)
			adjMyFlow[i].resize(N);
		judge.resize(APPNUM);
	}

	//流的维护，针对新的流需求
	void init()
	{
		pathRecord.clear();
		judge.clear();
		judge.resize(APPNUM);
	}

	//流提出方案
	virtual void propose(Req &req) = 0;
	
	//应用评价所有方案
	virtual void evaluate(Req &req, vector<Decider*> &appL) = 0;
	
};//Decider类的结束位置
#endif