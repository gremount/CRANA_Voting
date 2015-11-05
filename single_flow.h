#pragma once
#include"resources.h"
//Single Flow Routing

void CGraph::single_flow_propose(int k,int K){

	int src,dst,bw;
	src=r[k]->src;
	dst=r[k]->dst;
	bw=r[k]->bw;
	//cout<<"****************************************"<<endl;
	//cout<<"flow "<<k<<" give this proposal "<<endl;

	//洗牌算法，结果存在a[]
	int a[KC+2];
    for(int i=1;i<=K;i++)
       a[i]=i;
    
    int kk=0;
    int temp=0;
    int randnum;
    
    while(1)
    {
     if(kk>=K-1) break;
     randnum=rand()%(K-kk)+kk+1;      
     temp=a[kk+1];
     a[kk+1]=a[randnum];
     a[randnum]=temp;
     kk++;
    }
	/*
	cout<<"flow routing sequence: ";
	for(int i=1;i<=K;i++)
       cout<<a[i]<<" ";
	cout<<endl;
	*/
	//给主要流分配路径
	DijkstraAlg(k,k);
	//给剩余的流分配路径
    for(int i=1;i<=K;i++)
	{
		if(a[i]==k) continue;
		src=r[a[i]]->src;
		dst=r[a[i]]->dst;
		bw=r[a[i]]->bw;
		DijkstraAlg(k,a[i]);
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
		link_bw[k][v1][v2]+=band;  /////记录所有带宽的变化
		}
		else break;
	}
}

//第k条流对所有方案的评价
void CGraph::single_flow_evaluate(int k,int K){
	CPath* p;
	for(int i=1;i<=K;i++)
	{
		p=path_record[i][k];
		list<int>::iterator it,iend;
		iend=(p->path).end();
		int past;
		past=r[k]->src;
		int band;
		band=r[k]->bw;
		for(it=(p->path).begin();it!=iend;it++)
		{
			if(*it==INF) {judge[k][i]=INF;break;}
			if(*it==past){continue;}
			judge[k][i] = judge[k][i] + band + link_bw_backup[past][*it];
			//这个不是应该有的评价，因为放在后面部署，就会有更高的延时
			past=*it;
		}
	}
}

//基于所有流，对方案的评价
void CGraph::cost_evaluate(int k){
	list<CEdge*>::iterator it,iend;
	iend=IncidentList.end();
	for(it=IncidentList.begin();it!=iend;it++)
		judge_sum[k] += link_bw[k][(*it)->getTail()][(*it)->getHead()];
	
}