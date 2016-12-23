#ifndef APP_H
#define APP_H

#include "common.h"
#include "graph.h"
#include "ext.h"

class APP
{
public:
	int app_id;//APP�ı��
	vector<vector<double> > adj;//��APPά�����ڽӾ��󣬼�¼��ʵ����·�ϸ���
	vector<vector<double> > adjMyFlow;//��¼�����Լ�APP����·�ϸ���
	vector<int> pathRecord;//APP����ľ��巽����·����¼,�������������⣬���Ծ�һ��·��
	vector<double> judge;//APP�����з���������
	 
	//dijkstra��Ҫ�õı���
	set<int> S;//��û���ҳ����·���Ľڵ㼯��
	vector<int> p;
	vector<double> d;
	VGraph* g;//���ﲻ�����������ã���Ϊ��������ֱ�Ӹ���ʼֵ

	//��ʼ����ֻ��ʼ��һ�Σ�֮��������������ʱ��
	//ֻ�޸�֮ǰ�������൱��ͶƱ�Ļ�����ʩֻ����һ�Σ�ʣ�µ���ά��
	APP(int app_id2,VGraph &gv)
	{
		app_id=app_id2;
		adj.clear();
		pathRecord.clear();
		judge.clear();
		adj.resize(N);
		for(int i=0;i<N;i++)
			adj[i].resize(N);
		adjMyFlow.resize(N);
		for (int i = 0; i<N; i++)
			adjMyFlow[i].resize(N);
		judge.resize(APPNUM);
		g = &gv;//����dijkstra��ѯ
		p.resize(N);
		d.resize(N);
	}

	//����ά��������µ�������
	void init()
	{
		pathRecord.clear();
		judge.clear();
		judge.resize(APPNUM);
	}

	void update(int src, Req &req){
		for (int i = 0; i < g->adjL[src].size(); i++){
			int dst = g->adjL[src][i]->dst;
			double load = adj[src][dst] + req.flow;
			//1/(c-x)�Ŷ���ʱ��ʽ��һ��������ʱ
			double newLatency = d[src] + 1 / (g->adjL[src][i]->capacity - load);

			if (load > g->adjL[src][i]->capacity)continue;			
			if (newLatency < d[dst]){
				d[dst] = newLatency;
				p[dst] = src;
			}
		}
	}

	int find_min(){
		double dmin = INF;
		int dnode = -1;
		for (int i : S){
			if (dmin > d[i]){
				dmin = d[i];
				dnode = i;
			}
		}
		return dnode;
	}

	void record_path(Req &req)
	{
		int node = req.dst;
		do{
			pathRecord.insert(pathRecord.begin(),node);
			node = p[node];
		} while (node != -1);		
	}

	void dijkstra(Req &req)
	{
		int findIt = 0;
		S.clear();
		for (int i = 0; i < N; i++){
			d[i] = INF; p[i] = INF;
			S.insert(i);
		}
		
		d[req.src] = 0; p[req.src] = -1;
		update(req.src, req); S.erase(req.src);

		while (S.size() != 0){
			int next = find_min();
			if (next == req.dst){
				//already found the best path
				findIt = 1;
				record_path(req);
				return;
			}
			update(next,req);S.erase(next); 
		}
		if (findIt == 0) cout << "no path found" << endl;
	}

	void reduce_cost(){
		;
	}


	//���������
	void propose(Req &req)
	{	
		//if this flow is mine, choose the best path for this flow
		if (req.app_id == app_id){
			dijkstra(req);
		}

		//if this flow is not mine, choose the path which will not
		//hurt my interest
		if (req.app_id != app_id){
			reduce_cost();
		}
	}

	//������ �Լ�����ķ���
	void begin_implement(VGraph &g)
	{
		;
	}
	
	//Ӧ���������з���
	void evaluate(VGraph &g, vector<APP*> &appL)
	{
		/*
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
					temp+=g.reqL[i]->flow*linearCal(appL[k]->adj[src][dst],capacity);
				}
				judge[k]+=temp;
			}
		}
		*/
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

	//������� 1/(c-x)
	double linearCal(double load, double capacity)
	{
		double util=load/capacity;
		if(util<0.3333) return load/capacity;
		else if(util<0.6667) return 3*load/capacity - 2.0/3.0;
		else if(util<0.9) return 10*load/capacity - 16.0/3.0;
		else return 70*load/capacity - 178.0/3.0;
	}

};//APP��Ľ���λ��
#endif