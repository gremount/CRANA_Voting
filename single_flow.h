#pragma once
#include"resources.h"
//Single Flow Routing

void CGraph::single_flow_propose(int k){

	int src,dst,bw;
	src=r[k]->src;
	dst=r[k]->dst;
	bw=r[k]->bw;
	cout<<"****************************************"<<endl;
	cout<<"flow "<<k<<" give this proposal "<<endl;

	//����Ҫ������·��
	DijkstraAlg(k,k);
	//��ʣ���������·��
    for(int i=1;i<=K;i++)
	{
		if(i==k) continue;
		src=r[i]->src;
		dst=r[i]->dst;
		bw=r[i]->bw;
		DijkstraAlg(k,i);
	}
}

void CGraph::single_flow_implement(CPath* p,int k,int k2)
{
	// allocate bw  //
	list<int>::iterator it2,end2;
	end2 = (p->path).end();
	int band=r[k2]->bw;
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

//��k���������з���������
void CGraph::single_flow_evaluate(int k){
	CPath* p;
	for(int i=1;i<=K;i++)
	{
		if(i==k) p=path_record[i][1];
		else if(i<k) p=path_record[i][k];
		else p=path_record[i][k+1];
		
		list<int>::iterator it,iend;
		iend=(p->path).end();
		int past;
		past=r[k]->src;
		for(it=(p->path).begin();it!=iend;it++)
		{
			if(*it==past){continue;}
			//cout<<past<<"   &&&&&   "<<(*it)<<endl;
			judge[k][i] = judge[k][i] + (adjmatrix[past][*it]->getWeight())*bw[k];
			past=*it;
		}
	}
}
void CGraph::cost_evaluate(int k){
	list<CEdge*>::iterator it,iend;
	iend=IncidentList.end();
	for(it=IncidentList.begin();it!=iend;it++)
	{
		judge_sum[k]+=((*it)->getWeight())*link_bw[k][(*it)->getTail()][(*it)->getHead()];
	}

}