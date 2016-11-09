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

	//double te; //对最大链路利用率的约束参数，如果为10%，则要求提出的方案TE结果小于10%

	Neutral(int id2, int id_kind2):Player(id2, id_kind2)
	{
		;
	}

	void modify(int a, int b, int c)
	{
		path_record.clear();
		judge.clear();
		path_record.resize(Maxreq);
		judge.resize(Maxreq+4);//neutral没有投票资格，但是为了统一写法，这里这样写
	}

	//网络提出路由方案
	void propose(VGraph &g)
	{
		//以下处理是为了和flow的程序统一
		vector<Req*> reqPL;
		for(int i=0;i<g.reqL.size();i++)
		{	
			reqPL.push_back(g.reqL[i]);
		}

		LP_Neutral(&g, reqPL);
	}
	
	//以所有流的满意度为目标，以TE为约束
	double LP_Neutral(VGraph *g,vector<Req*> &reqL)
	{
		cout<<"id = "<<id<<", te = "<<te<<endl;

		IloEnv environment;
		IloModel model(environment);
		IloCplex solver(model);
	
		int K;//一个case里的req数量
		K=reqL.size();
	
		//变量
		IloArray<IloIntVarArray> x(environment, K);
		for(int d=0;d<K;d++)
			x[d]=IloIntVarArray(environment,g->m,0,1);
	

		IloNumVarArray D(environment,g->m,0,Inf);

		//优化目标
		IloExpr goal(environment);
		IloExpr temp(environment);
		IloExprArray L(environment,g->m);

		//L[i]:第i条边在该次流量部署后的流量
		for(int i=0;i<g->m;i++)
			L[i]=IloExpr(environment);
		for(int i=0;i<g->m;i++)
		{
			L[i]+=adj[g->incL[i]->src][g->incL[i]->dst];
			for(int d=0;d<K;d++)
				L[i]+=x[d][i]*reqL[d]->flow;
		}

		//延时约束条件，分段线性      这里的需要求解的变量数约为10*400*4=16000  （流需求数量*边数*4个约束）
		for(int i=0;i<g->m;i++)
		{
			double c=g->incL[i]->capacity;
			model.add(D[i] >= L[i]);
			model.add(D[i] >= 3*L[i]-2*c/3);
			model.add(D[i] >= 10*L[i]-16*c/3);
			model.add(D[i] >= 70*L[i]-178*c/3);
		}

		//maximize happiness   这里的约束变量有10*400=4000
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

		//虽然这里计算temp的方法是估算，计算g->cost_best的方法是准确计算，计算方式是不同的，
		//数值上差距是比较大的，但是两者拥有相同的走势，只差一个倍数
		//所以相当于goal的值放大或者缩小了一个倍数，但是差距特点还是保持原样的


		//约束1，流量约束，保证是一条流    这里的约束变量有 10*400*2=8000
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

		//约束2，容量约束，保证流量的大小不超过链路的容量   这里的约束变量有400*10=4000
		for(int i=0;i<g->m;i++)
		{
			IloExpr constraint(environment);
			for(int d=0;d<K;d++)
				constraint += reqL[d]->flow * x[d][i];
			model.add(constraint <= (g->incL[i]->capacity - adj[g->incL[i]->src][g->incL[i]->dst]));
		}

		//约束3，TE约束   这里的约束变量有 400*10=4000
		for(int i=0;i<g->m;i++)
		{
			IloExpr load(environment);
			for(int d=0;d<K;d++)
				load += x[d][i] * reqL[d]->flow;
			model.add((load + adj[g->incL[i]->src][g->incL[i]->dst]) <= te * g->incL[i]->capacity);
		}

		//综上，总的约束中的约束变量有36000个

		//计算模型
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
						adj[g->incL[i]->src][g->incL[i]->dst] += reqL[d]->flow;//记录负载
					}
				}
				path_record[reqL[d]->id]=path;//记录路由
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

	//计算各个方案的最大链路利用率
	void evaluate(VGraph &g, vector<Player*> &PlayerL)
	{
		cout<<"Neutral don't have the right to vote!"<<endl;
	}

	//部署 获胜的方案
	void end_implement(VGraph &g,int winner, vector<Player*> &candiL)
	{
		//Flow记录的图负载信息更新
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