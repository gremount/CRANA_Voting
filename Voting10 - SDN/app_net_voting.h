#ifndef APP_VOTING_H
#define APP_VOTING_H

#include"app.h"
#include"voting.h"

//线性拟合 1/(c-x)
double linearCal(double load, double capacity)
{
	double util=load/capacity;
	if(util<0.3333) return load/capacity;
	else if(util<0.6667) return 3*load/capacity - 2.0/3.0;
	else if(util<0.9) return 10*load/capacity - 16.0/3.0;
	else return 70*load/capacity - 178.0/3.0;
}

//Voting统计网络latency
double judge_sum_function(VGraph &g, vector<APP*> &appL, int winner)
{
	double latency=0;
	for(int i=0;i<M;i++)
	{
		int src,dst;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		double x=appL[winner]->adj[src][dst];
		double capacity= g.incL[i]->capacity;
		latency += x*linearCal(x,capacity);
	}
	return latency;
}

int selectMinS2Proposal(VGraph gv, double table[][M2C+1], int winner)
{
	//****************************   最高满意度方案和最小满意度方差方案 评价
	//计算 最高满意度方案 和 最小满意度方差方案
	double happinessVotingMax=0;//最高满意度
	double happinessVotingTemp=0;
	int happinessVotingLoc=0;
	double s2Min=Inf;//最小满意度方差
	double s2Temp=0;
	int s2Loc=0;//最小方差的方案
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
	/*
	cout <<"最高满意度方案的结果"<<endl;
	cout << "整体满意度： " << happinessVotingMax << endl;
	cout << "满意度满意度方差： " << s2VotingMax << endl;
	cout <<endl;

	cout <<"最小满意度 方差 方案的结果"<<endl;
	cout << "整体满意度： " << s2Min_Happiness << endl;
	cout << "满意度满意度方差： " << s2Min << endl;
	cout <<endl;
	*/
	//如果选择方差最小的方案对满意度损伤超过了30%，就采用schulze method的投票结果
	if(happinessVotingMax-s2Min_Happiness<=0.3) return s2Loc;
	else return winner;
	//****************************   END OF 最高满意度方案和最小满意度方差方案 评价  
}

void app_net_voting(string graph_address, string req_address, string path_address)
{
	//初始化流需求
	vector<Req*> reqL;//流需求
	reqL.clear();

	//读入流量矩阵: (id app_id src dst flow)
	ifstream infile(req_address);
	string str;
	int cnt=0;
	getline(infile,str);
	vector<double> e8;
	e8.resize(8);
	while(infile!=NULL){
		for(int i=0;i<8;i++)
			infile>>e8[i];
		cnt++;
		int app_id=e8[0];
		int src=e8[3];
		int dst=e8[5];
		double flow=e8[7];
		Req* r = new Req(app_id,app_id,src,dst,flow);
		reqL.push_back(r);
	}
	cnt--;

	int reqNum=cnt;
	APPNUM=reqNum+9;
	REQNUM=reqNum;

	cout<<reqNum<<endl;

	//读入图
	VGraph gv(graph_address);
	gv.reqL.clear();
	for(int i=0;i<REQNUM;i++)
		gv.reqL.push_back(reqL[i]);
	
	//************************  初始化  ***********************
	//投票系统初始化
	double table[M2C+1][N2C+1] = {0};//投票用的输入
	int ranking[N2C+1]={0};//记录一种排序的投票人数
	for(int j=0;j<APPNUM;j++)
		ranking[j]=1;//每种投票结果有1个voter,如果为2就说明该方案有得到两个voter的票
	ranking[0]=REQNUM/2;//网络投票者的票数为流需求数量的一半
	for(int j=0;j<REQNUM;j++)
		for(int k=0;k<APPNUM;k++)
			table[j][k]=0;

	//启动APP，并初始化
	vector<APP*> appL;//记录所有的APP
	for(int k=0;k<APPNUM;k++)
		appL.push_back(new APP(k));
	for(int j=0;j<APPNUM;j++)
		appL[j]->init();
	
	//************************  投票机制开始  **********************
	//算最完美方案，所有的自己的流都第一个走得到的方案
	for(int j=0;j<APPNUM;j++)
	{
		vector<Req*> reqPL_app;
		for(int i=0;i<reqL.size();i++)
		{	
			if(j==reqL[i]->app_id) reqPL_app.push_back(reqL[i]);
		}
		gv.network_delay_lp(reqPL_app,j);
	}

	//for(int j=0;j<Maxreq;j++)
		//cout<<"gv.cost_best "<<j<<" : "<<gv.cost_best[j]<<endl;

	//提方案
	for(int j=0;j<APPNUM;j++){
		cout<<"app "<<j<<" proposal"<<endl;
		appL[j]->propose(gv,appL);
	}

	//评价方案
	for(int j=0;j<REQNUM;j++)
		appL[j]->evaluate(gv,appL);

	//投票算法
	for(int j=0;j<REQNUM;j++)
		for(int k=0;k<APPNUM;k++)
		{
			if(appL[j]->judge[k]==0) table[j][k]=1000000000;//如果是0，说明流没有摆在网络中
			else table[j][k]=appL[j]->judge[k];
		}
	
	int choice=1;//选择一种投票算法
	int winner=0;
	Voting vv(table,ranking,APPNUM,APPNUM);
	winner=vv.voting(choice);
	cout<<"schulze winner = "<<winner<<endl;

	//如果选择方差最小的方案对满意度损伤超过了30%，就采用schulze method的投票结果
	//winner=selectMinS2Proposal(gv, table, winner);
	cout<<"crana winner = "<<winner<<endl;
	
	ofstream pathFile(path_address);
	pathFile<<"id	path"<<endl;
	for(int i=0;i<REQNUM;i++)
	{
		pathFile<<i<<" ";
		vector<int> realPath;
		int src=reqL[i]->src;
		Edge* empty_edge=new Edge(-1,-1,-1,-1,-1,-1,-1);
		pathFile<<reqL[i]->src<<"_1,";
		while(src!=reqL[i]->dst)
		{
			for(int j=0;j<appL[winner]->path_record[i].size();j++)
				if(appL[winner]->path_record[i][j]->src==src){
					
					int src2=appL[winner]->path_record[i][j]->src;
					int srcPort=appL[winner]->path_record[i][j]->srcPort;
					int dst=appL[winner]->path_record[i][j]->dst;
					int dstPort=appL[winner]->path_record[i][j]->dstPort;
					pathFile<<src2<<"_"<<srcPort<<","<<dst<<"_"<<dstPort<<",";

					src=appL[winner]->path_record[i][j]->dst;
					appL[winner]->path_record[i][j]=empty_edge;
					break;
				}
		}
		pathFile<<reqL[i]->dst<<"_1"<<endl;
	}

	//计算投票winner满意度
	double happiness=0;//一轮所有流的满意度和，越高越好,0<=满意度<=1
	for(int j=0;j<REQNUM;j++)
		happiness += gv.cost_best[j]/table[j][winner];//最好抉择评分/当前抉择评分 

	//计算投票winner的应用满意度方差
	/*
	double s2_voting=0;
	double happiness_avg=happiness/REQNUM;
	for(int j=0;j<REQNUM;j++)
	{
		if(gv.cost_best[j]==0) continue;
		s2_voting+=(gv.cost_best[j]/table[j][winner]-happiness_avg) * (gv.cost_best[j]/table[j][winner]-happiness_avg);
	}
	s2_voting=s2_voting/REQNUM;
	*/

	//统计网络latency
	double latencyVoting=0;
	latencyVoting=judge_sum_function(gv,appL,winner);

	/*
	cout <<"Voting Winner 方案的结果"<<endl;
	cout << "整体满意度： " << happiness/APPNUM << endl;
	cout << "满意度满意度方差： " << s2_voting << endl;
	cout << "多轮整体延时和: " << latencyVoting << endl;
	*/
	double maxUtil_Voting=0;
	for(int j=0;j<gv.m;j++)
	{
		int src=gv.incL[j]->src;
		int dst=gv.incL[j]->dst;
		double capacity=gv.incL[j]->capacity;
		if(maxUtil_Voting<(appL[winner]->adj[src][dst]/capacity))
			maxUtil_Voting=appL[winner]->adj[src][dst]/capacity;
	}
	cout<<"max utilization rate = "<<maxUtil_Voting<<endl;
	
	//胜利的方案部署到所有应用的adj里
	appL[winner]->end_implement(gv,appL);
	cout<<endl;
	
}

#endif