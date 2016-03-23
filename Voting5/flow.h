#ifndef FLOW_H
#define FLOW_H

#include"graph.h"
#include"common.h"
#include "res.h"
#include "LP_Voting.h"

class Flow
{
public:
	int id;//��id��������
	int src,dst,flow;//Դ��ַ��Ŀ�ĵ�ַ������С
	vector<vector<int> > adj;//����ά�����ڽӾ��󣬼�¼����
	vector<Path*> path_record;//���巽����·����¼
	vector<int> judge;//���������з���������
	

	//dijkstra��Ҫ�õ��ı���
	set<int> S, V;
    vector<int> p;
	vector<int> d;

	//��ʼ����ֻ��ʼ��һ�Σ�֮��������������ʱ��
	//ֻ�޸�֮ǰ�������൱��ͶƱ�Ļ�����ʩֻ����һ�Σ�ʣ�µ���ά��
	Flow(int id2, int a, int b, int c)
	{
		id=id2;src=a;dst=b;flow=c;
		adj.clear();
		path_record.clear();
		judge.clear();
		adj.resize(N);
		for(int i=0;i<N;i++)
			adj[i].resize(N);
		path_record.resize(Maxreq);
		judge.resize(Maxreq);
		d.resize(N);
        p.resize(N);
	}

	//����ά��������µ�������
	void flow_modify(int a, int b, int c)
	{
		src=a;dst=b;flow=c;
		path_record.clear();
		judge.clear();
		path_record.resize(Maxreq);
		judge.resize(Maxreq);
	}

	//���������
	void propose(VGraph &g,vector<Flow*> &flowL)
	{
		//���Լ���������dijkstra
		int loc=dst;
		int dist=0;
		dist=dijkstra(src,dst,flow,g);
		if(dist==Inf){cout<<"*********** no path *********"<<endl;}
		//û��·�����԰��ţ���evaluate���Ҫ���ӳͷ�
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
				adj[src][dst] += flow;
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
	void evaluate(VGraph &g, vector<Flow*> &flowL)
	{
		//���������з���
		int temp=0;
		int edge_num=0;//temp��¼·��Ȩֵ��
		for(int i=0;i<Maxreq;i++)
		{
			edge_num=flowL[i]->path_record[id]->pathL.size();
			temp=Inf;
			for(int j=0;j<edge_num;j++)
			{
				int src=flowL[i]->path_record[id]->pathL[j]->src;
				int dst=flowL[i]->path_record[id]->pathL[j]->dst;
				int capacity=flowL[i]->path_record[id]->pathL[j]->capacity;
				if(temp>capacity-flowL[i]->adj[src][dst]) temp=capacity-flowL[i]->adj[src][dst];
			}
			judge[i]=temp*flow;
			if(judge[i]==0) judge[i]=0;//û��·�����԰��ţ���Ҫ���ӳͷ�
		}
	}

	//������ ��ʤ�ķ���, ֱ����main.cpp��ʵ��Ҳ����
	void end_implement(VGraph &g,int winner, vector<Flow*> &flowL)
	{
		//Flow��¼��ͼ������Ϣ����
		for(int i=0;i<Maxreq;i++)
		{
			if(i==winner) continue;
			int edge_num=flowL[winner]->path_record[i]->pathL.size();
			for(int j=0;j<edge_num;j++)
			{
				int src,dst;
				src=flowL[winner]->path_record[i]->pathL[j]->src;
				dst=flowL[winner]->path_record[i]->pathL[j]->dst;
				adj[src][dst] = flowL[winner]->adj[src][dst];
			}
		}
	}

	void Update(int s,int flow, VGraph &g){
        for (int i = 0; i < g.adjL[s].size();i++){
			int src,dst;
			src=g.adjL[s][i]->src;dst=g.adjL[s][i]->dst;//����src = s
			
			if(flow > g.adjL[src][i]->capacity - adj[src][dst])continue;//��link�޷�ͨ������
			
			int temp;//link[i][j]����ͨ������������Ĵ���
			if(d[src] > g.adjL[src][i]->capacity-adj[src][dst]) temp=g.adjL[src][i]->capacity-adj[src][dst];//ˮ������
			else temp=d[src];//ˮԴ����(��src���·����������)
			
			if(temp>d[dst]) {d[dst]=temp;p[dst]=src;}//����ӵ�и�������·������
			else d[dst]=d[dst];//��Ϊ���и����·���Ե���dst���������ﲻ����
		}
    }

    int FindMax(){
        set<int>::iterator it, iend;
        iend = S.end();
        int maxe = 0;
        int max_node = -1;
        for (it = S.begin(); it != iend; it++){
            if(d[*it] > maxe) {
                maxe = d[*it];
                max_node = *it;
            }
        }
        return max_node;
    }

    int dijkstra(int src, int dst, int flow, VGraph &g){
        S.clear();
        V.clear();
        for (int i = 0; i < N; i++)
        {
            S.insert(i);
            d[i] = 0;
            p[i] = -2;
        }
        d[src] = Inf; p[src] = -1;
        Update(src,flow,g);
        S.erase(src);
        V.insert(src);
        while (S.size() != 0)
        {
            int mind;
            mind = FindMax();
            if (mind == dst) return d[mind];
			if (mind==-1) break;//û��·�ɴ�
            Update(mind,flow,g);
            S.erase(mind);
            V.insert(mind);
        }
		return 0;//û��·�ɴ�
    }
};

//Votingͳ������latency
double judge_sum_function(VGraph &g, vector<Flow*> &flowL, int winner)
{
	double latencyVoting=0;
	for(int i=0;i<M;i++)
	{
		int src,dst;
		double latencyTemp;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		if(flowL[winner]->adj[src][dst]==0) continue;//������ʱ������x1/(c-x1)�����Լ�ʹû��������
		//��Ȼ����ͳ����������ʱ�����������ж�һ����������Щ����
		latencyTemp = (double)flowL[winner]->adj[src][dst]/(1+ g.incL[i]->capacity - flowL[winner]->adj[src][dst]);
		latencyVoting += latencyTemp;
	}
	return latencyVoting;
}


//LPͳ������latency
double judge_sum_LP_function(PGraph &g, vector<Flow*> &flowL)
{
	double judge_sum_LP=0;
	for(int i=0;i<M;i++)
	{
		int src,dst;
		double latencyFunc;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		if(g.adj[src][dst]==0) continue;
		latencyFunc = (double)g.adj[src][dst]/(1+ g.incL[i]->capacity - g.adj[src][dst]);
		judge_sum_LP += latencyFunc;
	}
	return judge_sum_LP;
}

#endif