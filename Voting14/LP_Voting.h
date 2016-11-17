#ifndef LP_VOTING_H
#define LP_VOTING_H

#include "common.h"
#include "graph.h"
#include "res.h"
#include <ilcplex/ilocplex.h>

//����flow.h�У���voting�����Ĺ滮����
//����Ϊ�˱�������Թ滮����ǰcase������֮���Ӱ�첻���ǣ�Ҳ������ʱֻ��֮ǰcase�����й�

double LP_Voting(VGraph *g,vector<Req*> &reqL,vector<Path*> &path_record, int id, vector<vector<double> > &adj)
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
	for(int i=0;i<g->m;i++)
	{
		IloNumExpr load(environment);
		for(int d=0;d<K;d++)
			load+=x[d][i]*reqL[d]->flow;
		double c=g->incL[i]->capacity;
		model.add(cost[i] >= load);
		model.add(cost[i] >= 3*load-2*c/3);
		model.add(cost[i] >= 10*load-16*c/3);
		model.add(cost[i] >= 70*load-178*c/3);
		goal += cost[i];
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

		//·����¼
		
		int distance=0;
		for(int d=0;d<K;d++)
		{
			distance=0;
			Path* path=new Path();
			for(int i=0;i<g->m;i++)
			{
				if(solver.getValue(x[d][i])>0.5)
				{
					//cout<<"from node "<<g->incL[i]->src<<" to node "<<g->incL[i]->dst<< " has flow "<<solver.getValue(x[d][i])*reqL[d]->flow<<endl;
					path->pathL.push_back(g->incL[i]);
					distance += g->incL[i]->weight;
				}
			}
			if(distance==0){cout<<endl<<endl<<"error !!!!!!!!!!!!!!!!!"<<endl<<endl;continue;}
			path_record[reqL[d]->id]=path;
		}
	}
	else
		cout<<"Error: Unfeasible solve"<<endl;

	environment.end();
	return obj;
}
#endif