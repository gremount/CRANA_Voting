#ifndef APP_H
#define APP_H

#include"graph.h"
#include"common.h"
#include "res.h"
#include "LP_Voting.h"

class APP
{
public:
	int app_id;//APP�ı��
	vector<vector<double> > adj;//��APPά�����ڽӾ��󣬼�¼����
	vector<Path*> path_record;//APP����ľ��巽����·����¼
	vector<double> judge;//APP�����з���������
	

	//��ʼ����ֻ��ʼ��һ�Σ�֮��������������ʱ��
	//ֻ�޸�֮ǰ�������൱��ͶƱ�Ļ�����ʩֻ����һ�Σ�ʣ�µ���ά��
	APP(int app_id2)
	{
		app_id=app_id2;
		adj.clear();
		path_record.clear();
		judge.clear();
		adj.resize(N);
		for(int i=0;i<N;i++)
			adj[i].resize(N);
		path_record.resize(Maxreq);
		judge.resize(Maxreq);
	}

	//����ά��������µ�������
	void init()
	{
		path_record.clear();
		judge.clear();
		path_record.resize(Maxreq);
		judge.resize(Maxreq);
	}

	//���������
	void propose(VGraph &g,vector<APP*> &appL)
	{	
		vector<Req*> reqPL_app;//���Լ������ù滮����·��
		vector<Req*> reqPL_other;//�����������ù滮����·��
		for(int i=0;i<g.reqL.size();i++)
		{	
			if(app_id==g.reqL[i]->app_id) reqPL_app.push_back(g.reqL[i]);
			else	  reqPL_other.push_back(g.reqL[i]);
		}
		LP_Voting(&g,reqPL_app,path_record,app_id,adj);
		LP_Voting(&g,reqPL_other,path_record,app_id,adj);
		begin_implement(g);
	}

	//������ �Լ�����ķ���,����dijkstra�����·
	void begin_implement(VGraph &g)
	{
		for(int i=0;i<Maxreq;i++)
		{
			int edge_num=path_record[i]->pathL.size();
			for(int j=0;j<edge_num;j++)
			{
				int src,dst;
				src=path_record[i]->pathL[j]->src;
				dst=path_record[i]->pathL[j]->dst;
				adj[src][dst] += g.reqL[i]->flow;
			}
		}
	}
	
	//Ӧ���������з���
	void evaluate(VGraph &g, vector<APP*> &appL)
	{
		//��������APP����ķ���
		int edge_num=0;//temp��¼·��Ȩֵ��
		double temp;
		for(int k=0;k<APPNUM;k++)
		{
			judge[k]=0;
			for(int i=0;i<Maxreq;i++)
			{
				if(g.reqL[i]->app_id!=app_id){
					//judge[k]++;
					continue;
				}//�����������Լ����Ͳ��ܸ���������
				edge_num=appL[k]->path_record[i]->pathL.size();
				temp=0;
				for(int j=0;j<edge_num;j++)
				{
					int src=appL[k]->path_record[i]->pathL[j]->src;
					int dst=appL[k]->path_record[i]->pathL[j]->dst;
					int capacity=appL[k]->path_record[i]->pathL[j]->capacity;
					if(capacity-appL[k]->adj[src][dst]==0)
						temp+=g.reqL[i]->flow/(Rinf+capacity-appL[k]->adj[src][dst]);
					else
						temp+=g.reqL[i]->flow/(capacity-appL[k]->adj[src][dst]);
				}
				judge[k]+=temp;
			}
		}
	}

	//Ӧ�ò��� ��ʤ�ķ���
	void end_implement(VGraph &g,vector<APP*> &appL)
	{
		//APP��¼��ͼ������Ϣ����
		for(int k=0;k<APPNUM;k++)
		{
			if(k==app_id)continue;
			for(int i=0;i<g.incL.size();i++)
			{
				int src=g.incL[i]->src;
				int dst=g.incL[i]->dst;
				appL[k]->adj[src][dst]=appL[app_id]->adj[src][dst];
			}
		}
	}
};//APP��Ľ���λ��

//Votingͳ������latency
double judge_sum_function(VGraph &g, vector<APP*> &appL, int winner)
{
	double latencyVoting=0;
	for(int i=0;i<M;i++)
	{
		int src,dst;
		double latencyTemp;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		
		if(g.incL[i]->capacity - appL[winner]->adj[src][dst]==0)
			latencyTemp = (double)appL[winner]->adj[src][dst]/(Rinf+ g.incL[i]->capacity - appL[winner]->adj[src][dst]);
		else
			latencyTemp = (double)appL[winner]->adj[src][dst]/(g.incL[i]->capacity - appL[winner]->adj[src][dst]);
		//cout<<src<<" "<<dst<<" " <<appL[winner]->adj[src][dst]<<endl;
		latencyVoting += latencyTemp;
	}
	return latencyVoting;
}


//TEȫ���Ż���ͳ������latency
double delay_TENetworkGraph(TENetworkGraph &g)
{
	double judge_sum_LP=0;
	for(int i=0;i<M;i++)
	{
		int src,dst;
		double latencyFunc;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		if(g.adj[src][dst]==0) continue;
		if(g.incL[i]->capacity - g.adj[src][dst]==0)
			latencyFunc = (double)g.adj[src][dst]/(Rinf+ g.incL[i]->capacity - g.adj[src][dst]);
		else
			latencyFunc = (double)g.adj[src][dst]/(g.incL[i]->capacity - g.adj[src][dst]);
		judge_sum_LP += latencyFunc;
	}
	return judge_sum_LP;
}

//Delayȫ���Ż���ͳ������latency
double delay_DelayNetworkGraph(DelayNetworkGraph &g)
{
	double judge_sum_LP=0;
	for(int i=0;i<M;i++)
	{
		int src,dst;
		double latencyFunc;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		if(g.adj[src][dst]==0) continue;
		if(g.incL[i]->capacity - g.adj[src][dst]==0)
			latencyFunc = (double)g.adj[src][dst]/(Rinf+ g.incL[i]->capacity - g.adj[src][dst]);
		else
			latencyFunc = (double)g.adj[src][dst]/(g.incL[i]->capacity - g.adj[src][dst]);
		judge_sum_LP += latencyFunc;
	}
	return judge_sum_LP;
}

#endif