#ifndef LP_H
#define LP_H

#include "common.h"
#include "graph.h"
#include "res.h"
#include <ilcplex/ilocplex.h>

//����������Ĺ滮,��Ϊ������x[d]=IloIntVarArray(environment,g->m,0,1);
//ʹ��x[d][i]��0,1����

double LP(PGraph *g,vector<Req*> &reqL)
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

		//չʾ�������ߵ�·���������޸ĸ���link��capacity
		
		
		for(int d=0;d<K;d++)
		{
			//cout<<"flow "<<d+1<<" : "<<endl;
			int temp_bw=Inf;
			for(int i=0;i<g->m;i++)
			{
				if(solver.getValue(x[d][i])>0)
				{
					g->adj[g->incL[i]->src][g->incL[i]->dst] += reqL[d]->flow;
				}
			}
		}
		for(int d=0;d<K;d++)
		{
			double temp_bw=Inf;
			for(int i=0;i<g->m;i++)
			{
				if(solver.getValue(x[d][i])>0)
				{
					int src=g->incL[i]->src, dst=g->incL[i]->dst;
					if(temp_bw > g->incL[i]->capacity - g->adj[src][dst])
						temp_bw = g->incL[i]->capacity - g->adj[src][dst];
				}
			}
			//cout<<"d"<<d<<" :"<<temp_bw<<" "<<reqL[d]->flow<<endl;
			g->cost_LP[d] = temp_bw * reqL[d]->flow;
		}
	}
	else
		cout<<"Error: Unfeasible solve"<<endl;

	environment.end();
	return obj;
}
#endif