#include"common.h"
#include"Flow.h"
#include"graph.h"
#include"LP.h"
#include"voting.h"
#include "res.h"

const int Inf=99999;
const int N=18;
const int M=42;
const int Maxreq=10;
const int Maxflow=5;
int judge_sum;
vector<Flow*> flowL;//定义全局变量

int main()
{
	srand((unsigned)time(NULL));
	VGraph gv("d:\\github\\CRANA_Voting\\graph2.txt");
	
	int caseN=2;
	vector<Req*> reqL;
	float table[M2C+1][N2C+1] = {0};
	int ranking[N2C+1]={0};//记录一种排序的投票人数
	float happiness_sum=0;
	
	for(int j=0;j<Maxreq;j++)
		ranking[j]=1;//每种投票结果有1个voter


	for(int i=0;i<caseN;i++)
	{
		//初始化
		for(int j=0;j<Maxreq;j++)
			for(int k=0;k<Maxreq;k++)
				table[j][k]=0;
		
		reqL.clear();
		if(i==0){
			for(int j=0;j<Maxreq;j++)
			{
				int a,b,c;
				while(c==0){
					a = rand()%N;
					b = rand()%N;
					c = 3 + rand()%Maxflow;
				}
				Req* r = new Req(a,b,c);
				reqL.push_back(r);
				Flow* flow_now = new Flow(j,a,b,c);
				flowL.push_back(flow_now);
			}
		}
		else{
			for(int j=0;j<Maxreq;j++)
			{
				int a,b,c;
				while(c==0){
					a = rand()%N;
					b = rand()%N;
					c = 3 + rand()%Maxflow;
				}
				Req* r = new Req(a,b,c);
				reqL.push_back(r);
				flowL[j]->flow_modify(a,b,c);
			}
		}

		//提方案
		for(int j=0;j<Maxreq;j++)
			flowL[j]->propose(gv);

		//评价
		for(int j=0;j<Maxreq;j++)
			flowL[j]->evaluate(gv);

		//投票算法
		for(int j=0;j<Maxreq;j++)
			for(int k=0;k<Maxreq;k++)
			{
				if(flowL[j]->judge[k]==0) table[j+1][k+1]=10000;//如果是0，说明流没有摆在网络中
				else table[i+1][j+1]=flowL[j]->judge[k];
			}
		cout<<"          voting result          "<<endl;
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



		//计算满意度
		float happiness=0;//一轮所有流的满意度和，越高越好,0<=满意度<=1
		for(int j=0;j<Maxreq;j++)
			happiness += table[j][j]/table[j][winner];//最好抉择评分/当前抉择评分
		happiness_sum += happiness;

		//计算方案部署后当前总的cost，如果流没有被安排进网络，就增加惩罚cost
		judge_sum_function(gv);
		for(int i=0;i<=Maxreq;i++)
			if(table[i][winner]=10000) judge_sum += MAXPATH * reqL[i]->flow;

		cout << "第" << i << "轮整体满意度： " << happiness/Maxreq << endl;
		cout << "多轮整体满意度和：" << happiness_sum / ((i+1)*10) << endl;
		cout << "多轮整体代价和: " << judge_sum << endl;

		//胜利的方案部署
		for(int j=0;j<Maxreq;j++)
		{
			for(int k1=0;k1<N;k1++)
				for(int k2=0;k2<N;k2++)
					flowL[j]->adj[k1][k2]=flowL[winner]->adj[k1][k2];
		}
		
	}//一个case结束
	getchar();
	return 0;
}