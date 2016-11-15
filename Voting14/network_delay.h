#ifndef network_delay_H
#define network_delay_H

#include "common.h"
#include "graph.h"
#include "res.h"
#include <ilcplex/ilocplex.h>

//����������Ĺ滮,��Ϊ������x[d]=IloIntVarArray(environment,g->m,0,1);
//ʹ��x[d][i]��0,1����

double network_delay(DelayNetworkGraph *g,vector<Req*> &reqL)
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
	IloNumVarArray D(environment,g->m,0,Inf);

	//�Ż�Ŀ��
	IloExpr goal(environment);
	IloExpr temp(environment);
	IloExprArray L(environment,g->m);

	//L[i]:��i�����ڸô���������������
	for(int i=0;i<g->m;i++)
		L[i]=IloExpr(environment);
	for(int i=0;i<g->m;i++)
	{
		L[i]+=g->adj[g->incL[i]->src][g->incL[i]->dst];
		for(int d=0;d<K;d++)
			L[i]+=x[d][i]*reqL[d]->flow;
	}

	//��ʱԼ���������ֶ�����
	for(int i=0;i<g->m;i++)
	{
		double c=g->incL[i]->capacity;
		model.add(D[i] >= L[i]);
		model.add(D[i] >= 3*L[i]-2*c/3);
		model.add(D[i] >= 10*L[i]-16*c/3);
		model.add(D[i] >= 70*L[i]-178*c/3);
	}

	//maximize happiness
	for(int d=0;d<K;d++)
	{
		for(int i=0;i<g->m;i++)		
		{
			int src=g->incL[i]->src, dst=g->incL[i]->dst;
			temp += x[d][i] * reqL[d]->flow * D[i];
		}
		goal += temp/g->cost_best[reqL[d]->id];
	}
	//��Ȼ�������temp�ķ����ǹ��㣬���Ǻ��Ŷ���ʱ��ʽӵ����ͬ������


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
		
		//�Ȳ���
		for(int d=0;d<K;d++)
		{
			for(int i=0;i<g->m;i++)
			{
				if(solver.getValue(x[d][i])>0.5)
				{
					//cout<<"from node "<<g->incL[i]->src<<" to node "<<g->incL[i]->dst<< " has flow "<<solver.getValue(x[d][i])*reqL[d]->flow<<endl;
					g->adj[g->incL[i]->src][g->incL[i]->dst] += reqL[d]->flow;
				}
			}
		}

		//�ټ�����ʱ
		double latency=0;
		for(int i=0;i<APPNUM;i++)
			g->cost_LP[i]=0;
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
			g->cost_LP[reqL[d]->app_id] += latency;
		}
	}
	else
		cout<<"Error: Unfeasible solve"<<endl;

	environment.end();
	return obj;
}
#endif