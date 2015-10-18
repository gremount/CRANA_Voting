#pragma once
#include"resources.h"
//Single Flow Routing

void CGraph::single_flow_propose(int k){

	int src,dst,bw;
	src=r[k]->src;
	dst=r[k]->dst;
	bw=r[k]->bw;

	//给主要流分配路径
	DijkstraAlg(k);
	//给剩余的流分配路径
    for(int i=1;i<=K;i++)
	{
		if(i==k) continue;
		src=r[i]->src;
		dst=r[i]->dst;
		bw=r[i]->bw;
		DijkstraAlg(k);
	}
}

void CGraph::single_flow_evaluate(int k){
	CPath* p;
	for(int i=1;i<=K;i++)
	{
		if(i==k) p=path_record[i][1];
		else if(i<k) p=path_record[i][k];
		else p=path_record[i][k+1];
		
		list<int>::iterator it,iend;
		iend=(p->path).end();
		for(it=(p->path).begin();it!=iend;it++)
			judge[k][i] = judge[k][i] + (adjmatrix[r[k]->src][*it]->getWeight())*bw[k];
	} 
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
		link_bw[k][v1][v2]+=band;  /////记录所有带宽的变化
		}
		else break;
	}
}