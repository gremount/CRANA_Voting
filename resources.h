#pragma once
#include "common.h"

class CEdge{
private:
	int tail, head;
	int weight, capacity;
public:
	CEdge(int a, int b, int c, int d);
	CEdge(int a, int b, int c);
	CEdge(CEdge &x);
	int getHead(){return head;}
	int getTail(){return tail;}
	int getWeight(){return weight;}
	int getCap(){return capacity;}
	bool operator<(CEdge& x){
		if(weight<x.weight)
			return 1;
		else 
			return 0;
	}
};

class CReq {
public:
	int src;
	int dst;
	int bw;
	CReq(int a,int b, int c){src=a;dst=b;bw=c;}
};

class CVertex {
public:
	int d;
	int p;
	int ID;
	CVertex(int id){d=INF;p=-2;ID=id;}
	~CVertex();
};

class CPath {
public:
	int size;//the num of nodes
	list<int> path;
	CPath(){size=0;}
	~CPath();
};

class CGraph{
private:
	int numVertex;
	int numEdge;
	list<CEdge*> IncidentList;
public:
	CGraph(char* inputFile);
	CGraph(list<CEdge*> listEdge,int node_num,int edge_num);
	CGraph(CGraph &);

	set<int> S;
	set<int> V;
	int d[N+10];
	int p[N+10];

	map<int,int> degree_vertex;
	multimap<int,int> degree_vertex2;
	map<int,list<int>> adjlist;
	vector<vector<CEdge*>> adjmatrix;
	map<int,list<CEdge*>> nelist;
	map<int,CVertex*> mapVID_Vertex;

	int link_bw[K+1][N+1][N+1];
	int bw[K+1];
	vector<CReq*> r;
	vector<vector<CPath*> > path_record;
	int judge[K+1][K+1];
	int judge_sum[K+1];

	void single_flow_propose(int k);
	void single_flow_implement(CPath* p, int k,int k2);
	void single_flow_evaluate(int k);

	void cost_evaluate(int k);

	int getNumVertex(){
		return numVertex;
	}
	int getNumEdge(){
		return numEdge;
	}
	int cmp(const pair<int,int> &x, const pair<int, int> &y){
		return x.second > y.second;
	}
	void p1(){
		list<CEdge*>::iterator it,iend;
		multimap<int,int>::iterator it2;
		iend=IncidentList.end();
		vector<pair<int,int>> dv_vec;
		for(int i=1;i<=N;i++)
			degree_vertex[i]=0;
		for(it=IncidentList.begin();it!=iend;it++)
			degree_vertex[(*it)->getTail()]++;
		for(int i=1;i<=N;i++)
			degree_vertex2.insert(pair<int,int>(degree_vertex[i],i));
		it2=--degree_vertex2.end();
		printf("project 1:\n");
		for(;it2!=degree_vertex2.begin();it2--)
			printf("%d,%d\n",it2->second,it2->first);
	}
	void p2(){
		list<CEdge*>::iterator it,iend;
		list<int>::iterator it2,iend2;
		iend=IncidentList.end();
		//printf("incidentList:\n");
		for(it=IncidentList.begin();it!=iend;it++){
			adjlist[(*it)->getTail()].push_back((*it)->getHead());
			//printf("%d,%d\n",(*it)->getTail(),(*it)->getHead());
		}
		it2=adjlist[3].begin();
		iend2=adjlist[3].end();
		printf("\n");
		printf("project 2:\n");
		printf("3:");
		for(;it2!=iend2;it2++)
			printf("%d ",*it2);
	}
	void p3(){
		list<CEdge*>::iterator it,iend;
		iend=IncidentList.end();
		CEdge* emptyedge=new CEdge(-1,-1,-1,-1);
		for(int i=0;i<=numVertex;i++)
		{
			vector<CEdge*> vec;
			for(int j=0;j<=numVertex;j++)
			{
				vec.push_back(emptyedge);
			}
			adjmatrix.push_back(vec);
		}
		for(it=IncidentList.begin();it!=iend;it++){
			//CEdge* e = new CEdge((*it)->getTail(),(*it)->getHead(),(*it)->getWeight(),(*it)->getCap());
			adjmatrix[(*it)->getTail()][(*it)->getHead()] = *it ;
		}

	}

	void p4(){
		list<CEdge*>::iterator it,iend;
		iend=IncidentList.end();
		for(it=IncidentList.begin();it!=iend;it++){
			nelist[(*it)->getTail()].push_back(*it);
		}
	}
	


	void Update(int i,int k,int k2){
		list<CEdge*>::iterator it,iend;
		it=nelist[i].begin();
		iend=nelist[i].end();
		for(;it!=iend;it++){
			if(((*it)->getCap()-link_bw[k][(*it)->getTail()][(*it)->getHead()])<bw[k2]) continue;
			if((*it)->getWeight()+d[i]<d[(*it)->getHead()]){
				d[(*it)->getHead()]=(*it)->getWeight()+d[i];
				p[(*it)->getHead()]=i;
				mapVID_Vertex[(*it)->getHead()]->d=(*it)->getWeight()+d[i];
				mapVID_Vertex[(*it)->getHead()]->p=i;
			}
		}
	}

	int FindMin(){
		set<int>::iterator vi,vend;
		vend=V.end();
		int mini=10000000;
		int loc=0;
		for(vi=V.begin();vi!=vend;vi++)
			if(mini>=d[*vi])
				{mini=d[*vi];loc=*vi;}
		return loc;
	}

	CPath* make_CPath(int k,int k2){
		int d;
		d=r[k2]->dst;
		CPath* pa = new CPath();
		while(1)
		{
			pa->path.push_front(d);
			pa->size++;
			if(p[d]==-1) break;
			d=p[d];
		}
		list<int>::iterator it,iend;
		iend=pa->path.end();
		for(it=pa->path.begin();it!=iend;it++)
			cout<<*it<<" ";
		cout<<endl;
		path_record[k].push_back(pa);
		return pa;
	}

	//用dijkstra计算单源单宿最短路,k表示提出方案的流，k2表示当前算路的流
	void DijkstraAlg(int k,int k2){
		int i,j;
		int src,dst;
		src=r[k2]->src;
		dst=r[k2]->dst;
		S.clear();
		V.clear();
		for(i=1;i<=numVertex;i++)
			{d[i]=INF;p[i]=-2;}
		for(i=1;i<=numVertex;i++)
		{
			V.insert(i);
			CVertex* node = new CVertex(i);
			mapVID_Vertex[i]=node;
		}	
		S.insert(src);
		V.erase(src);
		d[src]=0;
		p[src]=-1;
		mapVID_Vertex[src]->d=0;
		mapVID_Vertex[src]->p=-1;
		Update(src,k,k2);
		while (V.size()!=0){
			j=FindMin();
			if(j==dst) {
				CPath* p;
				p=make_CPath(k,k2);
				single_flow_implement(p,k,k2);
			}
			S.insert(j);
			V.erase(j);
			Update(j,k,k2);
		}
		printf("distance is %d\n",d[dst]);
	}

};
