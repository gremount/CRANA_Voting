﻿#ifndef APP_NET_VOTING_H
#define APP_NET_VOTING_H

#include"app.h"
#include"net.h"
#include"voting.h"
#include"decider.h"

//产生 req.txt
void generate_traffic(string req_address)
{
	ofstream reqOutFile(req_address);
	reqOutFile << MAXREQ << endl;
	for (int i = 0; i < MAXREQ; i++){
		int app_id = rand() % APPNUM;
		int src = rand() % N;
		int dst = rand() % N;
		while (src == dst){
			dst = rand() % N;
		}
		double flow = 1.0 + MINFLOW + rand() % MAXFLOW;//前开后闭区间(MINFLOW,MAXFLOW]
		reqOutFile << i << " " << app_id << " " << src << " "
			<< dst << " " << flow << endl;
	}
}

//选出满意度方差最小的方案，这里没有用上
int voting_crana(vector<vector<double> > &table){
	double s2_min = INF;
	double s2_min_loc = 0;
	for (int i = 0; i < APPNUM; i++){
		//计算投票winner满意度
		double happiness_sum = 0;
		double happiness_avg = 0;
		for (int j = 0; j < APPNUM; j++)
			happiness_sum += table[j][j] / table[j][i];
		happiness_avg = happiness_sum / APPNUM;

		//计算满意度方差
		double s2_sum = 0;
		double s2_avg = 0;
		for (int j = 0; j < APPNUM; j++){
			s2_sum += (happiness_avg - table[j][j] / table[j][i])
				* (happiness_avg - table[j][j] / table[j][i]);
		}
		s2_avg = s2_sum / APPNUM;

		if (s2_min>s2_avg){
			s2_min = s2_avg;
			s2_min_loc = i;
		}
	}
	return s2_min_loc;
}

//app-net-voting算法的主要实现
void app_net_voting(string graph_address, string req_address, string result_address,
	vector<double>& teL, vector<double>& haL)
{
	double happiness_sum_all = 0;//满意度总和，为了求所有论部署的满意度平均值
	double s2_sum_all = 0;//方差总和

	//读入图
	VGraph* gv = new VGraph(graph_address);
	//cout << "graph complete" << endl;

	//产生 req.txt
	generate_traffic(req_address);

	//读入流量矩阵: (id app_id src dst flow)
	ifstream reqFile(req_address);
	int reqNum=0;
	reqFile>>reqNum;

	//************************  初始化  ***********************
	//投票系统初始化
	vector<vector<double> > table;
	vector<int> ranking;
	table.resize(DECNUM);
	ranking.resize(DECNUM);
	for (int i = 0; i < DECNUM; i++)
		table[i].resize(DECNUM);
	for (int j = 0; j<DECNUM; j++)
		ranking[j]=1;//每种投票结果有1个voter,如果为2就说明该方案有得到两个voter的票
	for (int j = APPNUM; j < DECNUM;j++)
		ranking[j] = VOTES;//Net投票者由于人数少，为了公平起见，给予更多的票数
	//cout << "voting init complete" << endl;

	//启动APP和Net，并初始化
	vector<Decider*> decL;//记录所有的Decider
	for (int k = 0; k < APPNUM; k++){
		Decider* decider_new = new APP(k, *gv);
		decL.push_back(decider_new);
	}
	for (int k = APPNUM; k < DECNUM; k++){
		Decider* decider_new = new Net(APPNUM, *gv);
		decL.push_back(decider_new);
	}
	for (int j = 0; j<DECNUM; j++)
		decL[j]->init();
	//cout << "APP and Net init complete" << endl;

	//记录所有的APP Decider
	vector<Decider*> appL;
	for (int k = 0; k < APPNUM; k++){
		appL.push_back(decL[k]);
	}

	//记录所有的Net Decider
	vector<Decider*> netL;
	for (int k = APPNUM; k < DECNUM; k++){
		netL.push_back(decL[k]);
	}

	//初始化APP背景流量
	int background_flow, background_flow_size;
	for (int k = 0; k < APPNUM; k++){
		for (int j = 0; j < M; j++){
			background_flow = j;
			background_flow_size = 1.0 + 
				rand() % (20);
			int src = gv->incL[background_flow]->src;
			int dst = gv->incL[background_flow]->dst;
			appL[k]->adjMyFlow[src][dst] += background_flow_size;
			gv->adj[src][dst] += background_flow_size;
		}
	}
	
	//初始化流需求
	vector<Req*> reqL;//流需求
	reqL.clear();
	for(int j=0;j<reqNum;j++)
	{
		int id = 0, app_id = 0, a = 0, b = 0;
		double c=0;
		reqFile>>id>>app_id>>a>>b>>c;
		Req* r = new Req(id,app_id,a,b,c);
		reqL.push_back(r);
	}
	//cout << "read request complete" << endl<<endl;
	
	//ofstream resultFile(result_address);

	//针对每一个req进行投票
	for (int i = 0; i < reqL.size(); i++)
	{
		//cout << "request " << i << endl;
		//resultFile <<endl<< "******************** request  "
			//<< i <<" ********************"<< endl;
		//resultFile << "req: " << reqL[i]->src << " -> " << reqL[i]->dst 
			//<<" app_id="<<reqL[i]->app_id<<" flow="<<reqL[i]->flow<< endl;
		for (int j = 0; j<DECNUM; j++)
			decL[j]->init();
		//************************  投票机制开始  **********************
		//提方案
		for (int j = 0; j<DECNUM; j++){
			decL[j]->propose(*reqL[i]);
			/*
			cout << "app " << j << " proposal is " << endl;
			for (int k = 0; k < appL[j]->pathRecord.size(); k++)
				cout << appL[j]->pathRecord[k] << " ";
			cout << endl;
			*/
		}
		//cout << "propose complete" << endl;

		//评价方案
		for (int j = 0; j<DECNUM; j++)
			decL[j]->evaluate(*reqL[i], decL);
		//cout << "evaluate complete" << endl;

		
		//投票算法
		for (int j = 0; j<DECNUM; j++)
			for (int k = 0; k<DECNUM; k++)
			{
				if (decL[j]->judge[k] == 0) table[j][k] = RINF;//防止出现 除0
				else table[j][k] = decL[j]->judge[k];
			}

		int choice = 1;//选择一种投票算法
		int winner = 0;
		Voting* vv = new Voting(table, ranking, DECNUM, DECNUM);
		winner = vv->voting(choice);
		
		//使用crana method来计算获胜方案
		//winner = voting_crana(table);

		//强制改变winner，为了实现对比方案 "selfish routing"
		//winner = reqL[i]->app_id;

		//强制改变winner，为了实现对比方案 "TE routing"
		//winner = APPNUM;

		// file record of table show
		/*
		resultFile << "Sequence:      ";
		for (int i = 0; i < appL.size(); i++){
			resultFile.setf(ios::right);
			resultFile << setw(11) << i;
		}
		resultFile << endl;
		for (int i = 0; i < appL.size(); i++)
		{
			resultFile << "APP ";
			resultFile.setf(ios::right);
			resultFile.width(3);
			resultFile << i << " judge: ";
			for (int j = 0; j < appL.size(); j++){
				resultFile.setf(ios::left);
				resultFile << setw(10) <<setprecision(5)<< table[i][j] << " ";
			}
			resultFile << endl;
		}
		resultFile << endl;
		*/
		//cout << "************************** winner = " << winner << endl;
		//resultFile << "winner = " << winner << endl;

		//计算投票winner满意度
		double happiness_sum = 0;
		double happiness_avg = 0;
		for (int j = 0; j < APPNUM; j++)
			happiness_sum += table[j][j] / table[j][winner];
		happiness_avg = happiness_sum / APPNUM;
		//cout << "happiness_avg = " << happiness_avg << endl;
		//resultFile << "happiness_avg = " << happiness_avg << endl;
		happiness_sum_all += happiness_sum;

		haL.push_back(happiness_avg);

		//计算满意度方差
		double s2_sum = 0;
		double s2_avg = 0;
		for (int j = 0; j < APPNUM; j++){
			s2_sum += (happiness_avg - table[j][j] / table[j][winner])
				* (happiness_avg - table[j][j] / table[j][winner]);
		}
		s2_avg = s2_sum / APPNUM;
		//cout << "s2_avg= " << s2_avg << endl;
		//resultFile << "s2_avg = " << s2_avg << endl;
		s2_sum_all += s2_sum;
		
		//胜利的方案部署到gv图的adj里和req对应的APP的adjMyFlow里
		for (int i = 0; i < decL[winner]->pathRecord.size() - 1; i++){
			int tail = decL[winner]->pathRecord[i];
			int head = decL[winner]->pathRecord[i + 1];
			gv->adj[tail][head] += reqL[i]->flow;
			decL[reqL[i]->app_id]->adjMyFlow[tail][head] += reqL[i]->flow;
		}

		//计算胜利方案部署后的TE
		double te = 0;
		for (int j = 0; j < gv->incL.size(); j++){
			int tail = gv->incL[j]->src;
			int head = gv->incL[j]->dst;
			if (te < gv->adj[tail][head] / gv->incL[j]->capacity)
				te = gv->adj[tail][head] / gv->incL[j]->capacity;
		}
		
		teL.push_back(te);

		/*
		//输出adj矩阵
		resultFile << endl;
		for (int i = 0; i < appL.size(); i++)
		{
			resultFile<< "APP "<<i<<endl;
			for (int j = 0; j < N; j++){
				for (int k = 0; k < N; k++){
					resultFile << setw(10) << setprecision(5) 
						<< appL[i]->adjMyFlow[j][k] << " ";
				}
				resultFile << endl;
			}
			resultFile << endl;
		}
		resultFile << endl;
		resultFile << "ADJ"<< endl;
		for (int i = 0; i < N; i++){
			for (int j = 0; j < N; j++){
				resultFile << setw(10) << setprecision(5)
					<< gv.adj[i][j] << " ";
			}
			resultFile << endl;
		}
		*/
		delete vv;
	}
	//cout << "最终满意度评价 = "<< happiness_sum_all / MAXREQ / APPNUM << endl;
	//cout << "最终满意度方差评价 = " << s2_sum_all / MAXREQ / APPNUM << endl;
	delete gv;
	for (int i = 0; i < DECNUM; i++)
		delete decL[i];
	for (int i = 0; i < MAXREQ; i++)
		delete reqL[i];
}

#endif