#ifndef APP_H
#define APP_H

#include "common.h"
#include "graph.h"
#include "ext.h"

class APP
{
public:
	int app_id;//APP的编号
	vector<vector<double> > adjMyFlow;//记录属于自己APP的链路上负载
	vector<int> pathRecord;//APP提出的具体方案：路径记录,由于是在线问题，所以就一条路径
	vector<double> judge;//APP对所有方案的评价
	 
	//dijkstra需要用的变量
	set<int> S;//还没有找出最短路径的节点集合
	vector<int> p;
	vector<double> d;
	VGraph* gv;//这里不可以声明引用，因为引用声明直接赋初始值

	//初始化，只初始化一次，之后其他需求来的时候，
	//只修改之前参数，相当于投票的基础设施只建立一次，剩下的是维护
	APP(int app_id2,VGraph &gv2)
	{
		app_id=app_id2;
		pathRecord.clear();
		judge.clear();
		adjMyFlow.resize(N);
		for (int i = 0; i<N; i++)
			adjMyFlow[i].resize(N);
		judge.resize(APPNUM);
		gv = &gv2;//方便dijkstra查询
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
		for (int i = 0; i < gv->adjL[src].size(); i++){
			int dst = gv->adjL[src][i]->dst;
			double load = gv->adj[src][dst] + req.flow;
			//1/(c-x)排队延时公式，一个包的延时
			double newLatency = d[src] + 1 / (gv->adjL[src][i]->capacity - load);

			if (load > gv->adjL[src][i]->capacity)continue;
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
			update(next, req);S.erase(next); 
		}
		if (findIt == 0) cout << "no path found" << endl;
	}

	//通过特殊处理后的图，在减少对自己流影响下，选一条最短路给该流
	void reduce_cost(Req &req){
		for (int i = 0; i < N; i++){
			for (int j = 0; j < N; j++){
				gv->adj[i][j] += adjMyFlow[i][j];
			}
		}
		dijkstra(req);
		
		//还原之前的网络状态
		for (int i = 0; i < N; i++){
			for (int j = 0; j < N; j++){
				gv->adj[i][j] -= adjMyFlow[i][j];
			}
		}
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
		else{
			reduce_cost(req);
		}
	}

	//流部署 自己提出的方案
	void begin_implement(VGraph &g)
	{
		;
	}
	
	//应用评价所有方案
	void evaluate(Req &req, vector<APP*> &appL)
	{
		/*if this flow is mine, calculate the delay of this flow*/
		if (req.app_id == app_id){
			for (int i = 0; i < appL.size(); i++){
				double delaySum = 0;
				for (int j = 0; j < appL[i]->pathRecord.size()-1; j++){
					int tail = appL[i]->pathRecord[j];
					int head = appL[i]->pathRecord[j + 1];
					//delay=flow/(capacity-load)越小越好
					delaySum += req.flow / 
						(gv->adjM[tail][head]->capacity - gv->adj[tail][head] - req.flow);
				}
				judge[i] = delaySum;
			}
		}
		/*if this flow is not mine, calculate the effect to me 
		which is due to this flow*/
		else{
			for (int i = 0; i < appL.size(); i++){
				double effect = 0;
				for (int j = 0; j < appL[i]->pathRecord.size() - 1; j++){
					int tail = appL[i]->pathRecord[j];
					int head = appL[i]->pathRecord[j + 1];
					//effect=myLoad/(capacity-load）越小越好
					effect = appL[app_id]->adjMyFlow[tail][head] /
						(gv->adjM[tail][head]->capacity - gv->adj[tail][head] - req.flow);
				}
				judge[i] = effect;
			}
		}
	}

};//APP类的结束位置
#endif