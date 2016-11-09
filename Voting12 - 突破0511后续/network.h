#ifndef NETWORK_H
#define NETWORK_H

#include "common.h"
#include "graph.h"
#include "res.h"
#include "LP_Voting.h"
#include <ilcplex/ilocplex.h>
#include "player.h"

class Network_LB: public Player
{
public:
	//vector<vector<double> > adj;//����ά�����ڽӾ��󣬼�¼����
	//vector<Path*> path_record;//���巽����·����¼
	//vector<double> judge;//���������з���������

	Network_LB(int id2, int id_kind2):Player(id2, id_kind2)
	{
		;
	}

	void modify(int a, int b, int c)
	{
		path_record.clear();
		judge.clear();
		path_record.resize(Maxreq);
		judge.resize(Maxreq);
	}

	//�������·�ɷ���
	void propose(VGraph &g)
	{
		//���´�����Ϊ�˺�flow�ĳ���ͳһ
		vector<Req*> reqPL;
		for(int i=0;i<g.reqL.size();i++)
		{	
			reqPL.push_back(g.reqL[i]);
		}

		LP_networkLB(&g, reqPL);
	}
	
	
	double LP_networkLB(VGraph *g,vector<Req*> &reqL)
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
			model.add((load + adj[g->incL[i]->src][g->incL[i]->dst]) <= z * g->incL[i]->capacity);
		}
		model.add(IloMinimize(environment, z));

		//Լ��1������Լ������֤��һ����
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

		//Լ��2������Լ������֤�����Ĵ�С��������·������
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

			for(int d=0;d<K;d++)
			{	
				Path* path=new Path();
				for(int i=0;i<g->m;i++)
				{
					if(solver.getValue(x[d][i])>0.5)
					{
						path->pathL.push_back(g->incL[i]);
						adj[g->incL[i]->src][g->incL[i]->dst] += reqL[d]->flow;//��¼����
					}
				}
				path_record[reqL[d]->id]=path;//��¼·��
			}
		}
		else
			cout<<"Error: Unfeasible solve"<<endl;

		environment.end();
		return obj;
	}

	//������������������·������
	void evaluate(VGraph &g, vector<Player*> &candiL)
	{
		double temp=0;//��¼ĳ�������������·������
		for(int i=0;i<candiL.size();i++)
		{
			if(i>Maxreq && candiL[i]->te==-1)  
			{
				 judge[i]=1;
				 continue;
			}
			temp=0;
			for(int j=0;j<g.incL.size();j++)
			{
				int src=g.incL[j]->src;
				int dst=g.incL[j]->dst;
				if(temp < candiL[i]->adj[src][dst]/g.incL[j]->capacity) 
					temp = candiL[i]->adj[src][dst]/g.incL[j]->capacity;
			}
			judge[i]=temp;
			if(judge[i]==0) judge[i]=1;//û��·�����԰��ţ���Ҫ���ӳͷ�
		}
	}

	//���� ��ʤ�ķ���
	void end_implement(VGraph &g,int winner, vector<Player*> &candiL)
	{
		//Flow��¼��ͼ������Ϣ����
		if(id==winner) return;
		for(int j=0;j<g.incL.size();j++)
		{
			int src,dst;
			src=g.incL[j]->src;
			dst=g.incL[j]->dst;
			adj[src][dst] = candiL[winner]->adj[src][dst];
		}
	}

};


#endif