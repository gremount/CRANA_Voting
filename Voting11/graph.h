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
	vector<vector<Edge*> > adj;//邻接矩阵, 记录的是边，不是负载

	vector<Req*> reqL;
	vector<double> cost_best;//记录每个req的最佳部署结果
	vector<double> cost_LP;//记录每个req的LP部署结果

	VGraph(){;}

	VGraph(string address)
	{
		ifstream infile(address);
		infile>>n>>m;
		//cout<<n<<" "<<m<<endl;

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
		
		for(int i=0;i<m;i++)
		{
			infile>>a>>b>>c>>d;
			Edge* e=new Edge(i,a,b,c,d);
		
			incL.push_back(e);
			adjL[a].push_back(e);
			adjRL[b].push_back(e);
			adj[a][b]=e;
		}
		cout<<"voting graph init completed"<<endl;
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
		
		for(int i=0;i<m;i++)
		{
			infile>>a>>b>>c>>d;
			Edge* e=new Edge(i,a,b,c,d);
		
			incL.push_back(e);
			adjL[a].push_back(e);
			adjRL[b].push_back(e);
		}
	}

	void Update(int s,int flow){
        for (int i = 0; i < adjL[s].size();i++){
			int src=adjL[s][i]->src;
			int dst=adjL[s][i]->dst;
			double x=flow+adj[src][dst];
			if(x > adjL[s][i]->capacity)continue;
			if (d[s] + flow/(adjL[s][i]->capacity - x + 1) < d[dst]){
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