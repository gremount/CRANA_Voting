#include"common.h"
#include"Flow.h"
#include"graph.h"
#include"voting.h"
#include "res.h"

const int Inf=99999;
const int N=18;
const int M=42;
const int Maxreq=10;

const int caseN=6;
const int Maxflow=3;	
const int Begin_num=3;

int judge_sum;


int main()
{
	srand((unsigned)time(NULL));
	VGraph gv("d:\\github\\CRANA_Voting\\graph2.txt");
	vector<Flow*> flowL;//记录所有的流实例

	
	vector<Req*> reqL;
	float table[M2C+1][N2C+1] = {0};
	int ranking[N2C+1]={0};//记录一种排序的投票人数
	float happiness_sum=0;
	
	for(int j=0;j<Maxreq;j++)
		ranking[j]=1;//每种投票结果有1个voter


	for(int i=0;i<caseN;i++)
	{
		cout<<endl<<"*************************"<<" case "<<i<<"*************************"<<endl;
		//初始化
		for(int j=0;j<Maxreq;j++)
			for(int k=0;k<Maxreq;k++)
				table[j][k]=0;
		
		reqL.clear();
		gv.reqL.clear();
		if(i==0){
			for(int j=0;j<Maxreq;j++)
			{
				int a=0,b=0,c=0;
				while(1){
					//a = rand()%N;
					//b = rand()%N;
					c = Begin_num + rand()%Maxflow;
					if(c!=0) break;
					//if(a!=b && c!=0) break;
				}
				a=1;b=17;
				Req* r = new Req(a,b,c);
				reqL.push_back(r);
				gv.reqL.push_back(r);
				Flow* flow_now = new Flow(j,a,b,c);
				flowL.push_back(flow_now);
			}
		}
		else{
			for(int j=0;j<Maxreq;j++)
			{
				int a=0,b=0,c=0;
				while(1){
					//a = rand()%N;
					//b = rand()%N;
					c = Begin_num + rand()%Maxflow;
					if(c!=0) break;
					//if(a!=b && c!=0) break;
				}
				a=1;b=17;
				Req* r = new Req(a,b,c);
				reqL.push_back(r);
				gv.reqL.push_back(r);
				flowL[j]->flow_modify(a,b,c);
			}
		}

		//提方案
		for(int j=0;j<Maxreq;j++)
			flowL[j]->propose(gv,flowL);

		//评价
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
		float happiness=0;//一轮所有流的满意度和，越高越好,0<=满意度<=1
		for(int j=1;j<=Maxreq;j++)
			happiness += table[j][j]/table[j][winner];//最好抉择评分/当前抉择评分
		happiness_sum += happiness;

		//计算方案部署后当前总的cost，如果流没有被安排进网络，就增加惩罚cost
		judge_sum_function(gv,flowL,winner-1);
		for(int j=0;j<Maxreq;j++)
			if(table[j+1][winner]=10000) judge_sum += MAXPATH * reqL[j]->flow;

		cout << "第" << i << "轮整体满意度： " << happiness/Maxreq << endl;
		cout << "多轮整体满意度和：" << happiness_sum / ((i+1)*10) << endl;
		cout << "多轮整体代价和: " << judge_sum << endl;

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

	}//一个case结束
	getchar();
	return 0;
}