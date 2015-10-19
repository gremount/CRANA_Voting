#pragma once
#include "resources.h"
#include "single_flow.h"

CEdge::CEdge(int a, int b, int c, int d){
	tail=a;
	head=b;
	weight=c;
	capacity=d;
}

CEdge::CEdge(int a, int b, int c){
	head=b;
	tail=a;
	weight=c;
}

CEdge::CEdge(CEdge & x){
	tail=x.getTail();
	head=x.getHead();
	weight=x.getWeight();
	capacity=x.getCap();
}

CGraph::CGraph(list<CEdge*> listEdge,int node_num,int edge_num){
	IncidentList=listEdge;
	numVertex=node_num;
	numEdge=edge_num*2;
	for(int i=0;i<=K;i++)
		for(int j=0;j<=N;j++)
			for(int k=0;k<=N;k++)
				link_bw[i][j][k]=0;
	for(int i=0;i<=K;i++)
	{
		CPath* p=new CPath();
		vector<CPath*> vec;
		vec.push_back(p);
		path_record.push_back(vec);
	}
}

int main()
{
	list<CEdge*> listEdge;

	//图的初始化
	ifstream test("d:\\a\\CRANA_Voting\\graph1.txt");
	int node_num,edge_num;
	int src,dst,weight,cap;
	test>>node_num>>edge_num;
	for(int i=1;i<=edge_num;i++)
	{
		test>>src>>dst>>weight>>cap;
		CEdge* e1=new CEdge(src,dst,weight,cap);
		CEdge* e2=new CEdge(dst,src,weight,cap);
		listEdge.push_back(e1);
		listEdge.push_back(e2);
	}
	CGraph g(listEdge,node_num,edge_num);
	g.p3();
	g.p4();

	//需求记录
	CReq* r1=new CReq(0,0,0);
	g.r.push_back(r1);
	ifstream flow_test("d:\\a\\CRANA_Voting\\req1.txt");
	for(int i=1;i<=K;i++)
	{
		int src,dst,bw;
		flow_test>>src>>dst>>bw;
		CReq* r2=new CReq(src,dst,bw);
		g.bw[i]=bw;
		g.r.push_back(r2);
	}

	//提方案
	for(int i=1;i<=K;i++)
	{
		g.single_flow_propose(i);
	}

	//评价
	for(int i=1;i<=K;i++)
		for(int j=1;j<=K;j++)
		g.judge[i][j]=0;
	for(int i=1;i<=K;i++)
		g.single_flow_evaluate(i);
	cout<<endl;
	cout<<"************judge value**************"<<endl;
	for(int i=1;i<=K;i++)
	{
		cout<<"flow "<<i<<" 对所有方案的评价";
		for(int j=1;j<=K;j++)
			cout<<g.judge[i][j]<<" ";
		cout<<endl;
	}
	getchar();
	return 0;
}