#ifndef FLOW_H
#define FLOW_H

#include"graph.h"
#include"common.h"
#include "res.h"
#include "LP2.h"

class Flow
{
public:
	int id;//第id个流需求
	int src,dst,flow;//源地址，目的地址，流大小
	vector<vector<int> > adj;//该流维护的邻接矩阵，记录负载
	vector<Path*> path_record;//具体方案：路径记录
	vector<int> judge;//该流对所有方案的评价
	

	//dijkstra需要用到的变量
	set<int> S, V;
    vector<int> d, p;

	//初始化，只初始化一次，之后其他需求来的时候，
	//只修改之前参数，相当于投票的基础设施只建立一次，剩下的是维护
	Flow(int id2, int a, int b, int c)
	{
		id=id2;src=a;dst=b;flow=c;
		adj.clear();
		path_record.clear();
		judge.clear();
		judge_sum=0;
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
		judge_sum=0;
		path_record.resize(Maxreq);
		judge.resize(Maxreq);
	}

	//流提出方案
	void propose(VGraph &g,vector<Flow*> &flowL)
	{
		//对自己的流先算dijkstra
		int dist=0,loc=dst;
		dist=dijkstra(src,dst,flow,g);
		if(dist==Inf){;}
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
				adj[src][dst] += g.reqL[id]->flow;
			}
		}

		

		//对其他的流用规划计算路径
		vector<Req*> reqPL;
		for(int i=0;i<g.reqL.size();i++)
		{
			//保证流放置的位置和之前记录的id相同
			//同时也要注意LP算出的一个商品是一个空路径
			if(i==id) reqPL.push_back(new Req(src,dst,0));
			else	  reqPL.push_back(g.reqL[i]);
		}

		LP2(&g,reqPL,path_record,id,adj);
		begin_implement(g);

	}

	//流部署 自己提出的方案,除掉dijkstra算出的路
	void begin_implement(VGraph &g)
	{
		for(int i=0;i<Maxreq;i++)
		{
			if(i==id) continue;
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
	
	//流评价所有方案
	void evaluate(VGraph &g, vector<Flow*> &flowL)
	{
		//流评价所有方案
		int temp=0,edge_num=0;//temp记录路径权值和
		for(int i=0;i<Maxreq;i++)
		{
			edge_num=flowL[i]->path_record[id]->pathL.size();
			temp=0;
			for(int j=0;j<edge_num;j++)
				temp+=flowL[i]->path_record[id]->pathL[j]->weight;
			judge[i]=temp*flow;
		}
	}

	//流部署 获胜的方案
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

	void Update(int s,int flow, VGraph &g){
        for (int i = 0; i < g.adjL[s].size();i++){
			int src,dst;
			src=g.adjL[s][i]->src;dst=g.adjL[s][i]->dst;
			if(flow>g.adjL[s][i]->capacity-adj[src][dst])continue;
			if (d[s] + g.adjL[s][i]->weight < d[g.adjL[s][i]->dst]){
                d[g.adjL[s][i]->dst] = d[s] + g.adjL[s][i]->weight;
                p[g.adjL[s][i]->dst] = s;
            }
		}
    }

    int FindMin(){
        set<int>::iterator it, iend;
        iend = S.end();
        int mine = Inf;
        int min_node = -1;
        for (it = S.begin(); it != iend; it++){
            if(d[*it] < mine) {
                mine = d[*it];
                min_node = *it;
            }
        }
        return min_node;
    }

    int dijkstra(int src, int dst, int flow, VGraph &g){
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

//统计累加的cost
void judge_sum_function(VGraph &g, vector<Flow*> &flowL, int winner)
{
	judge_sum=0;
	for(int i=0;i<M;i++)
	{
		int src,dst,weight;
		int cost=0;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		weight=g.incL[i]->weight;
		judge_sum+=weight * flowL[winner]->adj[src][dst];
	}
}


#endif