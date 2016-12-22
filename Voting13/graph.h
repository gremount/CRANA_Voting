#ifndef RESOURCES_H
#define RESOURCES_H

#include"common.h"
#include "res.h"
#include <ilcplex/ilocplex.h>
class Edge
{
public:
	int id;
	int src,dst;
	int weight,capacity;
	Edge(){;}
	Edge(int id2, int a, int b, int c, int d){
	id=id2;src=a;dst=b;weight=c;capacity=d;
	}
};

class Req
{
public:
	int id, app_id, src, dst, flow;
	Req(int id2, int app_id2, int a, int b, int c)
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
		

		d.resize(n);
        p.resize(n);
		adjL.resize(n);//点的编号从0开始
		adjRL.resize(n);
		adj.resize(n);
		for(int i=0;i<n;i++)
			adj[i].resize(n);
		cost_best.resize(APPNUM);
		//cost_LP.resize(APPNUM);

		int a,b,c,d;
		int temp=m/2;
		for(int i=0;i<temp;i++)
		{
			infile>>a>>b>>c>>d;
			Edge* e1=new Edge(2*i,a,b,c,d);
			Edge* e2=new Edge(2*i+1,b,a,c,d);

			incL.push_back(e1);incL.push_back(e2);
			adjL[a].push_back(e1);adjL[b].push_back(e2);
			adjRL[b].push_back(e1);adjRL[a].push_back(e2);
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
			if (d[s] + flow/(adjL[s][i]->capacity - x) < d[adjL[s][i]->dst]){
                d[adjL[s][i]->dst] = d[s] + flow/(adjL[s][i]->capacity - x);
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

	double network_delay(vector<Req*> &reqL,int app_id)
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
	

		IloNumVarArray D(environment,m,0,Inf);

		//优化目标
		IloExpr goal(environment);
		IloExpr temp(environment);
		IloExprArray L(environment,m);

		//L[i]:第i条边在该次流量部署后的流量
		for(int i=0;i<m;i++)
			L[i]=IloExpr(environment);
		for(int i=0;i<m;i++)
		{
			L[i]+=adj[incL[i]->src][incL[i]->dst];
			for(int d=0;d<K;d++)
				L[i]+=x[d][i]*reqL[d]->flow;
		}

		//延时约束条件，分段线性
		for(int i=0;i<m;i++)
		{
			double c=incL[i]->capacity;
			model.add(D[i] >= L[i]);
			model.add(D[i] >= 3*L[i]-2*c/3);
			model.add(D[i] >= 10*L[i]-16*c/3);
			model.add(D[i] >= 70*L[i]-178*c/3);
		}

		//maximize happiness
		for(int d=0;d<K;d++)
		{
			for(int i=0;i<m;i++)		
			{
				int src=incL[i]->src, dst=incL[i]->dst;
				temp += x[d][i] * reqL[d]->flow * D[i];
			}
			goal += temp;
		}
		//虽然这里计算temp的方法是估算，但是和排队延时公式拥有相同的走势
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
			model.add(constraint <= (incL[i]->capacity - adj[incL[i]->src][incL[i]->dst]));
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
					if(solver.getValue(x[d][i])>0.5){
						if(incL[i]->capacity - adj[incL[i]->src][incL[i]->dst]==0)
							latency += reqL[d]->flow/
							(Rinf+incL[i]->capacity - adj[incL[i]->src][incL[i]->dst]);
						else
							latency += reqL[d]->flow/
							(incL[i]->capacity - adj[incL[i]->src][incL[i]->dst]);
					}
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

class TENetworkGraph
{
public:
	int n,m;
	set<int> S, V;
    vector<int> p;
	vector<double> d;
	vector<Edge*> incL;//边的列表
	vector<vector<Edge*> > adjL,adjRL; //正向和反向邻接链表
	
	vector<vector<double> > adj;//该流维护的邻接矩阵，记录负载
	vector<double> cost_best;//记录每个req的最佳部署结果
	vector<double> cost_LP;//记录每个req的LP部署结果

	TENetworkGraph(){;}
	TENetworkGraph(string address)
	{
		ifstream infile(address);
		infile>>n>>m;
		

		d.resize(n);
        p.resize(n);
		adjL.resize(n);//点的编号从0开始
		adjRL.resize(n);
		cost_best.resize(Maxreq);
		cost_LP.resize(Maxreq);
		adj.resize(n);
		for(int i=0;i<n;i++)
			adj[i].resize(n);


		int a,b,c,d;
		int temp=m/2;
		for(int i=0;i<temp;i++)
		{
			infile>>a>>b>>c>>d;
			Edge* e1=new Edge(2*i,a,b,c,d);
			Edge* e2=new Edge(2*i+1,b,a,c,d);

			incL.push_back(e1);incL.push_back(e2);
			adjL[a].push_back(e1);adjL[b].push_back(e2);
			adjRL[b].push_back(e1);adjRL[a].push_back(e2);
		}
	}

	void Update(int s,int flow){
        for (int i = 0; i < adjL[s].size();i++){
			int src=adjL[s][i]->src;
			int dst=adjL[s][i]->dst;
			double x=flow+adj[src][dst];
			if(x > adjL[s][i]->capacity)continue;
			if (d[s] + flow/(adjL[s][i]->capacity - x) < d[dst]){
                d[adjL[s][i]->dst] = d[s] + flow/(adjL[s][i]->capacity - x);
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

    double dijkstra(int src, int dst, int flow){
        S.clear();
        V.clear();
        for (int i = 0; i < n; i++)
        {
            S.insert(i);
            d[i] = Inf;
            p[i] = -2;
        }
        d[src] = 0; p[src] = -1;
        Update(src,flow);
        S.erase(src);
        V.insert(src);
        while (S.size() != 0)
        {
            int mind;
            mind = FindMin();
            if (mind == dst) return d[mind];
			if (mind==-1) break;//没有路可达
            Update(mind,flow);
            S.erase(mind);
            V.insert(mind);
        }
		return Inf;//没有路可达
    }

};

class DelayNetworkGraph
{
public:
	int n,m;
	set<int> S, V;
    vector<int> p;
	vector<double> d;
	vector<Edge*> incL;//边的列表
	vector<vector<Edge*> > adjL,adjRL; //正向和反向邻接链表
	
	vector<vector<double> > adj;//该流维护的邻接矩阵，记录负载
	vector<double> cost_best;//记录每个req的最佳部署结果
	vector<double> cost_LP;//记录每个req的LP部署结果

	DelayNetworkGraph(){;}
	DelayNetworkGraph(string address)
	{
		ifstream infile(address);
		infile>>n>>m;
		

		d.resize(n);
        p.resize(n);
		adjL.resize(n);//点的编号从0开始
		adjRL.resize(n);
		cost_best.resize(Maxreq);
		cost_LP.resize(Maxreq);
		adj.resize(n);
		for(int i=0;i<n;i++)
			adj[i].resize(n);


		int a,b,c,d;
		int temp=m/2;
		for(int i=0;i<temp;i++)
		{
			infile>>a>>b>>c>>d;
			Edge* e1=new Edge(2*i,a,b,c,d);
			Edge* e2=new Edge(2*i+1,b,a,c,d);

			incL.push_back(e1);incL.push_back(e2);
			adjL[a].push_back(e1);adjL[b].push_back(e2);
			adjRL[b].push_back(e1);adjRL[a].push_back(e2);
		}
	}

	void Update(int s,int flow){
        for (int i = 0; i < adjL[s].size();i++){
			int src=adjL[s][i]->src;
			int dst=adjL[s][i]->dst;
			double x=flow+adj[src][dst];
			if(x > adjL[s][i]->capacity)continue;
			if (d[s] + flow/(adjL[s][i]->capacity - x) < d[dst]){
                d[adjL[s][i]->dst] = d[s] + flow/(adjL[s][i]->capacity - x);
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

    double dijkstra(int src, int dst, int flow){
        S.clear();
        V.clear();
        for (int i = 0; i < n; i++)
        {
            S.insert(i);
            d[i] = Inf;
            p[i] = -2;
        }
        d[src] = 0; p[src] = -1;
        Update(src,flow);
        S.erase(src);
        V.insert(src);
        while (S.size() != 0)
        {
            int mind;
            mind = FindMin();
            if (mind == dst) return d[mind];
			if (mind==-1) break;//没有路可达
            Update(mind,flow);
            S.erase(mind);
            V.insert(mind);
        }
		return Inf;//没有路可达
    }

};

#endif