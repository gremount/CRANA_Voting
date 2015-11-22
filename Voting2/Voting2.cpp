#include"common.h"
#include"Flow.h"
#include"graph.h"
#include"voting.h"
#include "res.h"
#include "LP.h"

const int Inf=99999;
const int N=18;
const int M=42;
const int Maxreq=10;

const int caseN=7;
const int Maxflow=5;	
const int Begin_num=1;

int judge_sum;


int main()
{
	srand((unsigned)time(NULL));
	VGraph gv("d:\\github\\CRANA_Voting\\graph2.txt");
	PGraph gp("d:\\github\\CRANA_Voting\\graph2.txt");
	vector<Flow*> flowL;//��¼���е���ʵ��

	double judge_LP=0,judge_sum_LP=0,result_sum_LP=0;
	judge_sum=0;
	
	vector<Req*> reqL;
	float table[M2C+1][N2C+1] = {0};
	int ranking[N2C+1]={0};//��¼һ�������ͶƱ����
	float happiness_sum=0;
	
	for(int j=0;j<Maxreq;j++)
		ranking[j]=1;//ÿ��ͶƱ�����1��voter

	
	for(int i=0;i<caseN;i++)
	{
		cout<<endl<<"*************************"<<" case "<<i<<"*************************"<<endl;
		//��ʼ��
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

		//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@

		//�����������������е�·����һ���ߵõ��ķ���
		for(int j=0;j<Maxreq;j++)
			gv.cost_best[j] = gv.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow,flowL[0]->adj);

		//�᷽��
		for(int j=0;j<Maxreq;j++)
			flowL[j]->propose(gv,flowL);

		//����
		for(int j=0;j<Maxreq;j++)
			flowL[j]->evaluate(gv,flowL);

		//ͶƱ�㷨
		for(int j=0;j<Maxreq;j++)
			for(int k=0;k<Maxreq;k++)
			{
				if(flowL[j]->judge[k]==0) table[j+1][k+1]=10000;//�����0��˵����û�а���������
				else table[j+1][k+1]=flowL[j]->judge[k];
			}
		cout<<endl<<"voting uses ";
		int choice=1;//ѡ��һ��ͶƱ�㷨
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
		

		//���������
		float happiness=0;//һ��������������Ⱥͣ�Խ��Խ��,0<=�����<=1
		for(int j=1;j<=Maxreq;j++)
			happiness += table[j][j]/table[j][winner];//��þ�������/��ǰ��������
		happiness_sum += happiness;

		//���㷽�������ǰ�ܵ�cost�������û�б����Ž����磬�����ӳͷ�cost
		judge_sum_function(gv,flowL,winner-1);
		for(int j=0;j<Maxreq;j++)
			if(table[j+1][winner]==10000) {
				cout<<"�����ͷ�����"<<endl;
				judge_sum += MAXPATH * reqL[j]->flow;
			}
		cout << "��" << i << "����������ȣ� " << happiness/Maxreq << endl;
		cout << "��������ȣ�" << happiness_sum / ((i+1)*10) << endl;
		cout << "����������ۺ�: " << judge_sum << endl;

		//ʤ���ķ�������
		
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

		//�ֶι滮����
		cout<<endl<<"			LP result			"<<endl;
		
		//���Ų������
		for(int j=0;j<Maxreq;j++)
			gp.cost_best[j] = reqL[j]->flow * gp.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow);

		//���Թ滮����
		double result_LP=0;
		result_LP=LP(&gp,reqL);

		//�����Թ滮�����cost
		result_sum_LP += result_LP;
		//cout<<"cost of this case is: "<<result_LP<<endl;
		//cout<<"����������ۺ�: "<<result_sum_LP<<endl;
		
		//���������
		if(result_LP==Inf)
			judge_LP=0;
		else
		{
			judge_LP=0;
			//for(int i=0;i<reqN;i++)
			//	cout<<g.cost_best[i]<<" "<<g.cost_LP[i]<<endl;
			for(int j=0;j<Maxreq;j++)
				judge_LP += gp.cost_best[j]/gp.cost_LP[j];
		}
		judge_sum_LP += judge_LP;
		cout<<"��������ȣ� "<<judge_LP/Maxreq<<endl;
		cout<<"��������ȣ� "<<judge_sum_LP/(Maxreq*(i+1))<<endl;
		cout<<"����������ۺ�: "<<result_sum_LP<<endl;


	}//һ��case����
	getchar();
	return 0;
}