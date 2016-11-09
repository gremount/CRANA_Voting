#ifndef NEUTRAL_H
#define NEUTRAL_H

#include "common.h"
#include "graph.h"
#include "res.h"
#include "LP_Voting.h"
#include <ilcplex/ilocplex.h>
#include "player.h"

class Neutral: public Player
{
public:

	//double te; //�������·�����ʵ�Լ�����������Ϊ10%����Ҫ������ķ���TE���С��10%

	Neutral(int id2, int id_kind2):Player(id2, id_kind2)
	{
		;
	}

	void modify(int a, int b, int c)
	{
		path_record.clear();
		judge.clear();
		path_record.resize(Maxreq);
		judge.resize(Maxreq+4);//neutralû��ͶƱ�ʸ񣬵���Ϊ��ͳһд������������д
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

		LP_Neutral(&g, reqPL);
	}
	
	//���������������ΪĿ�꣬��TEΪԼ��
	double LP_Neutral(VGraph *g,vector<Req*> &reqL)
	{
		cout<<"id = "<<id<<", te = "<<te<<endl;

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
			L[i]+=adj[g->incL[i]->src][g->incL[i]->dst];
			for(int d=0;d<K;d++)
				L[i]+=x[d][i]*reqL[d]->flow;
		}

		//��ʱԼ���������ֶ�����      �������Ҫ���ı�����ԼΪ10*400*4=16000  ������������*����*4��Լ����
		for(int i=0;i<g->m;i++)
		{
			double c=g->incL[i]->capacity;
			model.add(D[i] >= L[i]);
			model.add(D[i] >= 3*L[i]-2*c/3);
			model.add(D[i] >= 10*L[i]-16*c/3);
			model.add(D[i] >= 70*L[i]-178*c/3);
		}

		//maximize happiness   �����Լ��������10*400=4000
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

		//��Ȼ�������temp�ķ����ǹ��㣬����g->cost_best�ķ�����׼ȷ���㣬���㷽ʽ�ǲ�ͬ�ģ�
		//��ֵ�ϲ���ǱȽϴ�ģ���������ӵ����ͬ�����ƣ�ֻ��һ������
		//�����൱��goal��ֵ�Ŵ������С��һ�����������ǲ���ص㻹�Ǳ���ԭ����


		//Լ��1������Լ������֤��һ����    �����Լ�������� 10*400*2=8000
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

		//Լ��2������Լ������֤�����Ĵ�С��������·������   �����Լ��������400*10=4000
		for(int i=0;i<g->m;i++)
		{
			IloExpr constraint(environment);
			for(int d=0;d<K;d++)
				constraint += reqL[d]->flow * x[d][i];
			model.add(constraint <= (g->incL[i]->capacity - adj[g->incL[i]->src][g->incL[i]->dst]));
		}

		//Լ��3��TEԼ��   �����Լ�������� 400*10=4000
		for(int i=0;i<g->m;i++)
		{
			IloExpr load(environment);
			for(int d=0;d<K;d++)
				load += x[d][i] * reqL[d]->flow;
			model.add((load + adj[g->incL[i]->src][g->incL[i]->dst]) <= te * g->incL[i]->capacity);
		}

		//���ϣ��ܵ�Լ���е�Լ��������36000��

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
		{
			te=-1;
			cout<<"Error: Unfeasible solve"<<endl;
		}
		environment.end();
		return obj;
	}

	//������������������·������
	void evaluate(VGraph &g, vector<Player*> &PlayerL)
	{
		cout<<"Neutral don't have the right to vote!"<<endl;
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