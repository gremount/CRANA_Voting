#ifndef APP_H
#define APP_H

#include "common.h"
#include "graph.h"
#include "ext.h"

class APP
{
public:
	int app_id;//APP的编号
	vector<vector<double> > adj;//该APP维护的邻接矩阵，记录真实的链路上负载
	vector<vector<double> > adjMyFlow;//记录属于自己APP的链路上负载
	vector<int> pathRecord;//APP提出的具体方案：路径记录,由于是在线问题，所以就一条路径
	vector<double> judge;//APP对所有方案的评价
	 
	//dijkstra需要用的变量
	set<int> S;//还没有找出最短路径的节点集合
	vector<int> p;
	vector<double> d;
	VGraph* g;//这里不可以声明引用，因为引用声明直接赋初始值

	//初始化，只初始化一次，之后其他需求来的时候，
	//只修改之前参数，相当于投票的基础设施只建立一次，剩下的是维护
	APP(int app_id2,VGraph &gv)
	{
		app_id=app_id2;
		adj.clear();
		pathRecord.clear();
		judge.clear();
		adj.resize(N);
		for(int i=0;i<N;i++)
			adj[i].resize(N);
		adjMyFlow.resize(N);
		for (int i = 0; i<N; i++)
			adjMyFlow[i].resize(N);
		judge.resize(APPNUM);
		g = &gv;//方便dijkstra查询
		p.resize(N);
		d.resize(N);
	}

	//流的维护，针对新的流需求
	void init()
	{
		pathRecord.clear();
		judge.clear();
		judge.resize(APPNUM);
	}

	void update(int src, Req &req){
		for (int i = 0; i < g->adjL[src].size(); i++){
			int dst = g->adjL[src][i]->dst;
			double load = adj[src][dst] + req.flow;
			//1/(c-x)排队延时公式，一个包的延时
			double newLatency = d[src] + 1 / (g->adjL[src][i]->capacity - load);

			if (load > g->adjL[src][i]->capacity)continue;			
			if (newLatency < d[dst]){
				d[dst] = newLatency;
				p[dst] = src;
			}
		}
	}

	int find_min(){
		double dmin = INF;
		int dnode = -1;
		for (int i : S){
			if (dmin > d[i]){
				dmin = d[i];
				dnode = i;
			}
		}
		return dnode;
	}

	void record_path(Req &req)
	{
		int node = req.dst;
		do{
			pathRecord.insert(pathRecord.begin(),node);
			node = p[node];
		} while (node != -1);		
	}

	void dijkstra(Req &req)
	{
		int findIt = 0;
		S.clear();
		for (int i = 0; i < N; i++){
			d[i] = INF; p[i] = INF;
			S.insert(i);
		}
		
		d[req.src] = 0; p[req.src] = -1;
		update(req.src, req); S.erase(req.src);

		while (S.size() != 0){
			int next = find_min();
			if (next == req.dst){
				//already found the best path
				findIt = 1;
				record_path(req);
				return;
			}
			update(next,req);S.erase(next); 
		}
		if (findIt == 0) cout << "no path found" << endl;
	}

	void reduce_cost(){
		;
	}


	//流提出方案
	void propose(Req &req)
	{	
		//if this flow is mine, choose the best path for this flow
		if (req.app_id == app_id){
			dijkstra(req);
		}

		//if this flow is not mine, choose the path which will not
		//hurt my interest
		if (req.app_id != app_id){
			reduce_cost();
		}
	}

	//流部署 自己提出的方案
	void begin_implement(VGraph &g)
	{
		;
	}
	
	//应用评价所有方案
	void evaluate(VGraph &g, vector<APP*> &appL)
	{
		/*
		//评价所有APP提出的方案
		int edge_num=0;//temp记录路径权值和
		double temp;
		for(int k=0;k<APPNUM;k++)
		{
			judge[k]=0;
			for(int i=0;i<Maxreq;i++)
			{
				if(g.reqL[i]->app_id!=app_id){
					//judge[k]++;
					continue;
				}//该流不属于自己，就不管该流的利益
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
		*/
	}

	//应用部署 获胜的方案
	void end_implement(VGraph &g,vector<APP*> &appL)
	{
		//APP记录的图负载信息更新
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

	//线性拟合 1/(c-x)
	double linearCal(double load, double capacity)
	{
		double util=load/capacity;
		if(util<0.3333) return load/capacity;
		else if(util<0.6667) return 3*load/capacity - 2.0/3.0;
		else if(util<0.9) return 10*load/capacity - 16.0/3.0;
		else return 70*load/capacity - 178.0/3.0;
	}

};//APP类的结束位置
#endif