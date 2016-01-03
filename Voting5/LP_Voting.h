#ifndef LP_Voting_H
#define LP_Voting_H

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
	IloExprArray L(environment, g->m);//L[i]:��i��link����������flow
	IloArray<IloIntVar> Y(environment,K);
	
	for(int d=0;d<K;d++)
		Y[d]=IloIntVar(environment);
	for(int i=0;i<g->m;i++)
		L[i] = IloExpr(environment);

	//Ŀ������������
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
		//goal+=Y[d]*reqL[d]->flow/(double)g->cost_best[reqL[d]->id];

	model.add(IloMaximize(environment,goal));

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
		model.add(constraint <= (g->incL[i]->capacity - adj[g->incL[i]->src][g->incL[i]->dst]));
	}

	//����ģ��
	solver.setOut(environment.getNullStream());
	double obj=Inf;
	if(solver.solve())
	{
		obj=solver.getObjValue();

		//·����¼
		
		double distance=0;
		double temp=0;
		for(int d=0;d<K;d++)
		{
			distance=0;
			Path* path=new Path();
			//cout<<"flow "<<d+1<<" : "<<endl;
			//cout<<"Y[d] is "<<solver.getValue(Y[d])<<endl;
			for(int i=0;i<g->m;i++)
			{
				//cout<<"Y[d]<= "<<(1-solver.getValue(x[d][i]))*Inf + (g->incL[i]->capacity - solver.getValue(L[i]) - adj[g->incL[i]->src][g->incL[i]->dst])<<endl;
				if(solver.getValue(x[d][i])>0)
				{
					//temp=(1-solver.getValue(x[d][i]))*Inf + (g->incL[i]->capacity - solver.getValue(L[i]) - adj[g->incL[i]->src][g->incL[i]->dst]);
					//cout<<"Y[d]<= "<<temp<<endl;
					//cout<<"L[i] is "<<solver.getValue(L[i])<<endl;
					path->pathL.push_back(g->incL[i]);
					distance += g->incL[i]->weight;
					//cout<<"from node "<<g->incL[i]->src<<" to node "<<
						//g->incL[i]->dst<< " has flow "<<
						//solver.getValue(x[d][i])*reqL[d]->flow<<endl;
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