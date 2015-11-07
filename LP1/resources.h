#ifndef RESOURCES_H
#define RESOURCES_H

#include"common.h"

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
	int src, dst, flow;
	Req(int a, int b, int c)
	{
		src=a;
		dst=b;
		flow=c;
	}
};

class Graph
{
public:
	int n,m;
	vector<Edge*> incL;
	vector<vector<Edge*> > adjL,adjRL;
	Graph(){;}
	Graph(string address)
	{
		ifstream infile(address);
		infile>>n>>m;
		m=m*2;

		adjL.resize(n);//点的编号从0开始
		adjRL.resize(n);

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

};

#endif