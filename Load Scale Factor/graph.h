#ifndef _GRAPH_H
#define _GRAPH_H

#include "common.h"

class Edge
{
private:
	int src,dst,weight,capacity; //私有变量是不允许类外函数访问的

public:
	int flow;//记录在这条边上经过的流

	//一般由源地址，目的地址，权值	和容量来定义一条边
	Edge(int src2, int dst2, int weight2, int capacity2){
		src=src2; dst=dst2; weight=weight2; capacity=capacity2;flow=0;
	}

	//方便读取私有变量
	int get_src() {
		return src;
	}
	int get_dst() {
		return dst;
	}
	int get_weight(){
		return weight;
	}
	int get_capacity(){
		return capacity;
	}
};

class Graph
{
private:
	int node_num;
	int edge_num;// 有向边的数量

public:
	vector<Edge*> incL;//记录所有边
	vector<vector<Edge*> > adjL;//记录顶点关联的所有边
	vector<vector<Edge*> > adjM;//记录邻接矩阵，元素为边
	vector<vector<int> > tm;//traffic matrix
	vector<vector<int> > d;//距离标记
	vector<vector<int> > p;//最短路前驱节点

	Graph(char* inputFile)
	{
		ifstream in_file(inputFile);
		in_file>>node_num>>edge_num;

		adjL.resize(node_num);

		adjM.resize(node_num);
		for(int i=0;i<node_num;i++)
			adjM[i].resize(node_num);

		tm.resize(node_num);
		for(int i=0;i<node_num;i++)
			tm[i].resize(node_num);

		d.resize(node_num);
		for(int i=0;i<node_num;i++)
			d[i].resize(node_num);
		p.resize(node_num);
		for(int i=0;i<node_num;i++)
			p[i].resize(node_num);

		for(int i=0;i<edge_num;i++)
		{
			int src=0, dst=0, weight=0, capacity=0;
			in_file>>src>>dst>>weight>>capacity;
			Edge* e = new Edge(src,dst,weight,capacity);

			incL.push_back(e);
			adjL[src].push_back(e);
			adjM[src][dst]=e;
		}
		cout<<"graph init completed"<<endl;
	}

	int get_node_num() {return node_num;}
	int get_edge_num() {return edge_num;}

	//随机产生流量矩阵
	void tm_gen(int m)
	{
		for(int i=0;i<node_num;i++)
			for(int j=0;j<node_num;j++)
			{
				if(i==j) continue;
				tm[i][j]=rand()%(m + 1);
				//cout<<i<<"->"<<j<<" "<<tm[i][j]<<endl;
			}
		//cout<<"random flow init"<<endl;
	}

	//Floyd-Warshall算法
	void floyd()
	{
		for(int i=0;i<node_num;i++)
			for(int j=0;j<node_num;j++)
			{
				d[i][j]=INF;
				p[i][j]=-1;
			}
		for(int i=0;i<node_num;i++)
			d[i][i]=0;
		for(int i=0;i<incL.size();i++)
		{
			int src=incL[i]->get_src();
			int dst=incL[i]->get_dst();
			d[src][dst]=1;//hop-counted shortest path
			p[src][dst]=src;
		}
		//cout<<"floyd init"<<endl;

		//dynamic programming
		for(int k=0;k<node_num;k++)
			for(int i=0;i<node_num;i++)
				for(int j=0;j<node_num;j++)
				{
					//if(i==j) continue;
					if(d[i][j]>d[i][k]+d[k][j])
					{
						d[i][j]=d[i][k]+d[k][j];
						p[i][j]=p[k][j];
					}
				}

		//cout<<"floyd completed"<<endl;
	}

	//flow implement
	void implement()
	{
		for(int i=0;i<node_num;i++)
			for(int j=0;j<node_num;j++)
			{
				if(i==j || tm[i][j]==0) continue;
				int front=j;
				int back=0;
				while(1)
				{
					back=front;
					front=p[i][front];
					adjM[front][back]->flow += tm[i][j];
					if(front==i) break;
				}
			}
		//cout<<"flows have been implemented"<<endl;
	}

	//calculate maximum utilization rate
	double te_cal()
	{
		double te=0;
		for(int i=0;i<incL.size();i++)
		{
			double temp=(double)incL[i]->flow/incL[i]->get_capacity();
			if(temp>te) te=temp;
		}
		//cout<<"te is "<<te<<endl;
		return te;
	}

	//clean data structure
	void clean_ds()
	{
		for(int i=0;i<incL.size();i++)
		{
			incL[i]->flow=0;
		}
		for(int i=0;i<node_num;i++)
			for(int j=0;j<node_num;j++)
			{
				d[i][j]=0;
				p[i][j]=0;
			}
	}

};

#endif