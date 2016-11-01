#include"common.h"
#include"Flow.h"
#include"graph.h"
#include"voting.h"
#include "res.h"
#include "network_te.h"
#include "network_delay.h"

/*
//t3
const int Inf=99999;
const int N=3;//所有的点数
const int M=6;//包含正反向边
const int Maxreq=3;//一个case的流需求数量
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
const int Maxpath=N-1;//可能的最长路径: N-1

const int caseN=6;//case总数
const int Maxflow=15;//流的大小可变范围
const int Begin_num=5;//流的大小起始范围
*/
//如果改图，需要修改： 上面的参数 + 图输入 + req输入的部分

int main()
{
	srand((unsigned)time(NULL));
	string address="graph_all.txt";

	VGraph gv(address);//Voting用的图
	TENetworkGraph gn_te(address);//TE全局优化的图
	DelayNetworkGraph gn_delay(address);//延时全局优化的图

	vector<Flow*> flowL;//记录所有的流实例
	ofstream outfile("result.txt");//最后一个case的结果
	ofstream req_outfile("req_outfile.txt");

	outfile<<"graph_ATT网络拓扑 caseN: "<<caseN<<endl;
	outfile<<"flow Range: "<<Begin_num<<"--"<<Maxflow+Begin_num-1<<endl<<endl;

	double happiness_sum_teLP=0;//te网络的多轮满意度
	double happiness_sum_delayLP=0;//delay网络的多轮满意度
	double happiness_sum=0;//voting的多轮满意度

	vector<Req*> reqL;//流需求
	double table[M2C+1][N2C+1] = {0};//投票用的输入
	int ranking[N2C+1]={0};//记录一种排序的投票人数
	
	
	for(int j=1;j<=Maxreq;j++)
		ranking[j]=1;//每种投票结果有1个voter,如果为2就说明该方案有得到两个voter的票

	
	for(int i=0;i<caseN;i++)
	{
		cout<<endl<<"*************************"<<" case "<<i<<"*************************"<<endl;
		//初始化
		for(int j=0;j<Maxreq;j++)
			for(int k=0;k<Maxreq;k++)
				table[j][k]=0;
		
		req_outfile<<"case "<<i<<endl;

		
		//以下程序时防止每个case都会创建的Req导致的内存泄露
		vector<Req*>::iterator it,iend;
		iend=reqL.end();
		for(it=reqL.begin();it!=iend;it++)	
			delete(*it);
		
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
				//a=0;b=1;c=10;
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
			gv.cost_best[j] = gv.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow,flowL[0]->adj);

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
				if(flowL[j]->judge[k]==0) table[j][k]=10000;//如果是0，说明流没有摆在网络中
				else table[j][k]=flowL[j]->judge[k];
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
		/*
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
		*/

		//计算满意度
		double happiness=0;//一轮所有流的满意度和，越高越好,0<=满意度<=1
		for(int j=0;j<Maxreq;j++)
			happiness += gv.cost_best[j]/table[j][winner];//最好抉择评分/当前抉择评分
		happiness_sum += happiness;

		double s2_voting=0;
		double happiness_avg=happiness_sum/Maxreq;
		for(int j=0;j<Maxreq;j++)
			s2_voting+=(gv.cost_best[j]/table[j][winner]-happiness_avg) * (gv.cost_best[j]/table[j][winner]-happiness_avg);
		s2_voting=s2_voting/Maxreq;

		//计算方案部署后当前总的cost，如果流没有被安排进网络，就增加惩罚cost
		
		//统计网络latency
		double latencyVoting=0;
		latencyVoting=judge_sum_function(gv,flowL,winner);

		for(int j=0;j<Maxreq;j++)
			if(table[j][winner]==10000) {
				cout<<"触发惩罚机制"<<endl;
				latencyVoting += Maxpath * reqL[j]->flow;
			}
		cout << "第" << i << "轮整体满意度： " << happiness/Maxreq << endl;
		cout << "第" << i << "轮满意度满意度方差： " << s2_voting << endl;
		cout << "多轮满意度：" << happiness_sum / ((i+1)*Maxreq) << endl;
		cout << "多轮整体延时和: " << latencyVoting << endl;
		
		double maxUtil_Voting=0;
		for(int j=0;j<gv.m;j++)
		{
			int src=gv.incL[j]->src;
			int dst=gv.incL[j]->dst;
			double capacity=gv.incL[j]->capacity;
			if(maxUtil_Voting<(flowL[winner]->adj[src][dst]/capacity))
				maxUtil_Voting=flowL[winner]->adj[src][dst]/capacity;
		}
		cout<<"最大链路利用率: "<<maxUtil_Voting<<endl;

		//胜利的方案部署
		
		for(int j=0;j<Maxreq;j++)
		{
			for(int k1=0;k1<N;k1++)
				for(int k2=0;k2<N;k2++)
				{
					//cout<<j<<" "<<k1<<" "<<k2<<endl;
					flowL[j]->adj[k1][k2]=flowL[winner]->adj[k1][k2];
				}
		}
		
		cout<<endl;

		//@@@@@@@@@@@@@@@@@End of Voting@@@@@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@@End of Voting@@@@@@@@@@@@@@@@@@@@@@@@@

		//@@@@@@@@@@@@@@@@@Begin of network_te@@@@@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@@Begin of network_te@@@@@@@@@@@@@@@@@@@@@@@@@

		//TE全局优化规划部分
		cout<<endl<<"			network_te result			"<<endl;
		
		//最优部署计算
		for(int j=0;j<Maxreq;j++)
			gn_te.cost_best[j] =gn_te.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow);

		//线性规划部署
		double result_network_te=0;
		result_network_te=network_te(&gn_te,reqL);
		

		//计算满意度
		double happiness_teLP=0;//单轮满意度统计
		if(result_network_te==Inf)happiness_teLP=0;
		else
		{
			happiness_teLP=0;
			//for(int i=0;i<reqN;i++)
			//	cout<<g.cost_best[i]<<" "<<g.cost_LP[i]<<endl;
			for(int j=0;j<Maxreq;j++)
				happiness_teLP += gn_te.cost_best[j]/gn_te.cost_LP[j];
		}
		happiness_sum_teLP += happiness_teLP;
		cout<<"单轮满意度： "<<happiness_teLP/Maxreq<<endl;
		cout<<"多轮满意度： "<<happiness_sum_teLP/(Maxreq*(i+1))<<endl;

		double latency_teLP=0;
		latency_teLP=delay_TENetworkGraph(gn_te,flowL);
		cout << "多轮整体延时和: " << latency_teLP << endl;
		cout<<"最大链路利用率: "<<result_network_te<<endl;
		
		//@@@@@@@@@@@@@@@@@End of network_te@@@@@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@@End of network_te@@@@@@@@@@@@@@@@@@@@@@@@@
		
		//@@@@@@@@@@@@@@@@@Begin of network_delay@@@@@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@@Begin of network_delay@@@@@@@@@@@@@@@@@@@@@@@@@

		//Delay全局优化规划部分
		cout<<endl<<"			network_delay result			"<<endl;
		
		//最优部署计算
		for(int j=0;j<Maxreq;j++)
			gn_delay.cost_best[j] =gn_delay.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow);

		//线性规划部署
		double result_network_delay=0;//近似延时
		result_network_delay=network_delay(&gn_delay,reqL);
		
		//计算满意度
		double happiness_delayLP=0;//单轮满意度统计
		if(result_network_delay==Inf)happiness_delayLP=0;
		else
		{
			happiness_delayLP=0;
			//for(int i=0;i<reqN;i++)
			//	cout<<g.cost_best[i]<<" "<<g.cost_LP[i]<<endl;
			for(int j=0;j<Maxreq;j++)
				happiness_delayLP += gn_delay.cost_best[j]/gn_delay.cost_LP[j];
		}
		happiness_sum_delayLP += happiness_delayLP;

		double s2_delay=0;
		double happiness_avg_delay=happiness_sum_delayLP/Maxreq;
		for(int j=0;j<Maxreq;j++)
			s2_delay+=(gn_delay.cost_best[j]/gn_delay.cost_LP[j]-happiness_avg_delay) * (gn_delay.cost_best[j]/gn_delay.cost_LP[j]-happiness_avg_delay);
		s2_delay=s2_delay/Maxreq;

		cout<<"单轮满意度： "<<happiness_delayLP/Maxreq<<endl;
		cout<<"多轮满意度： "<<happiness_sum_delayLP/(Maxreq*(i+1))<<endl;
		cout << "第" << i << "轮满意度满意度方差： " << s2_delay << endl;

		double latency_delayLP=0;
		latency_delayLP=delay_DelayNetworkGraph(gn_delay,flowL);
		cout << "多轮整体延时和: " << latency_delayLP << endl;

		double maxUtil_DelayNetworkGraph=0;
		for(int j=0;j<gn_delay.m;j++)
		{
			int src=gn_delay.incL[j]->src;
			int dst=gn_delay.incL[j]->dst;
			double capacity=gn_delay.incL[j]->capacity;
			if(maxUtil_DelayNetworkGraph<(gn_delay.adj[src][dst]/capacity))
				maxUtil_DelayNetworkGraph=gn_delay.adj[src][dst]/capacity;
		}
		cout<<"最大链路利用率: "<<maxUtil_DelayNetworkGraph<<endl;

		//@@@@@@@@@@@@@@@@@End of network_delay@@@@@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@@End of network_delay@@@@@@@@@@@@@@@@@@@@@@@@@

	}//一个case结束
	getchar();
	return 0;
}