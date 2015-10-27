#pragma once
#include "resources.h"
#include "single_flow.h"
#include "voting.h"

/*
���Ҫ���Ӳ��Ե�������ֻ��Ҫ����req.txt�ļ���ע�����֣�Ȼ���޸�req_num��
ֵ�Ϳ��ԣ��м���req.txt�ļ���req_num���Ǽ�
*/

int K;
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
}

int main()
{
	list<CEdge*> listEdge;
	srand((unsigned)time(0));
	int winner = 0;//No.1
	float table[M2C+1][N2C+1] = {0};
	int ranking[N2C+1]={0};//��¼һ�������ͶƱ����

	//ͼ�ĳ�ʼ��
	ifstream test("d:\\github\\CRANA_Voting\\graph2.txt");
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
	int req_num,req_constant;
	req_num=6;
	req_constant = req_num+1;
	string s;
	s = "d:\\github\\CRANA_Voting\\req1.txt";

	//*********************big turn*****************************
	
	while(req_num){
	
		cout << endl;
		cout << "�� "<< req_constant-req_num << " �ֿ�ʼ" << endl;
	//���ֱ����ĳ�ʼ������,judge[][]�ĳ�ʼ�������۲���
	g.r.clear();
	

	//�����¼
	CReq* r1=new CReq(0,0,0);
	g.r.push_back(r1);
	
	ifstream flow_test(s);
	flow_test >> K;

	if (req_num != (req_constant-1))
	{
		for (int k = 1; k <= K; k++)
		{
			list<CEdge*>::iterator it, iend;
			iend = listEdge.end();
			for (it = listEdge.begin(); it != iend; it++)
			{
				g.link_bw[k][(*it)->getTail()][(*it)->getHead()] = g.link_bw[winner][(*it)->getTail()][(*it)->getHead()];
				//cout << (*it)->getTail() << " " << (*it)->getHead() << " " << g.link_bw[k][(*it)->getTail()][(*it)->getHead()]<<endl;
			}
		}
	}

	for (int i = 1; i <= K; i++)
		for (int j = 1; j <= K; j++)
		{
			CPath* pa = new CPath();
			g.path_record[i][j] = pa;
		}

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
		g.single_flow_propose(i,K);
	}

	//����
	for(int i=1;i<=K;i++)
		for(int j=1;j<=K;j++)
		g.judge[i][j]=0;
	for(int i=1;i<=K;i++)
		g.judge_sum[i]=0;
	for(int i=1;i<=K;i++)
	{
		//cout<<"evaluate "<<i<<endl;
		g.single_flow_evaluate(i,K);
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
			if(g.judge[i][j]==0) {table[i][j]=10000;}//�����0��˵����û�а���������
			else table[i][j]=g.judge[i][j];
		}
	for(int i=1;i<=K;i++)
		ranking[i]=1;

	cout<<"************ voting table **************"<<endl;
	for(int i=1;i<=K;i++)
	{
		cout<<"flow ";
		cout.setf(ios::right);
		cout.width(3);
		cout<<i<<" judge: ";
		for(int j=1;j<=K;j++){
			cout.setf(ios::left);
			cout.width(5);
			cout<<table[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<endl;

	int choice = 1;
	Voting vv(table,ranking,K,K);
	winner=vv.voting(choice);
	cout<<endl;
	if (choice == 1)
		cout << "schulze method : " << winner << endl;
	else if (choice == 2)
		cout << "comulative method: " << winner << endl;
	else if (choice == 3)
		cout << "copeland condorcet method: " << winner << endl;
	else
		cout << "ranked pairs method: " << winner << endl;

	//���������
	float happiness=0;//һ��������������Ⱥͣ�Խ��Խ��,0<=�����<=1
	for (int i = 1; i <= K; i++)
		happiness += table[i][i] / table[i][winner];//��þ�������/��ǰ��������
	cout << "��" << req_constant - req_num << "����������ȣ� " << happiness/K << endl;
	cout << "��" << req_constant - req_num << "���������: " << g.judge_sum[winner] << endl;
	req_num--;
	s[26]++;//req2-->req3
	}

	getchar();
	return 0;
}