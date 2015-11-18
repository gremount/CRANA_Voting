#ifndef FLOW_H
#define FLOW_H

#include"graph.h"
#include"common.h"
#include "res.h"
#include "LP2.h"

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
    vector<int> d, p;

	//��ʼ����ֻ��ʼ��һ�Σ�֮��������������ʱ��
	//ֻ�޸�֮ǰ�������൱��ͶƱ�Ļ�����ʩֻ����һ�Σ�ʣ�µ���ά��
	Flow(int id2, int a, int b, int c)
	{
		id=id2;src=a;dst=b;flow=c;
		adj.clear();
		path_record.clear();
		judge.clear();
		judge_sum=0;
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
		judge_sum=0;
		path_record.resize(Maxreq);
		judge.resize(Maxreq);
	}

	//���������
	void propose(VGraph &g,vector<Flow*> &flowL)
	{
		//���Լ���������dijkstra
		int dist=0,loc=dst;
		dist=dijkstra(src,dst,flow,g);
		if(dist==Inf){;}
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
			//��֤�����õ�λ�ú�֮ǰ��¼��id��ͬ
			//ͬʱҲҪע��LP�����һ����Ʒ��һ����·��
			if(i==id) reqPL.push_back(new Req(src,dst,0));
			else	  reqPL.push_back(g.reqL[i]);
		}

		LP2(&g,reqPL,path_record,id,adj);
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
		int temp=0,edge_num=0;//temp��¼·��Ȩֵ��
		for(int i=0;i<Maxreq;i++)
		{
			edge_num=flowL[i]->path_record[id]->pathL.size();
			temp=0;
			for(int j=0;j<edge_num;j++)
				temp+=flowL[i]->path_record[id]->pathL[j]->weight;
			judge[i]=temp*flow;
		}
	}

	//������ ��ʤ�ķ���
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
			src=g.adjL[s][i]->src;dst=g.adjL[s][i]->dst;
			if(flow>g.adjL[s][i]->capacity-adj[src][dst])continue;
			if (d[s] + g.adjL[s][i]->weight < d[g.adjL[s][i]->dst]){
                d[g.adjL[s][i]->dst] = d[s] + g.adjL[s][i]->weight;
                p[g.adjL[s][i]->dst] = s;
            }
		}
    }

    int FindMin(){
        set<int>::iterator it, iend;
        iend = S.end();
        int mine = Inf;
        int min_node = -1;
        for (it = S.begin(); it != iend; it++){
            if(d[*it] < mine) {
                mine = d[*it];
                min_node = *it;
            }
        }
        return min_node;
    }

    int dijkstra(int src, int dst, int flow, VGraph &g){
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

//ͳ���ۼӵ�cost
void judge_sum_function(VGraph &g, vector<Flow*> &flowL, int winner)
{
	judge_sum=0;
	for(int i=0;i<M;i++)
	{
		int src,dst,weight;
		int cost=0;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		weight=g.incL[i]->weight;
		judge_sum+=weight * flowL[winner]->adj[src][dst];
	}
}


#endif