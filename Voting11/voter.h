#ifndef VOTER_H
#define VOTER_H

#include "common.h"
#include "graph.h"
#include "res.h"
#include "LP_Voting.h"
#include <ilcplex/ilocplex.h>

//这里的Voter实际上是决策实体，可能为候选者，也可能为投票者
class Voter
{
public:
	int id;//voter num
    int id_kind;//0 is flow voter; 1 is network voter; 2 is neutral voter
	double te; //为neutral记录可以允许的最大链路利用率

    vector<vector<double> > adj;
    vector<Path*> path_record;
    vector<double> judge;
	
	Voter(int id2, int id_kind2)
	{
		id=id2;
		id_kind=id_kind2;

		adj.clear();
		path_record.clear();
		judge.clear();

		adj.resize(N);
		for(int i=0;i<N;i++)
			adj[i].resize(N);
		path_record.resize(Maxreq);
		judge.resize(Maxreq+4);//+1 network+3 neutral
	}
	
	//修改投票者的信息
	virtual void modify(int a, int b, int c)=0;

	//提出方案
    virtual void propose(VGraph &g)=0;
    
	//评价方案
	virtual void evaluate(VGraph &g, vector<Voter*> &flowL)=0;
    
	//最终部署方案
	virtual void end_implement(VGraph &g,int winner, vector<Voter*> &candiL)=0;

};


#endif