#ifndef LP_H
#define LP_H

#include "common.h"
#include "graph.h"
#include "res.h"
#include <ilcplex/ilocplex.h>

//不允许分流的规划,因为这里有x[d]=IloIntVarArray(environment,g->m,0,1);
//使得x[d][i]是0,1变量

double LP(PGraph *g,vector<Req*> &reqL)
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
	for(int i=0;i<g->m;i++)
		for(int d=0;d<K;d++)
			goal += x[d][i] * reqL[d]->flow * g->incL[i]->weight;
	model.add(IloMinimize(environment, goal));

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
		model.add(constraint<=g->incL[i]->capacity);
	}

	//计算模型
	solver.setOut(environment.getNullStream());
	double obj=Inf;
	if(solver.solve())
	{
		obj=solver.getObjValue();

		//展示流量所走的路径，并且修改各条link的capacity
		
		int distance=0;
		for(int d=0;d<K;d++)
		{
			//cout<<"flow "<<d+1<<" : "<<endl;
			distance=0;
			for(int i=0;i<g->m;i++)
			{
				if(solver.getValue(x[d][i])>0)
				{
					/*cout<<"from node "<<g->incL[i]->src<<" to node "<<
						g->incL[i]->dst<< " has flow "<<
						solver.getValue(x[d][i])*reqL[d]->flow<<endl;*/
					g->incL[i]->capacity = g->incL[i]->capacity - reqL[d]->flow;
					distance += g->incL[i]->weight;
				}
			}
			//cout<<distance<<endl;
			g->cost_LP[d]=distance * reqL[d]->flow;
		}
	}
	else
		cout<<"Error: Unfeasible solve"<<endl;

	environment.end();
	return obj;
}
#endif