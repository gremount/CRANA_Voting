#ifndef NET_H
#define NET_H

#include "decider.h"

class Net: public Decider
{
public:
	//int app_id;//APP的编号
	//vector<vector<double> > adjMyFlow;//记录属于自己APP的链路上负载
	//vector<int> pathRecord;//APP提出的具体方案：路径记录,由于是在线问题，所以就一条路径
	//vector<double> judge;//APP对所有方案的评价
	 
	//dijkstra需要用的变量
	set<int> S;//还没有找出最短路径的节点集合
	vector<int> p;
	vector<double> d;
	VGraph* gv;//这里不可以声明引用，因为引用声明直接赋初始值

	//初始化，只初始化一次，之后其他需求来的时候，
	//只修改之前参数，相当于投票的基础设施只建立一次，剩下的是维护
	Net(int app_id2, VGraph &gv2) :Decider(app_id2,gv2)
	{
		gv = &gv2;//方便dijkstra查询
		p.resize(N);
		d.resize(N);
	}

	//流的维护，针对新的流需求
	void init()
	{
		pathRecord.clear();
		judge.clear();
		judge.resize(DECNUM);
	}

	void update(int src, Req &req){
		for (int i = 0; i < gv->adjL[src].size(); i++){
			int dst = gv->adjL[src][i]->dst;
			double load = gv->adj[src][dst] + req.flow;
			double capacity = gv->adjL[src][i]->capacity;
			//Net模块的目标是降低网络的最大链路利用率
			double newTE = 0;
			if (load / capacity > d[src])
				newTE = load / capacity;
			else
				newTE = d[src];
			if (load > capacity)continue;
			if (newTE < d[dst]){
				d[dst] = newTE;
				p[dst] = src;
			}
		}
	}

	int find_min(){
		double dmin = 1.0;
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
			d[i] = 1.0; p[i] = INF;
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

	//流提出方案
	void propose(Req &req)
	{	
		//目标是网络的最大链路利用率最小
		dijkstra(req);
	}
	
	//应用评价所有方案
	void evaluate(Req &req, vector<Decider*> &appL)
	{
		for (int i = 0; i < appL.size(); i++){
			//假设按照该app的摆放流的方式摆放
			for (int j = 0; j < appL[i]->pathRecord.size() - 1; j++){
				int tail = appL[i]->pathRecord[j];
				int head = appL[i]->pathRecord[j + 1];
				gv->adj[tail][head] += req.flow;
			}

			double te = 0;
			for (int j = 0; j < gv->incL.size(); j++){
				int tail = gv->incL[j]->src;
				int head = gv->incL[j]->dst;
				if (te < gv->adj[tail][head] / gv->incL[j]->capacity)
					te = gv->adj[tail][head] / gv->incL[j]->capacity;
			}
			judge[i] = te;

			//还原现场
			for (int j = 0; j < appL[i]->pathRecord.size() - 1; j++){
				int tail = appL[i]->pathRecord[j];
				int head = appL[i]->pathRecord[j + 1];
				gv->adj[tail][head] -= req.flow;
			}
		}
	}//evaluate()结束

};//APP类的结束位置
#endif