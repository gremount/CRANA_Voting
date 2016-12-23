#ifndef APP_VOTING_H
#define APP_VOTING_H

#include"app.h"
#include"voting.h"

void app_voting(string graph_address, string req_address, string result_address)
{
	//读入流量矩阵: (id app_id src dst flow)
	ifstream reqFile(req_address);
	int reqNum=0;
	reqFile>>reqNum;
	MAXREQ=reqNum;

	//读入图
	VGraph gv(graph_address);
	cout << "graph complete" << endl;

	//************************  初始化  ***********************
	//投票系统初始化
	double table[M2C+1][N2C+1] = {0};//投票用的输入
	int ranking[N2C+1]={0};//记录一种排序的投票人数
	for(int j=0;j<reqNum;j++)
		ranking[j]=1;//每种投票结果有1个voter,如果为2就说明该方案有得到两个voter的票
	for(int j=0;j<reqNum;j++)
		for(int k=0;k<reqNum;k++)
			table[j][k]=0;
	cout << "voting complete" << endl;

	//启动APP，并初始化
	vector<APP*> appL;//记录所有的APP
	for(int k=0;k<APPNUM;k++)
		appL.push_back(new APP(k,gv));
	for(int j=0;j<APPNUM;j++)
		appL[j]->init();
	cout << "APP init complete" << endl;

	//初始化流需求
	vector<Req*> reqL;//流需求
	reqL.clear();
	gv.reqL.clear();
	for(int j=0;j<reqNum;j++)
	{
		int id = 0, app_id = 0, a = 0, b = 0;
		double c=0;
		reqFile>>id>>app_id>>a>>b>>c;
		Req* r = new Req(id,app_id,a,b,c);
		reqL.push_back(r);
		gv.reqL.push_back(r);
	}
	cout << "read request complete" << endl;
	
	//针对每一个req进行投票
	for (int i = 0; i < reqL.size(); i++)
	{
		//************************  投票机制开始  **********************
		//提方案
		for (int j = 0; j<APPNUM; j++){
			appL[j]->propose(*reqL[i]);
			cout << "app " << j << " proposal is " << endl;
			for (int k = 0; k < appL[j]->pathRecord.size(); k++)
				cout << appL[j]->pathRecord[k] << " ";
			cout << endl;
		}
		cout << "propose complete" << endl;

		//评价方案
		for (int j = 0; j<APPNUM; j++)
			appL[j]->evaluate(*reqL[i], appL);
		cout << "evaluate complete" << endl;

		/*
		//投票算法
		for (int j = 0; j<APPNUM; j++)
		for (int k = 0; k<APPNUM; k++)
		{
			if (appL[j]->judge[k] == 0) table[j][k] = 10000;//如果是0，说明流没有摆在网络中
			else table[j][k] = appL[j]->judge[k];
		}

		int choice = 1;//选择一种投票算法
		int winner = 0;
		Voting vv(table, ranking, APPNUM, APPNUM);
		winner = vv.voting(choice);
		cout << "schulze winner = " << winner << endl;

		//如果选择方差最小的方案对满意度损伤超过了30%，就采用schulze method的投票结果
		winner = selectMinS2Proposal(gv, table, winner);
		cout << "crana winner = " << winner << endl;

		//计算投票winner满意度
		double happiness = 0;//一轮所有流的满意度和，越高越好,0<=满意度<=1
		for (int j = 0; j<APPNUM; j++)
			happiness += gv.cost_best[j] / table[j][winner];//最好抉择评分/当前抉择评分 

		//计算投票winner的应用满意度方差
		double s2_voting = 0;
		double happiness_avg = happiness / APPNUM;
		for (int j = 0; j<APPNUM; j++)
		{
			if (gv.cost_best[j] == 0) continue;
			s2_voting += (gv.cost_best[j] / table[j][winner] - happiness_avg) * (gv.cost_best[j] / table[j][winner] - happiness_avg);
		}
		s2_voting = s2_voting / APPNUM;

		//统计网络latency
		double latencyVoting = 0;
		latencyVoting = judge_sum_function(gv, appL, winner);

		
		cout <<"Voting Winner 方案的结果"<<endl;
		cout << "整体满意度： " << happiness/APPNUM << endl;
		cout << "满意度满意度方差： " << s2_voting << endl;
		cout << "多轮整体延时和: " << latencyVoting << endl;
		
		double maxUtil_Voting = 0;
		for (int j = 0; j<gv.m; j++)
		{
			int src = gv.incL[j]->src;
			int dst = gv.incL[j]->dst;
			double capacity = gv.incL[j]->capacity;
			if (maxUtil_Voting<(appL[winner]->adj[src][dst] / capacity))
				maxUtil_Voting = appL[winner]->adj[src][dst] / capacity;
		}
		cout << "max utilization rate = " << maxUtil_Voting << endl;

		//胜利的方案部署到所有应用的adj里
		appL[winner]->end_implement(gv, appL);
		cout << endl;
		*/
	}
	
	
}

#endif