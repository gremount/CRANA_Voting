#ifndef APP_H
#define APP_H

#include"graph.h"
#include"common.h"
#include "ext.h"
#include "LP_Voting.h"

class APP
{
public:
	int app_id;//APP的编号
	vector<vector<double> > adj;//该APP维护的邻接矩阵，记录负载
	vector<Path*> path_record;//APP提出的具体方案：路径记录
	vector<double> judge;//APP对所有方案的评价

	//初始化，只初始化一次，之后其他需求来的时候，
	//只修改之前参数，相当于投票的基础设施只建立一次，剩下的是维护
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

	//流的维护，针对新的流需求
	void init()
	{
		path_record.clear();
		judge.clear();
		path_record.resize(Maxreq);
		judge.resize(Maxreq);
	}

	//流提出方案
	void propose(VGraph &g,vector<APP*> &appL)
	{	
		vector<Req*> reqPL_app;//对自己的流用规划计算路径
		vector<Req*> reqPL_other;//对其他的流用规划计算路径
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

	//流部署 自己提出的方案
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
	
	//应用评价所有方案
	void evaluate(VGraph &g, vector<APP*> &appL)
	{
		//评价所有APP提出的方案
		int edge_num=0;//temp记录路径权值和
		double temp;
		for(int k=0;k<APPNUM;k++)
		{
			judge[k]=0;
			for(int i=0;i<Maxreq;i++)
			{
				if(g.reqL[i]->app_id!=app_id){
					//judge[k]++;
					continue;
				}//该流不属于自己，就不管该流的利益
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

	//应用部署 获胜的方案
	void end_implement(VGraph &g,vector<APP*> &appL)
	{
		//APP记录的图负载信息更新
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

	//线性拟合 1/(c-x)
	double linearCal(double load, double capacity)
	{
		double util=load/capacity;
		if(util<0.3333) return load/capacity;
		else if(util<0.6667) return 3*load/capacity - 2.0/3.0;
		else if(util<0.9) return 10*load/capacity - 16.0/3.0;
		else return 70*load/capacity - 178.0/3.0;
	}
};//APP类的结束位置



#endif