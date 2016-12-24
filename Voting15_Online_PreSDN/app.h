#ifndef APP_H
#define APP_H

#include "common.h"
#include "graph.h"
#include "ext.h"

class APP
{
public:
	int app_id;//APP�ı��
	vector<vector<double> > adjMyFlow;//��¼�����Լ�APP����·�ϸ���
	vector<int> pathRecord;//APP����ľ��巽����·����¼,�������������⣬���Ծ�һ��·��
	vector<double> judge;//APP�����з���������
	 
	//dijkstra��Ҫ�õı���
	set<int> S;//��û���ҳ����·���Ľڵ㼯��
	vector<int> p;
	vector<double> d;
	VGraph* gv;//���ﲻ�����������ã���Ϊ��������ֱ�Ӹ���ʼֵ

	//��ʼ����ֻ��ʼ��һ�Σ�֮��������������ʱ��
	//ֻ�޸�֮ǰ�������൱��ͶƱ�Ļ�����ʩֻ����һ�Σ�ʣ�µ���ά��
	APP(int app_id2,VGraph &gv2)
	{
		app_id=app_id2;
		pathRecord.clear();
		judge.clear();
		adjMyFlow.resize(N);
		for (int i = 0; i<N; i++)
			adjMyFlow[i].resize(N);
		judge.resize(APPNUM);
		gv = &gv2;//����dijkstra��ѯ
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
		for (int i = 0; i < gv->adjL[src].size(); i++){
			int dst = gv->adjL[src][i]->dst;
			double load = gv->adj[src][dst] + req.flow;
			//1/(c-x)�Ŷ���ʱ��ʽ��һ��������ʱ
			double newLatency = d[src] + 1 / (gv->adjL[src][i]->capacity - load);

			if (load > gv->adjL[src][i]->capacity)continue;
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
			update(next, req);S.erase(next); 
		}
		if (findIt == 0) cout << "no path found" << endl;
	}

	//ͨ�����⴦����ͼ���ڼ��ٶ��Լ���Ӱ���£�ѡһ�����·������
	void reduce_cost(Req &req){
		for (int i = 0; i < N; i++){
			for (int j = 0; j < N; j++){
				gv->adj[i][j] += adjMyFlow[i][j];
			}
		}
		dijkstra(req);
		
		//��ԭ֮ǰ������״̬
		for (int i = 0; i < N; i++){
			for (int j = 0; j < N; j++){
				gv->adj[i][j] -= adjMyFlow[i][j];
			}
		}
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
		else{
			reduce_cost(req);
		}
	}

	//������ �Լ�����ķ���
	void begin_implement(VGraph &g)
	{
		;
	}
	
	//Ӧ���������з���
	void evaluate(Req &req, vector<APP*> &appL)
	{
		/*if this flow is mine, calculate the delay of this flow*/
		if (req.app_id == app_id){
			for (int i = 0; i < appL.size(); i++){
				double delaySum = 0;
				for (int j = 0; j < appL[i]->pathRecord.size()-1; j++){
					int tail = appL[i]->pathRecord[j];
					int head = appL[i]->pathRecord[j + 1];
					//delay=flow/(capacity-load)ԽСԽ��
					delaySum += req.flow / 
						(gv->adjM[tail][head]->capacity - gv->adj[tail][head] - req.flow);
				}
				judge[i] = delaySum;
			}
		}
		/*if this flow is not mine, calculate the effect to me 
		which is due to this flow*/
		else{
			for (int i = 0; i < appL.size(); i++){
				double effect = 0;
				for (int j = 0; j < appL[i]->pathRecord.size() - 1; j++){
					int tail = appL[i]->pathRecord[j];
					int head = appL[i]->pathRecord[j + 1];
					//effect=myLoad/(capacity-load��ԽСԽ��
					effect = appL[app_id]->adjMyFlow[tail][head] /
						(gv->adjM[tail][head]->capacity - gv->adj[tail][head] - req.flow);
				}
				judge[i] = effect;
			}
		}
	}

};//APP��Ľ���λ��
#endif