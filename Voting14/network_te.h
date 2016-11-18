#ifndef NETWORK_TE_H
#define NETWORK_TE_H

#include "common.h"
#include "graph.h"
#include "res.h"
#include <ilcplex/ilocplex.h>

//�����������Ĺ滮,��Ϊ������x[d]=IloIntVarArray(environment,g->m,0,1);
//ʹ��x[d][i]��0,1����

double network_te(TENetworkGraph *g,vector<Req*> &reqL)
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
	

	//�Ż�Ŀ�� ��С��->�����·������ 0<=z<=1
	IloNumVar z(environment,0,1);//���������z�ķ�Χ��z�Ϳ��ܳ���1
	for(int i=0;i<g->m;i++)
	{
		IloExpr load(environment);
		for(int d=0;d<K;d++)
			load += x[d][i] * reqL[d]->flow;
		model.add((load + g->adj[g->incL[i]->src][g->incL[i]->dst]) <= z * g->incL[i]->capacity);
	}
	model.add(IloMinimize(environment, z));

	//Լ��1������Լ��
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

	//Լ��2������Լ��
	for(int i=0;i<g->m;i++)
	{
		IloExpr constraint(environment);
		for(int d=0;d<K;d++)
			constraint += reqL[d]->flow * x[d][i];
		model.add(constraint <= (g->incL[i]->capacity - g->adj[g->incL[i]->src][g->incL[i]->dst]));
	}

	//����ģ��
	solver.setOut(environment.getNullStream());
	double obj=Inf;
	if(solver.solve())
	{
		obj=solver.getObjValue();

		//�Ȳ���
		for(int d=0;d<K;d++)
		{
			for(int i=0;i<g->m;i++)
			{
				if(solver.getValue(x[d][i])>0.5)
					g->adj[g->incL[i]->src][g->incL[i]->dst] += reqL[d]->flow;
			}
		}

		//�ټ�����ʱ
		double latency=0;
		for(int d=0;d<K;d++)
		{
			latency=0;
			for(int i=0;i<g->m;i++)
			{
				if(solver.getValue(x[d][i])>0.5){
					if(g->incL[i]->capacity - g->adj[g->incL[i]->src][g->incL[i]->dst]==0)
						latency += reqL[d]->flow/
							(Rinf+g->incL[i]->capacity - g->adj[g->incL[i]->src][g->incL[i]->dst]);
					else
						latency += reqL[d]->flow/
							(g->incL[i]->capacity - g->adj[g->incL[i]->src][g->incL[i]->dst]);
				}
			}
			g->cost_LP[d] = latency;
		}
	}
	else
		cout<<"Error: Unfeasible solve"<<endl;

	environment.end();
	return obj;
}
#endif