#ifndef APP_VOTING_H
#define APP_VOTING_H

#include"app.h"
#include"voting.h"

void app_voting(string graph_address, string req_address, string result_address)
{
	//读入图
	VGraph gv(graph_address);
	cout << "graph complete" << endl;

	//产生 req.txt
	ofstream reqOutFile(req_address);
	reqOutFile << MAXREQ << endl;
	for (int i = 0; i < MAXREQ; i++){
		int app_id = rand() % APPNUM;
		int src = rand() % N;
		int dst = rand() % N;
		while (src == dst){
			dst = rand() % N;
		}
		double flow = 1.0 + MINFLOW + rand() % MAXFLOW;
		reqOutFile << i << " " << app_id << " " << src << " " 
			<< dst << " " << flow << endl;
	}

	//读入流量矩阵: (id app_id src dst flow)
	ifstream reqFile(req_address);
	int reqNum=0;
	reqFile>>reqNum;

	//************************  初始化  ***********************
	//投票系统初始化
	double table[M2C+1][N2C+1] = {0};//投票用的输入
	int ranking[N2C+1]={0};//记录一种排序的投票人数
	for(int j=0;j<reqNum;j++)
		ranking[j]=1;//每种投票结果有1个voter,如果为2就说明该方案有得到两个voter的票
	for(int j=0;j<reqNum;j++)
		for(int k=0;k<reqNum;k++)
			table[j][k]=0;
	cout << "voting init complete" << endl;

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
	for(int j=0;j<reqNum;j++)
	{
		int id = 0, app_id = 0, a = 0, b = 0;
		double c=0;
		reqFile>>id>>app_id>>a>>b>>c;
		Req* r = new Req(id,app_id,a,b,c);
		reqL.push_back(r);
	}
	cout << "read request complete" << endl<<endl;
	
	ofstream resultFile(result_address);

	//针对每一个req进行投票
	for (int i = 0; i < reqL.size(); i++)
	{
		cout << "request " << i << endl;
		resultFile << "request " << i << endl;
		for (int j = 0; j<APPNUM; j++)
			appL[j]->init();
		//************************  投票机制开始  **********************
		//提方案
		for (int j = 0; j<APPNUM; j++){
			appL[j]->propose(*reqL[i]);
			//cout << "app " << j << " proposal is " << endl;
			//for (int k = 0; k < appL[j]->pathRecord.size(); k++)
				//cout << appL[j]->pathRecord[k] << " ";
			//cout << endl;
		}
		//cout << "propose complete" << endl;

		//评价方案
		for (int j = 0; j<APPNUM; j++)
			appL[j]->evaluate(*reqL[i], appL);
		//cout << "evaluate complete" << endl;

		
		//投票算法
		for (int j = 0; j<APPNUM; j++)
			for (int k = 0; k<APPNUM; k++)
			{
				if (appL[j]->judge[k] == 0) table[j][k] = RINF;//防止出现 除0
				else table[j][k] = appL[j]->judge[k];
			}

		int choice = 1;//选择一种投票算法
		int winner = 0;
		Voting vv(table, ranking, APPNUM, APPNUM);
		winner = vv.voting(choice);
		cout << "schulze winner = " << winner << endl;
		resultFile << "schulze winner = " << winner << endl;

		// file record of table show
		resultFile << endl;
		for (int i = 0; i < appL.size(); i++)
		{
			resultFile << "flow ";
			resultFile.setf(ios::right);
			resultFile.width(3);
			resultFile << i << " judge: ";
			for (int j = 0; j < appL.size(); j++){
				resultFile.setf(ios::left);
				resultFile << setw(10) << table[i][j] << " ";
			}
			resultFile << endl;
		}
		resultFile << endl;

		//计算投票winner满意度
		double happiness_sum = 0;
		double happiness_avg = 0;
		for (int j = 0; j < APPNUM; j++)
			happiness_sum += table[j][j] / table[j][winner];
		happiness_avg = happiness_sum / APPNUM;
		cout << "happiness_avg = " << happiness_avg << endl;
		resultFile << "happiness_avg = " << happiness_avg << endl;
		
		//胜利的方案部署到gv图的adj里和req对应的APP的adjMyFlow里
		for (int i = 0; i < appL[winner]->pathRecord.size() - 1; i++){
			int tail = appL[winner]->pathRecord[i];
			int head = appL[winner]->pathRecord[i + 1];
			gv.adj[tail][head] += reqL[i]->flow;
			appL[reqL[i]->app_id]->adjMyFlow[tail][head] += reqL[i]->flow;
		}
		cout << endl;
		
	}
}

#endif