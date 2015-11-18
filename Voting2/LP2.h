#ifndef LP2_H
#define LP2_H

#include "common.h"
#include "graph.h"
#include "res.h"
#include <ilcplex/ilocplex.h>


//���������reqL, reqL����һ����flow = 0

double LP2(VGraph *g,vector<Req*> &reqL,vector<Path*> &path_record, int id, vector<vector<int> > &adj)
{
	IloEnv environment;
	IloModel model(environment);
	IloCplex solver(model);
	
	int K;//һ��case���req����
	K=reqL.size();
	
	//����
	IloArray<IloIntVarArray> x(environment, K);
	for(int d=0;d<K;d++)
		x[d]=IloIntVarArray(environment,g->m,0,1);

	//�Ż�Ŀ��
	IloExpr goal(environment);
	IloExpr temp(environment);
	for(int d=0;d<K;d++)
	{
		for(int i=0;i<g->m;i++)		
			temp += x[d][i] * reqL[d]->flow * g->incL[i]->weight;
		goal += temp/g->cost_best[d];
	}
	model.add(IloMinimize(environment, goal));

	//Լ��1������Լ��
	for(int d=0;d < K;d++)
		for(int i=0;i < g->n;i++)
		{
			IloExpr constraint(environment);
			for(int k=0;k<g->adjL[i].size();k++)
				constraint += x[d][g->adjL[i][k]->id];
			for(int k=0;k<g->adjRL[i].size();k++)
				constraint -= x[d][g->adjRL[i][k]->id];
		
			if(i==reqL[d]->src && d!=id)
				model.add(constraint==1);
			else if(i==reqL[d]->dst && d!=id)
				model.add(constraint==-1);
			else
				model.add(constraint==0);
		}

	//Լ��2������Լ��
	for(int i=0;i<g->m;i++)
	{
		IloExpr constraint(environment);
		for(int d=0;d<K;d++)
			constraint += reqL[d]->flow * x[d][i];
		model.add(constraint <= (g->incL[i]->capacity - adj[g->incL[i]->src][g->incL[i]->dst]));
	}

	//����ģ��
	solver.setOut(environment.getNullStream());
	double obj=Inf;
	if(solver.solve())
	{
		obj=solver.getObjValue();

		//·����¼��������һ������dijk�㷨��¼���ˣ������·��Ϊ0�����Բ��������¼
		
		int distance=0;
		for(int d=0;d<K;d++)
		{
			distance=0;
			Path* path=new Path();
			for(int i=0;i<g->m;i++)
			{
				if(solver.getValue(x[d][i])>0)
				{
					path->pathL.push_back(g->incL[i]);
					distance += g->incL[i]->weight;
				}
			}
			if(distance==0)continue;
			path_record[d]=path;
		}
	}
	else
		cout<<"Error: Unfeasible solve"<<endl;

	environment.end();
	return obj;
}
#endif