#ifndef APP_H
#define APP_H

#include"graph.h"
#include"common.h"
#include "ext.h"
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
		if(app_id==0)
		{
			for(int i=0;i<g.reqL.size();i++)
				reqPL_other.push_back(g.reqL[i]);
			LP_Voting(&g,reqPL_other,path_record,app_id,adj);
			begin_implement(g);
		}
		else
		{
			for(int i=0;i<g.reqL.size();i++)
			{	
				if(app_id==g.reqL[i]->app_id) reqPL_app.push_back(g.reqL[i]);
				else	  reqPL_other.push_back(g.reqL[i]);
			}
			LP_Voting(&g,reqPL_app,path_record,app_id,adj);
			LP_Voting(&g,reqPL_other,path_record,app_id,adj);
			begin_implement(g);
		}
	}

	//������ �Լ�����ķ���
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
					temp+=g.reqL[i]->flow*linearCal(appL[k]->adj[src][dst],capacity);
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