#ifndef FLOW_H
#define FLOW_H

#include"graph.h"
#include"common.h"
#include "res.h"
#include "LP_Voting.h"
#include "voter.h"

class Flow: public Voter
{
public:
	//int id;//��id��������
	int src,dst,flow;//Դ��ַ��Ŀ�ĵ�ַ, ����С
	//vector<vector<double> > adj;//����ά�����ڽӾ��󣬼�¼����
	//vector<Path*> path_record;//���巽����·����¼
	//vector<double> judge;//���������з���������
	

	//dijkstra��Ҫ�õ��ı���
	set<int> S, V;
	vector<double> d;
	vector<int> p;
	//��ʼ����ֻ��ʼ��һ�Σ�֮��������������ʱ��
	//ֻ�޸�֮ǰ�������൱��ͶƱ�Ļ�����ʩֻ����һ�Σ�ʣ�µ���ά��
	Flow(int id2, int id_kind2, int a, int b, int c):Voter(id2, id_kind2)
	{
		src=a;dst=b;flow=c;
		d.resize(N);
		p.resize(N);
	}

	//����ά��������µ�������
	void modify(int a, int b, int c)
	{
		src=a;dst=b;flow=c;
		path_record.clear();
		judge.clear();
		path_record.resize(Maxreq);
		judge.resize(Maxreq);
	}

	//���������
	void propose(VGraph &g,vector<Voter*> &flowL)
	{
		//���Լ���������dijkstra
		int loc=dst;
		double dist=0;
		dist=dijkstra(src,dst,flow,g);
		if(dist==Inf){cout<<"*********** no path *********"<<endl;}
		//û��·�����԰��ţ���ͶƱ�㷨������table�����ӳͷ�
		else{
			//��·����¼����
			Path* path = new Path();
			while(p[loc]!=-1){
				path->pathL.push_back(g.adj[p[loc]][loc]);
				loc=p[loc];
			}
			path_record[id]=path;

			//��������·��
			int edge_num=path->pathL.size();
			for(int j=0;j<edge_num;j++)
			{
				int src,dst;
				src=path_record[id]->pathL[j]->src;
				dst=path_record[id]->pathL[j]->dst;
				adj[src][dst] += g.reqL[id]->flow;
			}
		}

		//�����������ù滮����·��
		vector<Req*> reqPL;
		for(int i=0;i<g.reqL.size();i++)
		{	
			if(i==id) continue;
			else	  reqPL.push_back(g.reqL[i]);
		}

		LP_Voting(&g,reqPL,path_record,id,adj);
		begin_implement(g);

	}

	//������ �Լ�����ķ���,����dijkstra�����·
	void begin_implement(VGraph &g)
	{
		for(int i=0;i<Maxreq;i++)
		{
			if(i==id) continue;
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
	
	//���������з���
	void evaluate(VGraph &g, vector<Voter*> &voterL)
	{
		//���������з���
		int edge_num=0;//temp��¼·����ʱ��
		double temp;
		for(int i=0;i<voterL.size();i++)
		{
			edge_num=voterL[i]->path_record[id]->pathL.size();
			temp=0;
			for(int j=0;j<edge_num;j++)
			{
				int src=voterL[i]->path_record[id]->pathL[j]->src;
				int dst=voterL[i]->path_record[id]->pathL[j]->dst;
				int capacity=g.adj[src][dst]->capacity;
				
				temp+=flow/((double)capacity - voterL[i]->adj[src][dst] + 1);
				//cout<<"evaluate---"<<temp<<" "<<flow<<" "<<capacity<<" "<<voterL[i]->adj[src][dst]<<endl;
			}
			judge[i]=temp;
			//if(judge[i]==0) judge[i]=Maxpath*flow;//û��·�����԰��ţ���Ҫ���ӳͷ�
		}
	}

	//������ ��ʤ�ķ���, ֱ����main.cpp��ʵ��Ҳ����
	void end_implement(VGraph &g,int winner, vector<Voter*> &voterL)
	{
		//Flow��¼��ͼ������Ϣ����
		for(int i=0;i<voterL.size();i++)
		{
			if(i==winner) continue;
			for(int j=0;j<g.incL.size();j++)
			{
				int src,dst;
				src=g.incL[j]->src;
				dst=g.incL[j]->dst;
				adj[src][dst] = voterL[winner]->adj[src][dst];
			}
		}
	}

	void Update(int s,int flow, VGraph &g){
		double x;
        for (int i = 0; i < g.adjL[s].size();i++)
		{
			int src,dst;
			src=g.adjL[s][i]->src;dst=g.adjL[s][i]->dst;
			x=adj[src][dst]+flow;
			if(x>g.adjL[s][i]->capacity)continue;
			if (d[s] + flow/(g.adjL[s][i]->capacity - x + 1) < d[dst]){
                d[dst] = d[s] + flow/(g.adjL[s][i]->capacity - x + 1);
                p[dst] = s;
            }
		}
    }

    int FindMin(){
        set<int>::iterator it, iend;
        iend = S.end();
        double mine = Inf;
        int min_node = -1;
        for (it = S.begin(); it != iend; it++){
            if(d[*it] < mine) {
                mine = d[*it];
                min_node = *it;
            }
        }
        return min_node;
    }

    double dijkstra(int src, int dst, int flow, VGraph &g){
        S.clear();
        V.clear();
        for (int i = 0; i < N; i++)
        {
            S.insert(i);
            d[i] = Inf;
            p[i] = -2;
        }
        d[src] = 0; p[src] = -1;
        Update(src,flow,g);
        S.erase(src);
        V.insert(src);
        while (S.size() != 0)
        {
            int mind;
            mind = FindMin();
            if (mind == dst) return d[mind];
			if (mind==-1) break;//û��·�ɴ�
            Update(mind,flow,g);
            S.erase(mind);
            V.insert(mind);
        }
		return Inf;//û��·�ɴ�
    }
};

//Votingͳ������latency
double judge_sum_function(VGraph &g, vector<Voter*> &voterL, int winner)
{
	double latencyVoting=0;
	for(int i=0;i<g.incL.size();i++)
	{
		int src,dst;
		double latencyTemp=0;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		if(voterL[winner]->adj[src][dst]==0) continue;//������ʱ������1+x2/(c-x1)�����Լ�ʹû��������
		//��Ȼ����ͳ����������ʱ�����������ж�һ����������Щ����
		latencyTemp = (double)voterL[winner]->adj[src][dst]/(1+ g.incL[i]->capacity - voterL[winner]->adj[src][dst]);
		latencyVoting += latencyTemp;
	}
	return latencyVoting;
}


//LPͳ������latency
double judge_sum_LP_function(PGraph &g)
{
	double judge_sum_LP=0;
	for(int i=0;i<g.incL.size();i++)
	{
		int src,dst;
		double latencyFunc=0;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		if(g.adj[src][dst]==0) continue;
		latencyFunc = (double)g.adj[src][dst]/(1+ g.incL[i]->capacity - g.adj[src][dst]);
		judge_sum_LP += latencyFunc;
	}
	return judge_sum_LP;
}

#endif