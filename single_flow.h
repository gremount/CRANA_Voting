#pragma once
#include"resources.h"
//Single Flow Routing

void CGraph::single_flow_propose(int k){

	int src,dst,bw;
	src=r[k]->src;
	dst=r[k]->dst;
	bw=r[k]->bw;

	//����Ҫ������·��
	DijkstraAlg(src,dst,bw);
	//��ʣ���������·��
    for(int i=1;i<=K;i++)
	{
		if(i==k) continue;
		src=r[i]->src;
		dst=r[i]->dst;
		bw=r[i]->bw;
		DijkstraAlg(src,dst,k);
	}
}

int CGraph::single_flow_evaluate(int k){
	list<CEdge*>::iterator it,iend;
	iend=IncidentList.end();
	int sum=0;
	for(it=IncidentList.begin();it!=iend;it++)
	{
		sum+=(*it)->getWeight() * link_bw[k][(*it)->getTail()][(*it)->getHead()];
	}
	return sum;
}

void CGraph::single_flow_implement(CPath* p,int k)
{
	// allocate bw  //
	list<int>::iterator it2,end2;
	end2 = (p->path).end();
	int band=r[k]->bw;
	for(it2=(p->path).begin();it2!=end2;)
	{	
		int v1=(*it2);
		it2++;
		if(it2!=end2){
		int v2=(*it2);
		link_bw[k][v1][v2]+=band;  /////��¼���д���ı仯
		}
		else break;
	}
}