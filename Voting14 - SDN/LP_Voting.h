#ifndef LP_VOTING_H
#define LP_VOTING_H

#include "common.h"
#include "graph.h"
#include "ext.h"
#include <ilcplex/ilocplex.h>

//����flow.h�У���voting�����Ĺ滮����
//����Ϊ�˱�������Թ滮����ǰcase������֮���Ӱ�첻���ǣ�Ҳ������ʱֻ��֮ǰcase�����й�

double LP_Voting(VGraph *g,vector<Req*> &reqL,vector<Path*> &path_record, int id, vector<vector<double> > &adj)
{
	IloEnv environment;
	IloModel model(environment);
	IloCplex solver(model);

	solver.setParam(IloCplex::Param::Barrier::QCPConvergeTol, 1e-10);

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

		//·����¼
		
		int distance=0;
		for(int d=0;d<K;d++)
		{
			//cout<<"flow "<<reqL[d]->id<<endl;
			distance=0;
			Path* path=new Path();
			for(int i=0;i<g->m;i++)
			{
				if(solver.getValue(x[d][i])>0.5)
				{
					//cout<<"from node "<<g->incL[i]->src<<" to node "<<g->incL[i]->dst<< " has flow "<<reqL[d]->flow<<" "<<solver.getValue(x[d][i])<<endl;
					path->pathL.push_back(g->incL[i]);
					distance += 1;
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