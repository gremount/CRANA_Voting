#ifndef FLOW_H
#define FLOW_H

#include"graph.h"
#include"common.h"
#include "res.h"
#include "LP_Voting.h"

class Flow
{
public:
	int id;//第id个流需求
	int src,dst,flow;//源地址，目的地址，流大小
	vector<vector<double> > adj;//该流维护的邻接矩阵，记录负载
	vector<Path*> path_record;//具体方案：路径记录
	vector<double> judge;//该流对所有方案的评价
	

	//dijkstra需要用到的变量
	set<int> S, V;
    vector<double> d;
	vector<int> p;
	//初始化，只初始化一次，之后其他需求来的时候，
	//只修改之前参数，相当于投票的基础设施只建立一次，剩下的是维护
	Flow(int id2, int a, int b, int c)
	{
		id=id2;src=a;dst=b;flow=c;
		adj.clear();
		path_record.clear();
		judge.clear();
		adj.resize(N);
		for(int i=0;i<N;i++)
			adj[i].resize(N);
		path_record.resize(Maxreq);
		judge.resize(Maxreq);
		d.resize(N);
        p.resize(N);
	}

	//流的维护，针对新的流需求
	void flow_modify(int a, int b, int c)
	{
		src=a;dst=b;flow=c;
		path_record.clear();
		judge.clear();
		path_record.resize(Maxreq);
		judge.resize(Maxreq);
	}

	//流提出方案
	void propose(VGraph &g,vector<Flow*> &flowL)
	{
		//对自己的流先算dijkstra
		int loc=dst;
		double dist=0;
		if(id!=0)
		{
			dist=dijkstra(src,dst,flow,g);
			if(dist==Inf){cout<<"*********** no path *********"<<endl;}
			//没有路径可以安排，在evaluate里就要增加惩罚
			else{
				//将路径记录下来
				Path* path = new Path();
				while(p[loc]!=-1){
					path->pathL.push_back(g.adj[p[loc]][loc]);
					loc=p[loc];
				}
				path_record[id]=path;

				//部署这条路径
				int edge_num=path->pathL.size();
				for(int j=0;j<edge_num;j++)
				{
					int src,dst;
					src=path_record[id]->pathL[j]->src;
					dst=path_record[id]->pathL[j]->dst;
					adj[src][dst] += 1*g.reqL[id]->flow;//假设有2倍的流量要部署，这样的决策更自私，想让其他应用不来占用该流的路径
				}
			}
		}
		
		//对其他的流用规划计算路径
		vector<Req*> reqPL;
		for(int i=0;i<g.reqL.size();i++)
		{	
			if(id==0 && i==id) reqPL.push_back(g.reqL[i]);
			else if(i==id) continue;
			else	  reqPL.push_back(g.reqL[i]);
		}

		LP_Voting(&g,reqPL,path_record,id,adj);
		begin_implement(g);

	}

	//流部署 自己提出的方案,除掉dijkstra算出的路
	void begin_implement(VGraph &g)
	{
		for(int i=0;i<Maxreq;i++)
		{
			//if(i==id) continue;
			int edge_num=path_record[i]->pathL.size();
			for(int j=0;j<edge_num;j++)
			{
				int src,dst;
				src=path_record[i]->pathL[j]->src;
				dst=path_record[i]->pathL[j]->dst;
				if(id==0 && i==id)
					adj[src][dst] += g.reqL[i]->flow;
				else if(i!=id)
					adj[src][dst] += g.reqL[i]->flow;
				else
					continue;
					//adj[src][dst] -= 1*g.reqL[i]->flow;//去掉APP在propose()里假想的流量
			}
		}
	}
	
	//流评价所有方案
	void evaluate(VGraph &g, vector<Flow*> &flowL)
	{
		//流评价所有方案
		int edge_num=0;//temp记录路径权值和
		double temp;
		for(int i=0;i<Maxreq;i++)
		{
			edge_num=flowL[i]->path_record[id]->pathL.size();
			temp=0;
			for(int j=0;j<edge_num;j++)
			{
				int src=flowL[i]->path_record[id]->pathL[j]->src;
				int dst=flowL[i]->path_record[id]->pathL[j]->dst;
				int capacity=flowL[i]->path_record[id]->pathL[j]->capacity;
				if(capacity-flowL[i]->adj[src][dst]==0)
					temp+=flow/(Rinf+capacity-flowL[i]->adj[src][dst]);
				else
					temp+=flow/(capacity-flowL[i]->adj[src][dst]);
			}
			judge[i]=temp;
			//if(judge[i]==0) judge[i]=Maxpath*flow;//没有路径可以安排，就要增加惩罚
		}
	}

	//流部署 获胜的方案, 直接在main.cpp里实现也可以
	/*
	void end_implement(VGraph &g,int winner, vector<Flow*> &flowL)
	{
		//Flow记录的图负载信息更新
		for(int i=0;i<Maxreq;i++)
		{
			if(i==winner) continue;
			int edge_num=flowL[winner]->path_record[i]->pathL.size();
			for(int j=0;j<edge_num;j++)
			{
				int src,dst;
				src=flowL[winner]->path_record[i]->pathL[j]->src;
				dst=flowL[winner]->path_record[i]->pathL[j]->dst;
				adj[src][dst] = flowL[winner]->adj[src][dst];
			}
		}
	}
	*/
	void Update(int s,int flow, VGraph &g){
		double x;
        for (int i = 0; i < g.adjL[s].size();i++){
			int src,dst;
			src=g.adjL[s][i]->src;dst=g.adjL[s][i]->dst;
			x=adj[src][dst]+flow;
			if(x>g.adjL[s][i]->capacity)continue;
			if (d[s] + flow/(g.adjL[s][i]->capacity - x + 1) < d[dst]){
                d[dst] = d[s] + flow/(g.adjL[s][i]->capacity - x + 1);
                p[dst] = s;
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

    double dijkstra(int src, int dst, int flow, VGraph &g){
        S.clear();
        V.clear();
        for (int i = 0; i < N; i++)
        {
            S.insert(i);
            d[i] = Inf;
            p[i] = -2;
        }
        d[src] = 0; p[src] = -1;
        Update(src,flow,g);
        S.erase(src);
        V.insert(src);
        while (S.size() != 0)
        {
            int mind;
            mind = FindMin();
            if (mind == dst) return d[mind];
			if (mind==-1) break;//没有路可达
            Update(mind,flow,g);
            S.erase(mind);
            V.insert(mind);
        }
		return Inf;//没有路可达
    }
};

//Voting统计网络latency
double judge_sum_function(VGraph &g, vector<Flow*> &flowL, int winner)
{
	double latencyVoting=0;
	for(int i=0;i<M;i++)
	{
		int src,dst;
		double latencyTemp;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		if(flowL[winner]->adj[src][dst]==0) continue;//由于延时函数是1+x2/(c-x1)，所以即使没有流量，
		//依然会在统计中算入延时，所以这里判断一下来消除这些误判
		if(g.incL[i]->capacity - flowL[winner]->adj[src][dst]==0)
			latencyTemp = (double)flowL[winner]->adj[src][dst]/(Rinf+ g.incL[i]->capacity - flowL[winner]->adj[src][dst]);
		else
			latencyTemp = (double)flowL[winner]->adj[src][dst]/(g.incL[i]->capacity - flowL[winner]->adj[src][dst]);
		//cout<<src<<" "<<dst<<" " <<flowL[winner]->adj[src][dst]<<endl;
		latencyVoting += latencyTemp;
	}
	return latencyVoting;
}


//TE全局优化后统计网络latency
double delay_TENetworkGraph(TENetworkGraph &g, vector<Flow*> &flowL)
{
	double judge_sum_LP=0;
	for(int i=0;i<M;i++)
	{
		int src,dst;
		double latencyFunc;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		if(g.adj[src][dst]==0) continue;
		if(g.incL[i]->capacity - g.adj[src][dst]==0)
			latencyFunc = (double)g.adj[src][dst]/(Rinf+ g.incL[i]->capacity - g.adj[src][dst]);
		else
			latencyFunc = (double)g.adj[src][dst]/(g.incL[i]->capacity - g.adj[src][dst]);
		judge_sum_LP += latencyFunc;
	}
	return judge_sum_LP;
}

//Delay全局优化后统计网络latency
double delay_DelayNetworkGraph(DelayNetworkGraph &g, vector<Flow*> &flowL)
{
	double judge_sum_LP=0;
	for(int i=0;i<M;i++)
	{
		int src,dst;
		double latencyFunc;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		if(g.adj[src][dst]==0) continue;
		if(g.incL[i]->capacity - g.adj[src][dst]==0)
			latencyFunc = (double)g.adj[src][dst]/(Rinf+ g.incL[i]->capacity - g.adj[src][dst]);
		else
			latencyFunc = (double)g.adj[src][dst]/(g.incL[i]->capacity - g.adj[src][dst]);
		judge_sum_LP += latencyFunc;
	}
	return judge_sum_LP;
}

#endif