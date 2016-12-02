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

	//�Ż�Ŀ��
	IloExpr goal(environment);
	IloNumVarArray cost(environment,g->m,0.0,Inf);
	
	//Լ��+Ŀ��
	//��������ʱ�Ĺ滮ģ�ͣ���������"Internet Traffic Engineering by Optimizing OSPF Weights"
	for(int i=0;i<g->m;i++)
	{
		IloNumExpr load(environment);
		for(int d=0;d<K;d++)
			load+=x[d][i]*reqL[d]->flow;
		double capacity=g->incL[i]->capacity;
		model.add(cost[i]>=load/(capacity));   // [0,1/3]
		model.add(cost[i]>=3.0*load/(capacity)-(2.0/3.0));  //[1/3,2/3]
		model.add(cost[i]>=10.0*load/(capacity)-(16.0/3.0)); // [2/3,9/10]
		model.add(cost[i]>=70.0*load/(capacity)-(178.0/3.0));  //[9/10,1]
		for(int d=0;d<K;d++)
			goal += cost[i]*x[d][i]*reqL[d]->flow/g->cost_best[reqL[d]->app_id];
		//goal += cost[i];
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
		model.add(constraint <= g->incL[i]->capacity);
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
			//cout<<"flow "<<d<<endl;
			for(int i=0;i<g->m;i++)
			{
				if(solver.getValue(x[d][i])>0.5)
				{
					//cout<<"from node "<<g->incL[i]->src<<" to node "<<g->incL[i]->dst<< " has flow "<<reqL[d]->flow<<" "<<solver.getValue(x[d][i])<<endl;
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
				if(solver.getValue(x[d][i])>0.5)
					latency+=reqL[d]->flow*linearCal(g->adj[g->incL[i]->src][g->incL[i]->dst],g->incL[i]->capacity);
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