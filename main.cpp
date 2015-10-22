#pragma once
#include "resources.h"
#include "single_flow.h"
#include "voting.h"
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
	srand((unsigned)time(0));
	int winner = 0;//No.1
	float table[M2+1][N2+1] = {0};
	int ranking[N2+1]={0};//��¼һ�������ͶƱ����

	//ͼ�ĳ�ʼ��
	ifstream test("d:\\a\\CRANA_Voting\\graph6.txt");
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

	//�����¼
	CReq* r1=new CReq(0,0,0);
	g.r.push_back(r1);
	ifstream flow_test("d:\\a\\CRANA_Voting\\req6.txt");
	for(int i=1;i<=K;i++)
	{
		int src,dst,bw;
		flow_test>>src>>dst>>bw;
		CReq* r2=new CReq(src,dst,bw);
		g.bw[i]=bw;
		g.r.push_back(r2);
	}

	//�᷽��
	for(int i=1;i<=K;i++)
	{
		g.single_flow_propose(i);
	}

	//����
	for(int i=1;i<=K;i++)
		for(int j=1;j<=K;j++)
		g.judge[i][j]=0;
	for(int i=1;i<=K;i++)
		g.judge_sum[i]=0;
	for(int i=1;i<=K;i++)
	{
		g.single_flow_evaluate(i);
		g.cost_evaluate(i);
	}

	cout<<endl;
	cout<<"************ single judge **************"<<endl;
	for(int i=1;i<=K;i++)
	{
		cout<<"flow ";
		cout.setf(ios::right);
		cout.width(3);
		cout<<i<<" judge: ";
		for(int j=1;j<=K;j++){
			cout.setf(ios::left);
			cout.width(4);
			cout<<g.judge[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
	cout<<"************ sum judge **************"<<endl;
	
	for(int i=1;i<=K;i++)
	{	
		cout<<"proposal "<<i<<" : "<<g.judge_sum[i]<<endl;
	}
	
	//voting method
	for(int i=1;i<=K;i++)
		for(int j=1;j<=K;j++)
		{
			if(g.judge[j][i]==0) {table[i][j]=10000;continue;}//�����0��˵����û�а���������
			table[i][j]=g.judge[j][i];
		}
	for(int i=1;i<=K;i++)
		ranking[i]=1;

	Voting vv(table,ranking);
	winner=vv.voting(1);
	cout<<endl;
	cout<<"schulze method : "<< winner<<endl;

	getchar();
	return 0;
}