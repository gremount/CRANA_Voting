#ifndef APP_VOTING_H
#define APP_VOTING_H

#include "common.h"
#include"app.h"
#include"graph.h"
#include"voting.h"
#include "res.h"
#include "calculate_delay.h"
#include "app_voting.h"

void app_voting(string graph_address, string req_address, string path_address)
{
	VGraph gv(graph_address);
	
	vector<APP*> appL;//记录所有的APP
	vector<Req*> reqL;//流需求

	double happiness_sum=0;//voting的多轮满意度
		
	double table[M2C+1][N2C+1] = {0};//投票用的输入
	int ranking[N2C+1]={0};//记录一种排序的投票人数
	
	//读入流量矩阵: (id app_id src dst flow)
	ifstream reqFile(req_address);
	int reqNum=0;
	reqFile>>reqNum;
	int APPNUM=reqNum;
	Maxreq=reqNum;

	for(int k=0;k<APPNUM;k++)
		appL.push_back(new APP(k));

	for(int j=0;j<reqNum;j++)
		ranking[j]=1;//每种投票结果有1个voter,如果为2就说明该方案有得到两个voter的票

	
	//初始化
	for(int j=0;j<reqNum;j++)
		for(int k=0;k<reqNum;k++)
			table[j][k]=0;
			
	reqL.clear();
	gv.reqL.clear();

	for(int j=0;j<APPNUM;j++)
		appL[j]->init();
		
	

	for(int j=0;j<reqNum;j++)
	{
		int app_id=0,a=0,b=0,c=0;
		reqFile>>app_id>>a>>b>>c;
		Req* r = new Req(j,app_id,a,b,c);
		reqL.push_back(r);
		gv.reqL.push_back(r);
	}
			
	//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@
	//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@

	//算最完美方案，所有的自己的流都第一个走得到的方案
	for(int j=0;j<APPNUM;j++)
	{
		vector<Req*> reqPL_app;
		for(int i=0;i<reqL.size();i++)
		{	
			if(j==reqL[i]->app_id) reqPL_app.push_back(reqL[i]);
		}
		gv.network_delay(reqPL_app,j);
	}

	//for(int j=0;j<Maxreq;j++)
		//cout<<"gv.cost_best "<<j<<" : "<<gv.cost_best[j]<<endl;

	//提方案
	for(int j=0;j<APPNUM;j++){
		cout<<"app "<<j<<" proposal"<<endl;
		appL[j]->propose(gv,appL);
	}

	//评价方案
	for(int j=0;j<APPNUM;j++)
		appL[j]->evaluate(gv,appL);

	//投票算法
	for(int j=0;j<APPNUM;j++)
		for(int k=0;k<APPNUM;k++)
		{
			if(appL[j]->judge[k]==0) table[j][k]=10000;//如果是0，说明流没有摆在网络中
			else table[j][k]=appL[j]->judge[k];
		}
	cout<<endl<<"voting uses ";
	int choice=1;//选择一种投票算法
	int winner=0;
	Voting vv(table,ranking,APPNUM,APPNUM);
	winner=vv.voting(choice);

	//****************************   最高满意度方案和最小满意度方差方案 评价
	//计算 最高满意度方案 和 最小满意度方差方案
	double happinessVotingMax=0;//最高满意度
	double happinessVotingTemp=0;
	int happinessVotingLoc=0;
	double s2Min=Inf;//最小满意度方差
	double s2Temp=0;
	int s2Loc=0;
	double s2Min_Happiness=0;//最小满意度方差方案的满意度
		
	for(int k=0;k<APPNUM;k++)
	{
		happinessVotingTemp=0;
		for(int j=0;j<APPNUM;j++)
			happinessVotingTemp+= gv.cost_best[j]/table[j][k];//最好抉择评分/当前抉择评分
		if(happinessVotingMax<happinessVotingTemp/APPNUM){
			happinessVotingMax=happinessVotingTemp/APPNUM;
			happinessVotingLoc=k;
		}
			
		s2Temp=0;
		for(int j=0;j<APPNUM;j++)
			s2Temp+= (gv.cost_best[j]/table[j][k]-happinessVotingTemp/APPNUM) * (gv.cost_best[j]/table[j][k]-happinessVotingTemp/APPNUM);
		if(s2Min>s2Temp/APPNUM){
			s2Min=s2Temp/APPNUM;
			s2Loc=k;
			s2Min_Happiness=happinessVotingTemp/APPNUM;
		}
	}

	//计算 最高满意度方案 的 应用满意度方差
	double s2VotingMax=0;
	//最高满意度方案的应用满意度的平均值
	double happinessVotingMaxAvg=happinessVotingMax/APPNUM;
	for(int j=0;j<APPNUM;j++){
		if(gv.cost_best[j]==0)continue;//没有流量需求的APP就没有满意度的区别
		s2VotingMax+=(gv.cost_best[j]/table[j][happinessVotingLoc]-happinessVotingMaxAvg) * (gv.cost_best[j]/table[j][happinessVotingLoc]-happinessVotingMaxAvg);
	}
	s2VotingMax=s2VotingMax/APPNUM;
		
	cout <<"最高满意度方案的结果"<<endl;
	cout << "整体满意度： " << happinessVotingMax << endl;
	cout << "满意度满意度方差： " << s2VotingMax << endl;
	cout <<endl;

	cout <<"最小满意度 方差 方案的结果"<<endl;
	cout << "整体满意度： " << s2Min_Happiness << endl;
	cout << "满意度满意度方差： " << s2Min << endl;
	cout <<endl;

	//****************************   END OF 最高满意度方案和最小满意度方差方案 评价  
	//如果选择方差最小的方案对满意度损伤超过了30%，就采用schulze method的投票结果
	if(happinessVotingMax-s2Min_Happiness<=0.3) winner=s2Loc;
			
	cout<<"crana voting method : " << winner << endl;

	//计算投票winner满意度
	double happiness=0;//一轮所有流的满意度和，越高越好,0<=满意度<=1
	for(int j=0;j<APPNUM;j++)
		happiness += gv.cost_best[j]/table[j][winner];//最好抉择评分/当前抉择评分 
	happiness_sum += happiness;

	//计算投票winner的应用满意度方差
	double s2_voting=0;
	double happiness_avg=happiness/APPNUM;
	for(int j=0;j<APPNUM;j++)
	{
		if(gv.cost_best[j]==0) continue;
		s2_voting+=(gv.cost_best[j]/table[j][winner]-happiness_avg) * (gv.cost_best[j]/table[j][winner]-happiness_avg);
	}
	s2_voting=s2_voting/APPNUM;

	//统计网络latency
	double latencyVoting=0;
	latencyVoting=judge_sum_function(gv,appL,winner);

	cout <<"Voting Winner 方案的结果"<<endl;
	cout << "整体满意度： " << happiness/APPNUM << endl;
	cout << "满意度满意度方差： " << s2_voting << endl;
	cout << "多轮整体延时和: " << latencyVoting << endl;
		
	double maxUtil_Voting=0;
	for(int j=0;j<gv.m;j++)
	{
		int src=gv.incL[j]->src;
		int dst=gv.incL[j]->dst;
		double capacity=gv.incL[j]->capacity;
		if(maxUtil_Voting<(appL[winner]->adj[src][dst]/capacity))
			maxUtil_Voting=appL[winner]->adj[src][dst]/capacity;
	}
	cout<<"最大链路利用率: "<<maxUtil_Voting<<endl;

	//胜利的方案部署到所有应用的adj里
	appL[winner]->end_implement(gv,appL);
	cout<<endl;
}

#endif