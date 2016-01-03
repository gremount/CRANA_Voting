#include"common.h"
#include"Flow.h"
#include"graph.h"
#include"voting.h"
#include "res.h"
#include "LP.h"


//t3
const int Inf=99999;
const int N=3;//所有的点数
const int M=6;//包含正反向边
const int Maxreq=3;//一个case的流需求数量
const int Maxpath=N-1;//可能的最长路径: N-1

const int caseN=2;//case总数
const int Maxflow=5;//流的大小可变范围
const int Begin_num=10;//流的大小起始范围

/*
//graph_all
const int Inf=99999;
const int N=20;//所有的点数
const int M=380;//包含正反向边
const int Maxreq=10;//一个case的流需求数量
const int Maxpath=N-1;//可能的最长路径: N-1

const int caseN=10;//case总数
const int Maxflow=30;//流的大小可变范围
const int Begin_num=1;//流的大小起始范围
*/

/*
//graph_Compuserve
const int Inf=99999;
const int N=11;//所有的点数
const int M=28;//包含正反向边
const int Maxreq=10;//一个case的流需求数量
const int Maxpath=N-1;//可能的最长路径: N-1

const int caseN=7;//case总数
const int Maxflow=10;//流的大小可变范围
const int Begin_num=1;//流的大小起始范围
*/

/*
//graph_ATT
const int Inf=99999;
const int N=25;//所有的点数
const int M=112;//包含正反向边
const int Maxreq=10;//一个case的流需求数量
const int Maxpath=N-1;//可能的最长路径: N-1

const int caseN=6;//case总数
const int Maxflow=20;//流的大小可变范围
const int Begin_num=1;//流的大小起始范围
*/

//如果改图，需要修改： 上面的参数 + 图输入 + req输入的部分

int main()
{
	srand((unsigned)time(NULL));
	VGraph gv("d:\\github\\CRANA_Voting\\t3.txt");//Voting用的图
	PGraph gp("d:\\github\\CRANA_Voting\\t3.txt");//LP用的图
	vector<Flow*> flowL;//记录所有的流实例
	ofstream outfile("d:\\github\\result.txt");//最后一个case的结果
	ofstream req_outfile("d:\\github\\req_outfile.txt");

	outfile<<"graph_ATT网络拓扑 caseN: "<<caseN<<endl;
	outfile<<"flow Range: "<<Begin_num<<"--"<<Maxflow+Begin_num-1<<endl<<endl;

	double judge_LP=0,judge_sum_LP=0;
	
	vector<Req*> reqL;
	double table[M2C+1][N2C+1] = {0};
	int ranking[N2C+1]={0};//记录一种排序的投票人数
	double happiness_sum=0;
	
	for(int j=0;j<Maxreq;j++)
		ranking[j]=1;//每种投票结果有1个voter,如果为2就说明该方案有得到两个voter的票

	
	for(int i=0;i<caseN;i++)
	{
		cout<<endl<<"*************************"<<" case "<<i<<"*************************"<<endl;
		//初始化
		for(int j=0;j<Maxreq;j++)
			for(int k=0;k<Maxreq;k++)
				table[j][k]=0;
		
		req_outfile<<"case "<<i<<endl;
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
				a=0;b=1;c=10;
				Req* r = new Req(j,a,b,c);
				reqL.push_back(r);
				gv.reqL.push_back(r);
				Flow* flow_now = new Flow(j,a,b,c);
				flowL.push_back(flow_now);
				req_outfile<<j<<" "<<a<<" "<<b<<" "<<c<<endl;
			}
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
				a=0;b=1;c=10;
				Req* r = new Req(j,a,b,c);
				reqL.push_back(r);
				gv.reqL.push_back(r);
				flowL[j]->flow_modify(a,b,c);
				req_outfile<<j<<" "<<a<<" "<<b<<" "<<c<<endl;
			}
		}

		//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@

		//算最完美方案，所有的路都第一个走得到的方案
		for(int j=0;j<Maxreq;j++)
			gv.cost_best[j] = reqL[j]->flow * gv.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow,flowL[0]->adj);

		//for(int j=0;j<Maxreq;j++)
			//cout<<"gv.cost_best "<<j<<" : "<<gv.cost_best[j]<<endl;

		//提方案
		for(int j=0;j<Maxreq;j++)
			flowL[j]->propose(gv,flowL);

		//评价方案
		for(int j=0;j<Maxreq;j++)
			flowL[j]->evaluate(gv,flowL);

		//投票算法
		for(int j=0;j<Maxreq;j++)
			for(int k=0;k<Maxreq;k++)
			{
				if(flowL[j]->judge[k]==0) table[j+1][k+1]=10000;//如果是0，说明流没有摆在网络中
				else table[j+1][k+1]=flowL[j]->judge[k];
			}
		cout<<endl<<"voting uses ";
		int choice=1;//选择一种投票算法
		int winner=0;
		Voting vv(table,ranking,Maxreq,Maxreq);
		winner=vv.voting(choice);
		
		if (choice == 1)
			cout << "schulze method : " << winner << endl;
		else if (choice == 2)
			cout << "comulative method: " << winner << endl;
		else if (choice == 3)
			cout << "copeland condorcet method: " << winner << endl;
		else
			cout << "ranked pairs method: " << winner << endl;

		
		// table show
		for(int i=1;i<=Maxreq;i++)
		{
			cout<<"flow ";
			cout.setf(ios::right);
			cout.width(3);
			cout<<i<<" judge: ";
			for(int j=1;j<=Maxreq;j++){
				cout.setf(ios::left);
				cout.width(5);
				cout<<table[i][j]<<" ";
			}
			cout<<endl;
		}
		cout<<endl;
		

		//计算满意度
		double happiness=0;//一轮所有流的满意度和，越高越好,0<=满意度<=1
		for(int j=1;j<=Maxreq;j++)
			happiness += gv.cost_best[j-1]/table[j][winner];//最好抉择评分/当前抉择评分
		happiness_sum += happiness;

		//计算方案部署后当前总的cost，如果流没有被安排进网络，就增加惩罚cost
		
		//统计网络latency
		double latencyVoting=0;
		latencyVoting=judge_sum_function(gv,flowL,winner-1);

		for(int j=0;j<Maxreq;j++)
			if(table[j+1][winner]==10000) {
				cout<<"触发惩罚机制"<<endl;
				latencyVoting += Maxpath * reqL[j]->flow;
			}
		cout << "第" << i << "轮整体满意度： " << happiness/Maxreq << endl;
		cout << "多轮满意度：" << happiness_sum / ((i+1)*Maxreq) << endl;
		cout << "多轮整体延时和: " << latencyVoting << endl;
		
		double maxUtil_Voting=0;
		for(int j=0;j<gp.m;j++)
		{
			int src=gv.incL[j]->src;
			int dst=gv.incL[j]->dst;
			double capacity=gv.incL[j]->capacity;
			if(maxUtil_Voting<(flowL[winner-1]->adj[src][dst]/capacity))
				maxUtil_Voting=flowL[winner-1]->adj[src][dst]/capacity;
		}
		cout<<"最大链路利用率: "<<maxUtil_Voting<<endl;

		//胜利的方案部署
		
		for(int j=0;j<Maxreq;j++)
		{
			for(int k1=0;k1<N;k1++)
				for(int k2=0;k2<N;k2++)
				{
					//cout<<j<<" "<<k1<<" "<<k2<<endl;
					flowL[j]->adj[k1][k2]=flowL[winner-1]->adj[k1][k2];
				}
		}
		
		cout<<endl;

		//@@@@@@@@@@@@@@@@@End of Voting@@@@@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@@End of Voting@@@@@@@@@@@@@@@@@@@@@@@@@

		//分段规划部分
		cout<<endl<<"			LP result			"<<endl;
		
		//最优部署计算
		for(int j=0;j<Maxreq;j++)
			gp.cost_best[j] = reqL[j]->flow * gp.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow);

		//线性规划部署
		double result_LP=0;
		result_LP=LP(&gp,reqL);
		
		//计算满意度
		if(result_LP==Inf)judge_LP=0;
		else
		{
			judge_LP=0;
			//for(int i=0;i<reqN;i++)
			//	cout<<g.cost_best[i]<<" "<<g.cost_LP[i]<<endl;
			for(int j=0;j<Maxreq;j++)
				judge_LP += gp.cost_best[j]/gp.cost_LP[j];
		}
		judge_sum_LP += judge_LP;
		cout<<"单轮满意度： "<<judge_LP/Maxreq<<endl;
		cout<<"多轮满意度： "<<judge_sum_LP/(Maxreq*(i+1))<<endl;

		double latency_LP=0;
		latency_LP=judge_sum_LP_function(gp,flowL);
		cout << "多轮整体延时和: " << latency_LP << endl;
		cout<<"最大链路利用率: "<<result_LP<<endl;
		
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