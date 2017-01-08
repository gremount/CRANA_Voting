#ifndef RESOURCES_H
#define RESOURCES_H

#include"common.h"
#include "ext.h"

class Edge
{
public:
	int id;
	int src,dst;
	double capacity;//链路带宽
	Edge(){;}
	Edge(int a, int b, int c, double d){
		id=a;
		src=b;
		dst=c;
		capacity=d;
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

class VGraph
{
public:
	int n,m;
	set<int> S, V;
    vector<int> p;
	vector<double> d;
	vector<Edge*> incL;//边的列表
	vector<vector<Edge*> > adjL,adjRL; //正向和反向邻接链表
	vector<vector<Edge*> > adjM;//边的邻接矩阵
	vector<vector<double> > adj;//记录负载

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
		adjM.resize(n);
		adj.resize(n);
		for (int i = 0; i < n; i++){
			adj[i].resize(n);
			adjM[i].resize(n);
		}

		int a,b,c;
		double d;
		for(int i=0;i<m;i++)
		{
			infile>>a>>b>>c>>d;
			//cout<<a<<" "<<b<<" "<<c<<endl;
			Edge* ed=new Edge(a,b,c,d*10);
			
			incL.push_back(ed);
			adjL[b].push_back(ed);
			adjRL[c].push_back(ed);
			adjM[b][c] = ed;
		}
		//cout<<"graph init completed"<<endl;
	}
};

#endif