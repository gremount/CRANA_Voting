#include"common.h"
#include"Flow.h"
#include"graph.h"
#include"voting.h"
#include "res.h"
#include "LP.h"
#include "network.h"
#include "voter.h"
#include "neutral.h"

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

//如果改图，需要修改： 上面的参数 + 图输入3处 + req输入的部分

int main()
{
	srand((unsigned)time(NULL));
	string graph_address="graph_all.txt";

	VGraph gv(graph_address);//Voting用的图
	PGraph gp(graph_address);//LP用的图
	VGraph gv_flow(graph_address); //Flow Voting用的图
	vector<Voter*> flowL;//记录所有的流实例
	vector<Voter*> voterL;//记录所有的投票者
	vector<Voter*> candiL;//记录所有的候选者
	int voting_choice=1;

	//收集每一轮投票的结果
	vector<double> app_happiness, voting_happiness, network_happiness;
	vector<double> app_delay, voting_delay, network_delay;
	vector<double> app_te, voting_te, network_te;
	vector<double> app_load, voting_load, network_load;

	//记录当前时间
	time_t tt = time(NULL);//这句返回的只是一个时间cuo
 	tm* t= localtime(&tt);

	string s="result";
	stringstream ss1;
	ss1<<t->tm_year+1900;
	s.append(ss1.str());

	stringstream ss2;
	ss2<<t->tm_mon+1;
	s.append(ss2.str());
 	
 	stringstream ss3;
	ss3<<t->tm_mday;
	s.append(ss3.str());
	
	stringstream ss4;
	ss4<<t->tm_hour;
	s.append(ss4.str());
	
	stringstream ss5;
	ss5<<t->tm_min;
	s.append(ss5.str());
	s.append(".txt");

	cout<<"current output file : "<<s<<endl;

	ofstream outfile(s);//最后一个case的结果
	ofstream req_outfile("req_outfile.txt");
	outfile<<graph_address<<"网络拓扑 caseN: "<<caseN<<endl;
	outfile<<"flow Range: "<<Begin_num<<"--"<<Maxflow+Begin_num-1<<endl<<endl;
	outfile<<"current output file : "<<s<<endl;

	vector<Req*> reqL;
	double table[M2C+1][N2C+1] = {0};
	int ranking[N2C+1]={0};//记录一种排序的投票人数
	double happiness_sum=0;
	
	for(int j=1;j<=Voternum;j++)
		ranking[j]=1;//每种投票结果有1个voter,如果为2就说明该方案有得到两个voter的票

	Voter* net_lb = new Network_LB(Maxreq,1);// Maxreq是投票者id, 1 表示网络投票者
	ranking[Voternum]=5;//给网络投票者更多的票数

	//Maxreq+1是投票者id，2表示中立投票者
	Voter* neutral_low = new Neutral(Maxreq+1, 2);  // te = 胜者TE基础上+5%
	Voter* neutral_middle = new Neutral(Maxreq+2, 2);  // te = 胜者TE基础上+10%
	Voter* neutral_high = new Neutral(Maxreq+3, 2); // te = 胜者TE基础上+20%

	//初次 修改neutral的te值
	neutral_low->te = 0.3;
	neutral_middle->te = 0.4;
	neutral_high->te = 0.5;

	//******** Flow Voting Variables *******
	vector<Voter*> flowL_flow;//记录Flow Voting所有的流实例
	vector<Voter*> voterL_flow;//记录Flow Voting所有的投票者
	vector<Voter*> candiL_flow;//记录Flow Voting所有的候选者
	double table_flow[M2C+1][N2C+1] = {0};
	int ranking_flow[N2C+1]={0};//记录一种排序的投票人数
	double happiness_sum_flow=0;
	for(int j=1;j<=Maxreq;j++)
		ranking_flow[j]=1;//每种投票结果有1个voter,如果为2就说明该方案有得到两个voter的票



	for(int i=0;i<caseN;i++)
	{
		cout<<endl<<"*************************"<<" case "<<i<<"*************************"<<endl;
		outfile<<endl<<"*************************"<<" case "<<i<<"*************************"<<endl;
		//初始化
		for(int j=0;j<Voternum;j++)
			for(int k=0;k<Voternum+3;k++)
			{
				table[j][k]=0;
				table_flow[j][k]=0;//table_flow的有效数据范围是table的子集
			}
		req_outfile<<"case "<<i<<endl;

		
		//以下程序时防止每个case都会创建的Req导致的内存泄露
		vector<Req*>::iterator it,iend;
		iend=reqL.end();
		for(it=reqL.begin();it!=iend;it++)	
			delete(*it);
		
		//删除上一个case的流需求
		reqL.clear();
		gv.reqL.clear();
		gv_flow.reqL.clear();

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
				gv_flow.reqL.push_back(r);
				Voter* flow_now = new Flow(j,0,a,b,c);
				Voter* flow_now_flow =  new Flow(j,0,a,b,c);//Flow Voting use
				flowL.push_back(flow_now);
				voterL.push_back(flow_now);
				candiL.push_back(flow_now);
				flowL_flow.push_back(flow_now_flow);
				voterL_flow.push_back(flow_now_flow);
				candiL_flow.push_back(flow_now_flow);
				req_outfile<<j<<" "<<a<<" "<<b<<" "<<c<<endl;
			}
			voterL.push_back(net_lb);
			
			candiL.push_back(net_lb);
			candiL.push_back(neutral_low);
			candiL.push_back(neutral_middle);
			candiL.push_back(neutral_high);
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
				gv_flow.reqL.push_back(r);
				flowL[j]->modify(a,b,c);
				//candiL[j]->modify(a,b,c);
				flowL_flow[j]->modify(a,b,c);
				//candiL_flow[j]->modify(a,b,c);
				req_outfile<<j<<" "<<a<<" "<<b<<" "<<c<<endl;
			}
			net_lb->modify(0,0,0);
			neutral_low->modify(0,0,0);
			neutral_middle->modify(0,0,0);
			neutral_high->modify(0,0,0);
		}

		//cout<<"init completed"<<endl;
		
	

		//**************************************  Flow Voting  **************************************
		//**************************************  Flow Voting  **************************************
		//only flows' proposal can be voted for a winner
		
		//算最完美方案，所有的路都第一个走得到的方案
		for(int j=0;j<Maxreq;j++)
			gv_flow.cost_best[j] = gv.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow,flowL_flow[0]->adj);

		//for(int j=0;j<Maxreq;j++)
			//cout<<"gv.cost_best "<<j<<" : "<<gv.cost_best[j]<<endl;

		//提方案
		for(int j=0;j<candiL_flow.size();j++)
			candiL_flow[j]->propose(gv_flow);
		//cout<<"提出方案结束"<<endl;

		//评价方案
		for(int j=0;j<voterL_flow.size();j++)
			voterL_flow[j]->evaluate(gv_flow,candiL_flow);
		//cout<<"评价方案结束"<<endl;

		//投票算法, 投票算法的输入是从1开始的，但是judge是从0开始的，所以需要调整一下
		for(int j=0;j<voterL_flow.size();j++)
			for(int k=0;k<candiL_flow.size();k++)
			{
				//该操作在evaluate时候就惩罚----不用if(voterL[j]->judge[k]==0) table[j+1][k+1]=10000;//如果是0，说明流没有摆在网络中
				table_flow[j+1][k+1]=voterL_flow[j]->judge[k];
			}
		cout<<endl<<"voting uses ";
		int choice_flow=voting_choice;//选择一种投票算法
		int winner_flow=0;
		Voting vv_flow(table_flow,ranking_flow,voterL_flow.size(),candiL_flow.size());
		winner_flow=vv_flow.voting(choice_flow);
		winner_flow=winner_flow-1;//将结果还是按照从0开始编号

		if (choice_flow == 1)
			cout << "schulze method : " << winner_flow << endl;
		else if (choice_flow == 2)
			cout << "comulative method: " << winner_flow << endl;
		else if (choice_flow == 3)
			cout << "copeland condorcet method: " << winner_flow << endl;
		else
			cout << "ranked pairs method: " << winner_flow << endl;

		
		// table show
		for(int i=1;i<=voterL_flow.size();i++)
		{
			cout<<"flow ";
			cout.setf(ios::right);
			cout.width(3);
			cout<<i-1<<" judge: ";
			for(int j=1;j<=candiL_flow.size();j++){
				cout.setf(ios::left);
				cout.width(5);
				cout<<table_flow[i][j]<<" ";
			}
			cout<<endl;
		}
		cout<<endl;
		
		// file record of table show
		outfile<<endl;
		for(int i=1;i<=voterL_flow.size();i++)
		{
			outfile<<"flow ";
			outfile.setf(ios::right);
			outfile.width(3);
			outfile<<i-1<<" judge: ";
			for(int j=1;j<=candiL_flow.size();j++){
				outfile.setf(ios::left);
				outfile.width(5);
				outfile<<table_flow[i][j]<<" ";
			}
			outfile<<endl;
		}
		outfile<<endl;


		//计算满意度
		double happiness_flow=0;//一轮所有流的满意度和，越高越好,0<=满意度<=1
		for(int j=1;j<=Maxreq;j++)
			happiness_flow += gv_flow.cost_best[j-1]/table_flow[j][winner_flow+1];//最好抉择评分/当前抉择评分
		happiness_sum_flow += happiness_flow;
		
		//计算方案部署后当前总的cost，如果流没有被安排进网络，就增加惩罚cost
		
		//统计网络latency
		double latencyVoting_flow=0;
		latencyVoting_flow=judge_sum_function(gv_flow,candiL_flow,winner_flow);

		cout << "winner = "<<winner_flow<<endl;
		cout << "第" << i << "轮整体满意度： " << happiness_flow/Maxreq << endl;
		cout << "多轮满意度：" << happiness_sum_flow/ ((i+1)*Maxreq) << endl;
		cout << "多轮整体延时和: " << latencyVoting_flow << endl;

		double maxUtil_Voting_flow=0;
		double loadNUM_flow=0;//统计网络负载
		for(int j=0;j<gv_flow.m;j++)
		{
			int src=gv_flow.incL[j]->src;
			int dst=gv_flow.incL[j]->dst;
			loadNUM_flow+=flowL_flow[winner_flow]->adj[src][dst];
			double capacity=gv_flow.incL[j]->capacity;
			if(maxUtil_Voting_flow<(flowL_flow[winner_flow]->adj[src][dst]/capacity))
				maxUtil_Voting_flow=flowL_flow[winner_flow]->adj[src][dst]/capacity;
		}
		cout<<"最大链路利用率: "<<maxUtil_Voting_flow<<endl;

		//文件记录
		outfile<<"Case "<<i<<" Flow Voting Result"<<endl;
		outfile << "winner = "<<winner_flow<<endl;
		outfile << "第" << i << "轮整体满意度： " << happiness_flow/Maxreq << endl;
		outfile << "多轮满意度：" << happiness_sum_flow/ ((i+1)*Maxreq) << endl;
		outfile << "多轮整体延时和: " << latencyVoting_flow << endl;
		outfile <<"最大链路利用率: "<<maxUtil_Voting_flow<<endl;
		outfile <<"网络负载"<<loadNUM_flow<<endl;

		app_happiness.push_back(happiness_sum_flow/ ((i+1)*Maxreq));
		app_delay.push_back(latencyVoting_flow);
		app_te.push_back(maxUtil_Voting_flow);
		app_load.push_back(loadNUM_flow);

		//胜利的方案部署
		for(int j=0;j<candiL_flow.size();j++)
			candiL_flow[j]->end_implement(gv_flow,winner_flow,candiL_flow);

		cout<<endl;

		//**************************************   End of Flow Voting    **************************************
		//**************************************   End of Flow Voting    **************************************


		//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@

		//算最完美方案，所有的路都第一个走得到的方案
		for(int j=0;j<Maxreq;j++)
			gv.cost_best[j] = gv.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow,flowL[0]->adj);

		//for(int j=0;j<Maxreq;j++)
			//cout<<"gv.cost_best "<<j<<" : "<<gv.cost_best[j]<<endl;

		//提方案
		for(int j=0;j<candiL.size();j++)
			candiL[j]->propose(gv);
		//cout<<"提出方案结束"<<endl;

		//评价方案
		for(int j=0;j<voterL.size();j++)
			voterL[j]->evaluate(gv,candiL);
		//cout<<"评价方案结束"<<endl;

		//投票算法, 投票算法的输入是从1开始的，但是judge是从0开始的，所以需要调整一下
		for(int j=0;j<voterL.size();j++)
			for(int k=0;k<candiL.size();k++)
			{
				//该操作在evaluate时候就惩罚----不用if(voterL[j]->judge[k]==0) table[j+1][k+1]=10000;//如果是0，说明流没有摆在网络中
				table[j+1][k+1]=voterL[j]->judge[k];
			}
		cout<<endl<<"voting uses ";
		int choice=voting_choice;//选择一种投票算法
		int winner=0;
		Voting vv(table,ranking,voterL.size(),candiL.size());
		winner=vv.voting(choice);
		winner=winner-1;//将结果还是按照从0开始编号

		outfile<<endl<<"beat_record: ";
		for(int i=1;i<=candiL.size();i++)
			outfile<<vv.beat_record[i]<<" ";
		outfile<<endl;

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
			cout<<i-1<<" judge: ";
			for(int j=1;j<=candiL.size();j++){
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
			outfile<<i-1<<" judge: ";
			for(int j=1;j<=candiL.size();j++){
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
		latencyVoting=judge_sum_function(gv,candiL,winner);

		cout << "winner = "<<winner<<endl;
		cout << "第" << i << "轮整体满意度： " << happiness/Maxreq << endl;
		cout << "多轮满意度：" << happiness_sum / ((i+1)*Maxreq) << endl;
		cout << "多轮整体延时和: " << latencyVoting << endl;

		double maxUtil_Voting=0;
		maxUtil_Voting=voterL[Maxreq]->judge[winner];
		cout<<"最大链路利用率: "<<maxUtil_Voting<<endl;

		double loadNUM_voting=0;
		for(int j=0;j<gv.m;j++)
		{
			int src=gv.incL[j]->src;
			int dst=gv.incL[j]->dst;
			loadNUM_voting+=candiL[winner]->adj[src][dst];
		}


		//文件记录
		outfile<<"Case "<<i<<" Voting Result"<<endl;
		outfile << "winner = "<<winner<<endl;
		outfile << "第" << i << "轮整体满意度： " << happiness/Maxreq << endl;
		outfile << "多轮满意度：" << happiness_sum / ((i+1)*Maxreq) << endl;
		outfile << "多轮整体延时和: " << latencyVoting << endl;
		outfile <<"最大链路利用率: "<<maxUtil_Voting<<endl;
		outfile <<"网络负载"<<loadNUM_voting<<endl;

		voting_happiness.push_back(happiness_sum / ((i+1)*Maxreq));
		voting_delay.push_back(latencyVoting);
		voting_te.push_back(maxUtil_Voting);
		voting_load.push_back(loadNUM_voting);

		//胜利的方案部署
		for(int j=0;j<candiL.size();j++)
			candiL[j]->end_implement(gv,winner,candiL);
		
		//修改neutral的te值
		neutral_low->te = maxUtil_Voting + 0.05;
		neutral_middle->te = maxUtil_Voting + 0.1;
		neutral_high->te = maxUtil_Voting + 0.2;

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
		
		double loadNUM_network=0;
		for(int j=0;j<gp.m;j++)
		{
			int src=gp.incL[j]->src;
			int dst=gp.incL[j]->dst;
			loadNUM_network+=gp.adj[src][dst];
		}

		//文件记录
		outfile<<"Case "<<i<<" LP Result"<<endl;
		outfile<<"单轮满意度： "<<judge_LP/Maxreq<<endl;
		outfile<<"多轮满意度： "<<judge_sum_LP/(Maxreq*(i+1))<<endl;
		outfile << "多轮整体延时和: " << latency_LP << endl;
		outfile<<"最大链路利用率: "<<result_LP<<endl;
		outfile <<"网络负载"<<loadNUM_network<<endl;

		network_happiness.push_back(judge_sum_LP/(Maxreq*(i+1)));
		network_delay.push_back(latency_LP);
		network_te.push_back(result_LP);
		network_load.push_back(loadNUM_network);

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

	//happiness results: 三行结果分别是app voting network的满意度
	outfile<<endl;
	outfile<<"happiness results: app voting network"<<endl;
	for(int i=0;i<caseN;i++)
	{
		outfile<<app_happiness[i]<<",";
	}
	outfile<<endl;
	for(int i=0;i<caseN;i++)
	{
		outfile<<voting_happiness[i]<<",";
	}
	outfile<<endl;
	for(int i=0;i<caseN;i++)
	{
		outfile<<network_happiness[i]<<",";
	}

	//delay results 
	outfile<<endl;
	outfile<<"delay results: app voting network"<<endl;
	for(int i=0;i<caseN;i++)
	{
		outfile<<app_delay[i]<<",";
	}
	outfile<<endl;
	for(int i=0;i<caseN;i++)
	{
		outfile<<voting_delay[i]<<",";
	}
	outfile<<endl;
	for(int i=0;i<caseN;i++)
	{
		outfile<<network_delay[i]<<",";
	}

	//te results
	outfile<<endl;
	outfile<<"te results: app voting network"<<endl;
	for(int i=0;i<caseN;i++)
	{
		outfile<<app_te[i]<<",";
	}
	outfile<<endl;
	for(int i=0;i<caseN;i++)
	{
		outfile<<voting_te[i]<<",";
	}
	outfile<<endl;
	for(int i=0;i<caseN;i++)
	{
		outfile<<network_te[i]<<",";
	}
	outfile<<endl;

	//load results
	outfile<<endl;
	outfile<<"load results: app voting network"<<endl;
	for(int i=0;i<caseN;i++)
	{
		outfile<<app_load[i]<<",";
	}
	outfile<<endl;
	for(int i=0;i<caseN;i++)
	{
		outfile<<voting_load[i]<<",";
	}
	outfile<<endl;
	for(int i=0;i<caseN;i++)
	{
		outfile<<network_load[i]<<",";
	}
	outfile<<endl;

	getchar();
	return 0;
}