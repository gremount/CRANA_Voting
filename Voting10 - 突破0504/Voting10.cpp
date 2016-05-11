#include"common.h"
#include"Flow.h"
#include"graph.h"
#include"voting.h"
#include "res.h"
#include "LP.h"
#include "network.h"
#include "voter.h"

/*
//t3
const int Inf=99999;
const int N=3;//所有的点数
const int M=6;//包含正反向边
const int Maxreq=3;//一个case的流需求数量
const int Voternum=4;//flow+network
const int Maxpath=N-1;//可能的最长路径: N-1

const int caseN=2;//case总数
const int Maxflow=5;//流的大小可变范围
const int Begin_num=10;//流的大小起始范围
*/


//graph_all
const int Inf=99999;
const int N=20;//所有的点数
const int M=380;//包含正反向边
const int Maxreq=10;//一个case的流需求数量
const int Voternum=11;//flow+network
const int Maxpath=N-1;//可能的最长路径: N-1

const int caseN=10;//case总数
const int Maxflow=30;//流的大小可变范围
const int Begin_num=1;//流的大小起始范围


/*
//graph_Compuserve
const int Inf=99999;
const int N=11;//所有的点数
const int M=28;//包含正反向边
const int Maxreq=10;//一个case的流需求数量
const int Voternum=11;//flow+network
const int Maxpath=N-1;//可能的最长路径: N-1

const int caseN=6;//case总数
const int Maxflow=10;//流的大小可变范围
const int Begin_num=1;//流的大小起始范围
*/

/*
//graph_ATT
const int Inf=99999;
const int N=25;//所有的点数
const int M=112;//包含正反向边
const int Maxreq=10;//一个case的流需求数量
const int Voternum=11;//flow+network
const int Maxpath=N-1;//可能的最长路径: N-1

const int caseN=8;//case总数
const int Maxflow=15;//流的大小可变范围
const int Begin_num=5;//流的大小起始范围
*/

//如果改图，需要修改： 上面的参数 + 图输入 + req输入的部分

int main()
{
	srand((unsigned)time(NULL));
	VGraph gv("graph_all.txt");//Voting用的图
	PGraph gp("graph_all.txt");//LP用的图
	vector<Voter*> flowL;//记录所有的流实例
	vector<Voter*> voterL;//记录所有的投票者
	ofstream outfile("result.txt");//最后一个case的结果
	ofstream req_outfile("req_outfile.txt");

	outfile<<"graph_all网络拓扑 caseN: "<<caseN<<endl;
	outfile<<"flow Range: "<<Begin_num<<"--"<<Maxflow+Begin_num-1<<endl<<endl;

	
	
	vector<Req*> reqL;
	double table[M2C+1][N2C+1] = {0};
	int ranking[N2C+1]={0};//记录一种排序的投票人数
	double happiness_sum=0;
	
	for(int j=1;j<=Voternum;j++)
		ranking[j]=1;//每种投票结果有1个voter,如果为2就说明该方案有得到两个voter的票

	Voter* net_lb = new Network_LB(Maxreq,1);// Maxreq是投票者id, 1 表示网络投票者
	ranking[Voternum]=5;//给网络投票者更多的票数

	for(int i=0;i<caseN;i++)
	{
		cout<<endl<<"*************************"<<" case "<<i<<"*************************"<<endl;
		outfile<<endl<<"*************************"<<" case "<<i<<"*************************"<<endl;
		//初始化
		for(int j=0;j<Voternum;j++)
			for(int k=0;k<Voternum;k++)
				table[j][k]=0;
		
		req_outfile<<"case "<<i<<endl;

		
		//以下程序时防止每个case都会创建的Req导致的内存泄露
		vector<Req*>::iterator it,iend;
		iend=reqL.end();
		for(it=reqL.begin();it!=iend;it++)	
			delete(*it);
		
		//删除上一个case的流需求
		reqL.clear();
		gv.reqL.clear();

		if(i==0){
			for(int j=0;j<Maxreq;j++)
			{
				int a=0,b=0,c=0;
				while(1){
					c = Begin_num + rand()%Maxflow;
					//if(c!=0) break;
					a = rand()%N;
					b = rand()%N;
					if(a!=b && c!=0) break;
				}
				//a=0;b=1;c=10;
				Req* r = new Req(j,a,b,c);
				reqL.push_back(r);
				gv.reqL.push_back(r);
				Voter* flow_now = new Flow(j,0,a,b,c);
				flowL.push_back(flow_now);
				voterL.push_back(flow_now);
				req_outfile<<j<<" "<<a<<" "<<b<<" "<<c<<endl;
			}
			voterL.push_back(net_lb);
		}
		else{
			for(int j=0;j<Maxreq;j++)
			{
				int a=0,b=0,c=0;
				while(1){
					c = Begin_num + rand()%Maxflow;
					//if(c!=0) break;
					a = rand()%N;
					b = rand()%N;
					if(a!=b && c!=0) break;
				}
				//a=0;b=1;c=10;
				Req* r = new Req(j,a,b,c);
				reqL.push_back(r);
				gv.reqL.push_back(r);
				flowL[j]->modify(a,b,c);
				req_outfile<<j<<" "<<a<<" "<<b<<" "<<c<<endl;
			}
			net_lb->modify(0,0,0);
		}

		//cout<<"init completed"<<endl;
		
		//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@

		//算最完美方案，所有的路都第一个走得到的方案
		for(int j=0;j<Maxreq;j++)
			gv.cost_best[j] = gv.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow,flowL[0]->adj);

		//for(int j=0;j<Maxreq;j++)
			//cout<<"gv.cost_best "<<j<<" : "<<gv.cost_best[j]<<endl;

		//提方案
		for(int j=0;j<voterL.size();j++)
			voterL[j]->propose(gv,flowL);
		//cout<<"提出方案结束"<<endl;

		//评价方案
		for(int j=0;j<voterL.size();j++)
			voterL[j]->evaluate(gv,voterL);
		//cout<<"评价方案结束"<<endl;

		//投票算法, 投票算法的输入是从1开始的，但是judge是从0开始的，所以需要调整一下
		for(int j=0;j<voterL.size();j++)
			for(int k=0;k<voterL.size();k++)
			{
				if(voterL[j]->judge[k]==0) table[j+1][k+1]=10000;//如果是0，说明流没有摆在网络中
				else table[j+1][k+1]=voterL[j]->judge[k];
			}
		cout<<endl<<"voting uses ";
		int choice=1;//选择一种投票算法
		int winner=0;
		Voting vv(table,ranking,voterL.size(),voterL.size());
		winner=vv.voting(choice);
		winner=winner-1;//将结果还是按照从0开始编号

		if (choice == 1)
			cout << "schulze method : " << winner << endl;
		else if (choice == 2)
			cout << "comulative method: " << winner << endl;
		else if (choice == 3)
			cout << "copeland condorcet method: " << winner << endl;
		else
			cout << "ranked pairs method: " << winner << endl;

		
		// table show
		for(int i=1;i<=voterL.size();i++)
		{
			cout<<"flow ";
			cout.setf(ios::right);
			cout.width(3);
			cout<<i<<" judge: ";
			for(int j=1;j<=voterL.size();j++){
				cout.setf(ios::left);
				cout.width(5);
				cout<<table[i][j]<<" ";
			}
			cout<<endl;
		}
		cout<<endl;
		
		// file record of table show
		outfile<<endl;
		for(int i=1;i<=voterL.size();i++)
		{
			outfile<<"flow ";
			outfile.setf(ios::right);
			outfile.width(3);
			outfile<<i<<" judge: ";
			for(int j=1;j<=voterL.size();j++){
				outfile.setf(ios::left);
				outfile.width(5);
				outfile<<table[i][j]<<" ";
			}
			outfile<<endl;
		}
		outfile<<endl;


		//计算满意度
		double happiness=0;//一轮所有流的满意度和，越高越好,0<=满意度<=1
		for(int j=1;j<=Maxreq;j++)
			happiness += gv.cost_best[j-1]/table[j][winner+1];//最好抉择评分/当前抉择评分
		happiness_sum += happiness;

		//计算方案部署后当前总的cost，如果流没有被安排进网络，就增加惩罚cost
		
		//统计网络latency
		double latencyVoting=0;
		latencyVoting=judge_sum_function(gv,voterL,winner);

		for(int j=0;j<Maxreq+1;j++)
			if(table[j+1][winner+1]==10000) {
				cout<<"触发惩罚机制"<<endl;
				latencyVoting += Maxpath * reqL[j]->flow;
			}
		cout << "winner = "<<winner<<endl;
		cout << "第" << i << "轮整体满意度： " << happiness/Maxreq << endl;
		cout << "多轮满意度：" << happiness_sum / ((i+1)*Maxreq) << endl;
		cout << "多轮整体延时和: " << latencyVoting << endl;

		double maxUtil_Voting=0;
		maxUtil_Voting=voterL[Maxreq]->judge[winner];
		cout<<"最大链路利用率: "<<maxUtil_Voting<<endl;

		//文件记录
		outfile<<"Case "<<i<<" Voting Result"<<endl;
		outfile << "winner = "<<winner<<endl;
		outfile << "第" << i << "轮整体满意度： " << happiness/Maxreq << endl;
		outfile << "多轮满意度：" << happiness_sum / ((i+1)*Maxreq) << endl;
		outfile << "多轮整体延时和: " << latencyVoting << endl;
		outfile <<"最大链路利用率: "<<maxUtil_Voting<<endl;

		//胜利的方案部署
		for(int j=0;j<voterL.size();j++)
			voterL[j]->end_implement(gv,winner,voterL);
		
		cout<<endl;

		//@@@@@@@@@@@@@@@@@End of Voting@@@@@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@@End of Voting@@@@@@@@@@@@@@@@@@@@@@@@@

		//分段规划部分
		cout<<endl<<"			LP result			"<<endl;
		
		//judge_LP记录单轮满意度总和，judge_sum_LP记录多轮满意度总和
		double judge_LP=0;
		static double judge_sum_LP=0;

		//最优部署计算
		for(int j=0;j<Maxreq;j++)
			gp.cost_best[j] =gp.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow);

		//线性规划部署
		double result_LP=0;//TE结果
		result_LP=LP(&gp,reqL);
		
		//计算满意度
		if(result_LP==Inf)judge_LP=0;
		else
		{
			judge_LP=0;
			//for(int i=0;i<Maxreq;i++)
				//cout<<gp.cost_best[i]<<" "<<gp.cost_LP[i]<<endl;
			for(int j=0;j<Maxreq;j++)
				judge_LP += gp.cost_best[j]/gp.cost_LP[j];
		}
		judge_sum_LP += judge_LP;
		cout<<"单轮满意度： "<<judge_LP/Maxreq<<endl;
		cout<<"多轮满意度： "<<judge_sum_LP/(Maxreq*(i+1))<<endl;

		double latency_LP=0;
		latency_LP=judge_sum_LP_function(gp);
		cout << "多轮整体延时和: " << latency_LP << endl;
		cout<<"最大链路利用率: "<<result_LP<<endl;
		
		//文件记录
		outfile<<"Case "<<i<<" LP Result"<<endl;
		outfile<<"单轮满意度： "<<judge_LP/Maxreq<<endl;
		outfile<<"多轮满意度： "<<judge_sum_LP/(Maxreq*(i+1))<<endl;
		outfile << "多轮整体延时和: " << latency_LP << endl;
		outfile<<"最大链路利用率: "<<result_LP<<endl;

		/*
		cout<<"单轮最大剩余链路利用率: "<<result_LP<<endl;
		double maxUtil_LP=0;
		for(int j=0;j<gp.m;j++)
		{
			int src=gp.incL[j]->src;
			int dst=gp.incL[j]->dst;
			float capacity=gp.incL[j]->capacity;
			if(maxUtil_LP<(gp.adj[src][dst]/capacity))
				maxUtil_LP=gp.adj[src][dst]/capacity;
		}
		*/
		
		/*
		if(i==(caseN-1)){
			outfile << "LP result"<<endl;
			outfile<<"单轮满意度： "<<judge_LP/Maxreq<<endl;
			outfile<<"多轮满意度： "<<judge_sum_LP/(Maxreq*(i+1))<<endl;
			outfile<<"多轮整体代价和: "<<result_sum_LP<<endl;
		}
		*/
		

	}//一个case结束
	getchar();
	return 0;
}