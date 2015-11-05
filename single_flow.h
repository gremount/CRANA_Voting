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

	//ϴ���㷨���������a[]
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
	//����Ҫ������·��
	DijkstraAlg(k,k);
	//��ʣ���������·��
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
		link_bw[k][v1][v2]+=band;  /////��¼���д���ı仯
		}
		else break;
	}
}

//��k���������з���������
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
			//�������Ӧ���е����ۣ���Ϊ���ں��沿�𣬾ͻ��и��ߵ���ʱ
			past=*it;
		}
	}
}

//�������������Է���������
void CGraph::cost_evaluate(int k){
	list<CEdge*>::iterator it,iend;
	iend=IncidentList.end();
	for(it=IncidentList.begin();it!=iend;it++)
		judge_sum[k] += link_bw[k][(*it)->getTail()][(*it)->getHead()];
	
}