#ifndef RESOURCES_H
#define RESOURCES_H

#include"common.h"
#include "ext.h"
#include <ilcplex/ilocplex.h>

class Edge
{
public:
	int id;
	int src,dst;
	double capacity;//链路带宽
	double use;//sflow测得的当前带宽消耗
	double latency;//floodlight测得的latency
	Edge(){;}
	Edge(int a, int b, int c, double d, double e, double f){
		id=a;
		src=b;
		dst=c;
		capacity=d;
		use=e;
		latency=f;
	}
};

class Req
{
public:
	int id, app_id, src, dst;
	double flow;
	Req(int id2, int app_id2, int a, int b, double c)
	{
		id=id2;
		app_id=app_id2;
		src=a;
		dst=b;
		flow=c;
	}
};

class Path
{
public:
	vector<Edge*> pathL;
	Path(){;}
	~Path(){;}
};

class VGraph
{
public:
	int n,m;
	set<int> S, V;
    vector<int> p;
	vector<double> d;
	vector<Edge*> incL;//边的列表
	vector<vector<Edge*> > adjL,adjRL; //正向和反向邻接链表
	vector<vector<double> > adj;//记录负载

	vector<Req*> reqL;
	vector<double> cost_best;//记录每个APP的最佳部署结果
	//vector<double> cost_LP;//记录每个APP的LP部署结果

	VGraph(){;}
	VGraph(string address)
	{
		ifstream infile(address);
		infile>>n>>m;
		N=n;//给全局变量赋值
		M=m;//给全局变量赋值

		d.resize(n);
        p.resize(n);
		adjL.resize(n);//点的编号从0开始
		adjRL.resize(n);
		adj.resize(n);
		for(int i=0;i<n;i++)
			adj[i].resize(n);
		
		cost_best.resize(APPNUM);
		//cost_LP.resize(APPNUM);

		int a,b,c;
		double d,e,f;
		for(int i=0;i<m;i++)
		{
			infile>>a>>b>>c;
			infile>>d>>e>>f;
			
			Edge* ed=new Edge(a,b,c,d,e,f);
			
			incL.push_back(ed);
			adjL[b].push_back(ed);
			adjRL[c].push_back(ed);
		}
	}

	//将一个case的所有req都记录在图里，方便所有的Flow调用
	void req_modify(vector<Req*> &reqL2){
		reqL.clear();
		int req_num=0;
		req_num=reqL2.size();
		for(int i=0;i<req_num;i++)
			reqL.push_back(reqL2[i]);
	}

	void Update(int s,int flow,vector<vector<double> > &adj){
        double x;
		for (int i = 0; i < adjL[s].size();i++){
			x=adj[adjL[s][i]->src][adjL[s][i]->dst]+flow;
			if(x > adjL[s][i]->capacity)continue;
			if (d[s] + flow/(adjL[s][i]->capacity - x + 1) < d[adjL[s][i]->dst]){
                d[adjL[s][i]->dst] = d[s] + flow/(adjL[s][i]->capacity - x + 1);
                p[adjL[s][i]->dst] = s;
            }
		}
    }

    int FindMin(){
        set<int>::iterator it, iend;
        iend = S.end();
        double mine = Inf;
        int min_node = -1;
        for (it = S.begin(); it != iend; it++){
            if(d[*it] < mine) {
                mine = d[*it];
                min_node = *it;
            }
        }
        return min_node;
    }

    double dijkstra(int src, int dst, int flow, vector<vector<double> > &adj){
        S.clear();
        V.clear();
        for (int i = 0; i < n; i++)
        {
            S.insert(i);
            d[i] = Inf;
            p[i] = -2;
        }
        d[src] = 0; p[src] = -1;
        Update(src,flow,adj);
        S.erase(src);
        V.insert(src);
        while (S.size() != 0)
        {
            int mind;
            mind = FindMin();
            if (mind == dst) return d[mind];
			if (mind==-1) break;//没有路可达
            Update(mind,flow,adj);
            S.erase(mind);
            V.insert(mind);
        }
		return Inf;//没有路可达
    }

	//线性拟合 1/(c-x)
	double linearCal(double load, double capacity)
	{
		double util=load/capacity;
		if(util<0.3333) return load/capacity;
		else if(util<0.6667) return 3*load/capacity - 2.0/3.0;
		else if(util<0.9) return 10*load/capacity - 16.0/3.0;
		else return 70*load/capacity - 178.0/3.0;
	}

	double network_delay_lp(vector<Req*> &reqL,int app_id)
	{
		IloEnv environment;
		IloModel model(environment);
		IloCplex solver(model);
	
		int K;//一个case里的req数量
		K=reqL.size();
	
		//变量
		IloArray<IloIntVarArray> x(environment, K);
		for(int d=0;d<K;d++)
			x[d]=IloIntVarArray(environment,m,0,1);
	
		//优化目标
		IloExpr goal(environment);
		IloNumVarArray cost(environment,m,0.0,Inf);
		
		//约束+目标
		for(int i=0;i<m;i++)
		{
			IloNumExpr load(environment);
			for(int d=0;d<K;d++)
				load+=x[d][i]*reqL[d]->flow;
			double capacity=incL[i]->capacity;
		model.add(cost[i]>=load/(capacity));   // [0,1/3]
		model.add(cost[i]>=3.0*load/(capacity)-(2.0/3.0));  //[1/3,2/3]
		model.add(cost[i]>=10.0*load/(capacity)-(16.0/3.0)); // [2/3,9/10]
		model.add(cost[i]>=70.0*load/(capacity)-(178.0/3.0));  //[9/10,1]s
		for(int d=0;d<K;d++)
			goal += cost[i]*x[d][i]*reqL[d]->flow;	
		}
		model.add(IloMinimize(environment, goal));

		//约束1，流量约束
		for(int d=0;d < K;d++)
			for(int i=0;i < n;i++)
			{
				IloExpr constraint(environment);
				for(int k=0;k<adjL[i].size();k++)
					constraint += x[d][adjL[i][k]->id];
				for(int k=0;k<adjRL[i].size();k++)
					constraint -= x[d][adjRL[i][k]->id];
		
				if(i==reqL[d]->src)
					model.add(constraint==1);
				else if(i==reqL[d]->dst)
					model.add(constraint==-1);
				else
					model.add(constraint==0);
			}

		//约束2，容量约束
		for(int i=0;i<m;i++)
		{
			IloExpr constraint(environment);
			for(int d=0;d<K;d++)
				constraint += reqL[d]->flow * x[d][i];
			model.add(constraint <= incL[i]->capacity);
		}

		//计算模型
		solver.setOut(environment.getNullStream());
		double obj=Inf;
		if(solver.solve())
		{
			obj=solver.getObjValue();

			//展示流量所走的路径，并且修改各条link的capacity
		
			//先部署，方便计算延时
			for(int d=0;d<K;d++)
			{
				for(int i=0;i<m;i++)
				{
					if(solver.getValue(x[d][i])>0.5)
					{
						//cout<<"from node "<<incL[i]->src<<" to node "<<incL[i]->dst<< " has flow "<<solver.getValue(x[d][i])*reqL[d]->flow<<endl;
						adj[incL[i]->src][incL[i]->dst] += reqL[d]->flow;
					}
				}
			}

			//再计算延时
			double latency=0;
			cost_best[app_id]=0;
			for(int d=0;d<K;d++)
			{
				latency=0;
				for(int i=0;i<m;i++)
				{
					if(solver.getValue(x[d][i])>0.5)
						latency+=reqL[d]->flow*linearCal(adj[incL[i]->src][incL[i]->dst],incL[i]->capacity);
				}
				cost_best[app_id] += latency;
			}

			//再撤销，方便下一个应用计算最优方案
			for(int d=0;d<K;d++)
			{
				for(int i=0;i<m;i++)
				{
					if(solver.getValue(x[d][i])>0.5)
					{
						adj[incL[i]->src][incL[i]->dst] -= reqL[d]->flow;
					}
				}
			}
		}


		else
			cout<<"Error: Unfeasible solve"<<endl;

		environment.end();
		return obj;
	}

};

#endif