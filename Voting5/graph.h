#ifndef RESOURCES_H
#define RESOURCES_H

#include"common.h"
#include "res.h"

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
	int id, src, dst, flow;
	Req(int id2, int a, int b, int c)
	{
		id=id2;
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
	vector<vector<Edge*> > adj;//邻接矩阵

	vector<Req*> reqL;
	vector<double> cost_best;//记录每个req的最佳部署结果
	vector<double> cost_LP;//记录每个req的LP部署结果

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
		cost_best.resize(Maxreq);
		cost_LP.resize(Maxreq);

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
			adj[a][b]=e1;adj[b][a]=e2;
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
        
		for (int i = 0; i < adjL[s].size();i++){
			int src,dst;
			src=adjL[s][i]->src;dst=adjL[s][i]->dst;//这里src = s
			
			if(flow > (adjL[s][i]->capacity - adj[src][dst] ))continue;//该link无法通过该流
			
			double temp;//link[i][j]可以通过的最大流（的带宽）
			if(d[src] > adjL[src][i]->capacity-adj[src][dst]-flow) temp=adjL[src][i]->capacity-adj[src][dst]-flow;//水管受限
			else temp=d[src];//水源受限(到src点的路径带宽有限)
			
			if(temp>d[dst]) {d[dst]=temp;p[dst]=src;}//发现拥有更大带宽的路，更新
			else d[dst]=d[dst];//因为还有更宽的路可以到达dst，所以这里不更新
		}
    }

    int FindMax(){
        set<int>::iterator it, iend;
        iend = S.end();
        double maxe = 0;
        int max_node = -1;
        for (it = S.begin(); it != iend; it++){
            if(d[*it] > maxe) {
                maxe = d[*it];
                max_node = *it;
            }
        }
        return max_node;
    }

    double dijkstra(int src, int dst, int flow, vector<vector<double> > &adj){
        S.clear();
        V.clear();
        for (int i = 0; i < n; i++)
        {
            S.insert(i);
            d[i] = 0;
            p[i] = -2;
        }
        d[src] = Inf; p[src] = -1;
        Update(src,flow,adj);
        S.erase(src);
        V.insert(src);
        while (S.size() != 0)
        {
            int mind;
            mind = FindMax();
            if (mind == dst) return d[mind];
			if (mind==-1) break;//没有路可达
            Update(mind,flow,adj);
            S.erase(mind);
            V.insert(mind);
        }
		return 0;//没有路可达
    }
	
};

class PGraph
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

	PGraph(){;}
	PGraph(string address)
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
			
			if(flow > (adjL[s][i]->capacity-adj[src][dst]))continue;
			
			double temp;//link[i][j]可以通过的最大流（的带宽）
			if(d[src] > adjL[src][i]->capacity-flow-adj[src][dst]) temp=adjL[src][i]->capacity-flow-adj[src][dst];//水管受限
			else temp=d[src];//水源受限(到src点的路径带宽有限)
			
			if(temp>d[dst]) {d[dst]=temp;p[dst]=src;}//发现拥有更大带宽的路，更新
			else d[dst]=d[dst];//因为还有更宽的路可以到达dst，所以这里不更新
            }
	}
    

    int FindMax(){
        set<int>::iterator it, iend;
        iend = S.end();
        double maxe = 0;
        int max_node = -1;
        for (it = S.begin(); it != iend; it++){
            if(d[*it] > maxe) {
                maxe = d[*it];
                max_node = *it;
            }
        }
        return max_node;
    }

    double dijkstra(int src, int dst, int flow){
        S.clear();
        V.clear();
        for (int i = 0; i < n; i++)
        {
            S.insert(i);
            d[i] = 0;
            p[i] = -2;
        }
        d[src] = Inf; p[src] = -1;
        Update(src,flow);
        S.erase(src);
        V.insert(src);
        while (S.size() != 0)
        {
            int mind;
            mind = FindMax();
            if (mind == dst) return d[mind];
			if (mind==-1) break;//没有路可达
            Update(mind,flow);
            S.erase(mind);
            V.insert(mind);
        }
		return 0;//没有路可达
    }

};

#endif