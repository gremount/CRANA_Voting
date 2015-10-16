#include "resources.h"


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
}

int main()
{
	list<CEdge*> listEdge;
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
	g.p4();
	CReq* r1=new CReq(0,0,0);
	g.r.push_back(r1);
	ifstream flow_test("d:\\a\\CRANA_Voting\\req6.txt");
	for(int i=1;i<=K;i++)
	{
		int src,dst,bw;
		flow_test>>src>>dst>>bw;
		CReq* r2=new CReq(src,dst,bw);
		g.r.push_back(r2);
	}
	for(int i=1;i<K;i++)
	{
		g.DijkstraAlg(i);
	}
	
	getchar();
	return 0;
}