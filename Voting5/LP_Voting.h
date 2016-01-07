#ifndef LP_Voting_H
#define LP_Voting_H

#include "common.h"
#include "graph.h"
#include "res.h"
#include <ilcplex/ilocplex.h>

//用在flow.h中，是voting方案的规划部分
//这里为了避免非线性规划，当前case流互相之间的影响不考虑，也就是延时只和之前case的流有关

double LP_Voting(VGraph *g,vector<Req*> &reqL,vector<Path*> &path_record, int id, vector<vector<int> > &adj)
{
	IloEnv environment;
	IloModel model(environment);
	IloCplex solver(model);
	
	int K;//一个case里的req数量
	K=reqL.size();
	
	//变量
	IloArray<IloIntVarArray> x(environment, K);

	for(int d=0;d<K;d++)
		x[d]=IloIntVarArray(environment,g->m,0,1);

	//优化目标
	IloExpr goal(environment);
	IloExprArray L(environment, g->m);//L[i]:第i条link上所流经的flow
	IloIntVarArray Y(environment,K,0,Inf);

	for(int i=0;i<g->m;i++)
		L[i] = IloExpr(environment);

	//目标是最大化满意度
	for(int i=0;i<g->m;i++)
	{
		for(int d=0;d<K;d++)
			L[i]+=x[d][i]*reqL[d]->flow;
	}

	for(int d=0;d<K;d++)
		for(int i=0;i<g->m;i++)
			model.add(Y[d] <= ((1-x[d][i])*Inf + (g->incL[i]->capacity - L[i] - adj[g->incL[i]->src][g->incL[i]->dst])));

	for(int d=0;d<K;d++)
		goal+=Y[d]*reqL[d]->flow;
		//goal+=Y[d]*reqL[d]->flow/(int)g->cost_best[reqL[d]->id];

	model.add(IloMaximize(environment,goal));

	//约束1，流量约束
	for(int d=0;d < K;d++)
		for(int i=0;i < g->n;i++)
		{
			IloExpr constraint(environment);
			for(int k=0;k<g->adjL[i].size();k++)
				constraint += x[d][g->adjL[i][k]->id];
			for(int k=0;k<g->adjRL[i].size();k++)
				constraint -= x[d][g->adjRL[i][k]->id];
		
			if(i==reqL[d]->src)
				model.add(constraint==1);
			else if(i==reqL[d]->dst)
				model.add(constraint==-1);
			else
				model.add(constraint==0);
		}
	
	//约束2，容量约束
	for(int i=0;i<g->m;i++)
	{
		IloExpr constraint(environment);
		for(int d=0;d<K;d++)
			constraint += reqL[d]->flow * x[d][i];
		model.add(constraint <= (g->incL[i]->capacity - adj[g->incL[i]->src][g->incL[i]->dst]));
	}

	//计算模型
	solver.setOut(environment.getNullStream());
	double obj=Inf;
	if(solver.solve())
	{
		obj=solver.getObjValue();

		//路径记录
		
		int distance=0;
		int temp=0;
		for(int d=0;d<K;d++)
		{
			distance=0;
			Path* path=new Path();
			
			for(int i=0;i<g->m;i++)
			{
				if(solver.getValue(x[d][i])!=0 || solver.getValue(x[d][i])!=1)
					cout<<"!!!!!!!!!!!!!!!!!!!  x[d][i] error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<d<<" "<<i<<" "<<solver.getValue(x[d][i])<<endl;
				if(solver.getValue(x[d][i])>0.5)
				{
					path->pathL.push_back(g->incL[i]);
					distance += g->incL[i]->weight;
				}
			}
			path_record[reqL[d]->id]=path;
			if(distance==0){cout<<endl<<endl<<"error !!!!!!!!!!!!!!!!!"<<endl<<endl;continue;}
		}
	}
	else
		cout<<"Error: Unfeasible solve"<<endl;

	environment.end();
	return obj;
}
#endif