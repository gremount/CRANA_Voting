#ifndef APP_H
#define APP_H

#include "common.h"
#include "graph.h"
#include "ext.h"
#include <ilcplex/ilocplex.h>

class APP
{
public:
	int app_id;//APP�ı��
	vector<vector<double> > adj;//��APPά�����ڽӾ��󣬼�¼����
	vector<Path*> path_record;//APP����ľ��巽����·����¼
	vector<double> judge;//APP�����з���������

	//��ʼ����ֻ��ʼ��һ�Σ�֮��������������ʱ��
	//ֻ�޸�֮ǰ�������൱��ͶƱ�Ļ�����ʩֻ����һ�Σ�ʣ�µ���ά��
	APP(int app_id2)
	{
		app_id=app_id2;
		adj.clear();
		path_record.clear();
		judge.clear();
		adj.resize(N);
		for(int i=0;i<N;i++)
			adj[i].resize(N);
		path_record.resize(REQNUM);
		judge.resize(REQNUM);
	}

	//����ά��������µ�������
	void init()
	{
		path_record.clear();
		judge.clear();
		path_record.resize(REQNUM);
		judge.resize(REQNUM);
	}

	//���������
	void propose(VGraph &g,vector<APP*> &appL)
	{	
		vector<Req*> reqPL_app;//���Լ������ù滮����·��
		vector<Req*> reqPL_other;//�����������ù滮����·��
		if(app_id==0)
		{
			for(int i=0;i<g.reqL.size();i++)
				reqPL_other.push_back(g.reqL[i]);
			voting_lp(g,reqPL_other);
			begin_implement(g);
		}
		else
		{
			for(int i=0;i<g.reqL.size();i++)
			{	
				if(app_id==g.reqL[i]->app_id) reqPL_app.push_back(g.reqL[i]);
				else	  reqPL_other.push_back(g.reqL[i]);
			}
			voting_lp(g,reqPL_app);
			voting_lp(g,reqPL_other);
			begin_implement(g);
		}
	}

	//������ �Լ�����ķ���
	void begin_implement(VGraph &g)
	{
		for(int i=0;i<REQNUM;i++)
		{
			int edge_num=path_record[i]->pathL.size();
			for(int j=0;j<edge_num;j++)
			{
				int src,dst;
				src=path_record[i]->pathL[j]->src;
				dst=path_record[i]->pathL[j]->dst;
				adj[src][dst] += g.reqL[i]->flow;
			}
		}
	}
	
	//Ӧ���������з���
	void evaluate(VGraph &g, vector<APP*> &appL)
	{
		//��������APP����ķ���
		int edge_num=0;//temp��¼·��Ȩֵ��
		double temp;
		for(int k=0;k<APPNUM;k++)
		{
			judge[k]=0;
			for(int i=0;i<REQNUM;i++)
			{
				if(g.reqL[i]->app_id!=app_id){
					//judge[k]++;
					continue;
				}//�����������Լ����Ͳ��ܸ���������
				edge_num=appL[k]->path_record[i]->pathL.size();
				temp=0;
				for(int j=0;j<edge_num;j++)
				{
					int src=appL[k]->path_record[i]->pathL[j]->src;
					int dst=appL[k]->path_record[i]->pathL[j]->dst;
					int capacity=appL[k]->path_record[i]->pathL[j]->capacity;
					temp+=g.reqL[i]->flow*linearCal(appL[k]->adj[src][dst],capacity);
				}
				judge[k]+=temp;
			}
		}
	}

	//Ӧ�ò��� ��ʤ�ķ���
	void end_implement(VGraph &g,vector<APP*> &appL)
	{
		//APP��¼��ͼ������Ϣ����
		for(int k=0;k<APPNUM;k++)
		{
			if(k==app_id)continue;
			for(int i=0;i<g.incL.size();i++)
			{
				int src=g.incL[i]->src;
				int dst=g.incL[i]->dst;
				appL[k]->adj[src][dst]=appL[app_id]->adj[src][dst];
			}
		}
	}

	//������� 1/(c-x)
	double linearCal(double load, double capacity)
	{
		double util=load/capacity;
		if(util<0.3333) return load/capacity;
		else if(util<0.6667) return 3*load/capacity - 2.0/3.0;
		else if(util<0.9) return 10*load/capacity - 16.0/3.0;
		else return 70*load/capacity - 178.0/3.0;
	}

	double voting_lp(VGraph &g,vector<Req*> &reqL)
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
			x[d]=IloIntVarArray(environment,g.m,0,1);

		//�Ż�Ŀ��
		IloExpr goal(environment);
		IloNumVarArray cost(environment,g.m,0.0,Inf);
	
		//Լ��+Ŀ��
		for(int i=0;i<g.m;i++)
		{
			IloNumExpr load(environment);
			for(int d=0;d<K;d++)
				load+=x[d][i]*reqL[d]->flow;
			double capacity=g.incL[i]->capacity;

			//�Լ���ϵ�
			model.add(cost[i]>=load/(capacity));   // [0,1/3]
			model.add(cost[i]>=3.0*load/(capacity)-(2.0/3.0));  //[1/3,2/3]
			model.add(cost[i]>=10.0*load/(capacity)-(16.0/3.0)); // [2/3,9/10]
			model.add(cost[i]>=70.0*load/(capacity)-(178.0/3.0));  //[9/10,1]
			
			
			/*
			//OSPF Weights
			model.add(cost[i] >= load);
			model.add(cost[i] >= 3*load-2*capacity/3);
			model.add(cost[i] >= 10*load-16*capacity/3);
			model.add(cost[i] >= 70*load-178*capacity/3);
			*/
			for(int d=0;d<K;d++)
				goal += cost[i]*x[d][i]*reqL[d]->flow/g.cost_best[reqL[d]->app_id];
				//goal += cost[i];
		}
		model.add(IloMinimize(environment, goal));

		//Լ��1������Լ��
		for(int d=0;d < K;d++)
			for(int i=0;i < g.n;i++)
			{
				IloExpr constraint(environment);
				for(int k=0;k<g.adjL[i].size();k++)
					constraint += x[d][g.adjL[i][k]->id];
				for(int k=0;k<g.adjRL[i].size();k++)
					constraint -= x[d][g.adjRL[i][k]->id];
		
				if(i==reqL[d]->src)
					model.add(constraint==1);
				else if(i==reqL[d]->dst)
					model.add(constraint==-1);
				else
					model.add(constraint==0);
			}

		//Լ��2������Լ��
		for(int i=0;i<g.m;i++)
		{
			IloExpr constraint(environment);
			for(int d=0;d<K;d++)
				constraint += reqL[d]->flow * x[d][i];
			model.add(constraint <= g.incL[i]->capacity);
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
				for(int i=0;i<g.m;i++)
				{
					if(solver.getValue(x[d][i])>0.5)
					{
						//cout<<"from node "<<g->incL[i]->src<<" to node "<<g->incL[i]->dst<< " has flow "<<reqL[d]->flow<<" "<<solver.getValue(x[d][i])<<endl;
						path->pathL.push_back(g.incL[i]);
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

};//APP��Ľ���λ��
#endif