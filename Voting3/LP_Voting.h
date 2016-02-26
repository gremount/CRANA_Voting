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

	IloNumVarArray D(environment,g->m,0,Inf);

	//�Ż�Ŀ��
	IloExpr goal(environment);
	IloExpr temp(environment);
	IloExprArray L(environment,g->m);

	for(int i=0;i<g->m;i++)
	{
		L[i]+=adj[g->incL[i]->src][g->incL[i]->dst];
		for(int d=0;d<K;d++)
			L[i]+=x[d][i]*reqL[d]->flow;
	}

	//��ʱԼ���������ֶ�����
	for(int i=0;i<g->m;i++)
	{
		double c=g->incL[i]->capacity;
		model.add(D[i] >= 2*(c+1)*L[i]/(c*(c+2) + (c+1)/(c+2)));
		model.add(D[i] >= 20*(c-1)*L[i]/((c+2)*(c+10)) + (-8*c*c+32*c+20)/((c+2)*(c+10)));
		model.add(D[i] >= 10*(c+1)*L[i]/(c+10) + (-9*c*c+c+10)/(c+10));
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
		model.add(constraint <= (g->incL[i]->capacity - adj[g->incL[i]->src][g->incL[i]->dst]));
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