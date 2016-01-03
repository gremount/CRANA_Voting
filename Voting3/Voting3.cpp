#include"common.h"
#include"Flow.h"
#include"graph.h"
#include"voting.h"
#include "res.h"
#include "LP.h"


//t3
const int Inf=99999;
const int N=3;//���еĵ���
const int M=6;//�����������
const int Maxreq=3;//һ��case������������
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=2;//case����
const int Maxflow=5;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=10;//���Ĵ�С��ʼ��Χ

/*
//graph_all
const int Inf=99999;
const int N=20;//���еĵ���
const int M=380;//�����������
const int Maxreq=10;//һ��case������������
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=10;//case����
const int Maxflow=30;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=1;//���Ĵ�С��ʼ��Χ
*/

/*
//graph_Compuserve
const int Inf=99999;
const int N=11;//���еĵ���
const int M=28;//�����������
const int Maxreq=10;//һ��case������������
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=7;//case����
const int Maxflow=10;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=1;//���Ĵ�С��ʼ��Χ
*/

/*
//graph_ATT
const int Inf=99999;
const int N=25;//���еĵ���
const int M=112;//�����������
const int Maxreq=10;//һ��case������������
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=6;//case����
const int Maxflow=20;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=1;//���Ĵ�С��ʼ��Χ
*/

//�����ͼ����Ҫ�޸ģ� ����Ĳ��� + ͼ���� + req����Ĳ���

int main()
{
	srand((unsigned)time(NULL));
	VGraph gv("d:\\github\\CRANA_Voting\\t3.txt");//Voting�õ�ͼ
	PGraph gp("d:\\github\\CRANA_Voting\\t3.txt");//LP�õ�ͼ
	vector<Flow*> flowL;//��¼���е���ʵ��
	ofstream outfile("d:\\github\\result.txt");//���һ��case�Ľ��
	ofstream req_outfile("d:\\github\\req_outfile.txt");

	outfile<<"graph_ATT�������� caseN: "<<caseN<<endl;
	outfile<<"flow Range: "<<Begin_num<<"--"<<Maxflow+Begin_num-1<<endl<<endl;

	double judge_LP=0,judge_sum_LP=0;
	
	vector<Req*> reqL;
	double table[M2C+1][N2C+1] = {0};
	int ranking[N2C+1]={0};//��¼һ�������ͶƱ����
	double happiness_sum=0;
	
	for(int j=0;j<Maxreq;j++)
		ranking[j]=1;//ÿ��ͶƱ�����1��voter,���Ϊ2��˵���÷����еõ�����voter��Ʊ

	
	for(int i=0;i<caseN;i++)
	{
		cout<<endl<<"*************************"<<" case "<<i<<"*************************"<<endl;
		//��ʼ��
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

		//�����������������е�·����һ���ߵõ��ķ���
		for(int j=0;j<Maxreq;j++)
			gv.cost_best[j] = reqL[j]->flow * gv.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow,flowL[0]->adj);

		//for(int j=0;j<Maxreq;j++)
			//cout<<"gv.cost_best "<<j<<" : "<<gv.cost_best[j]<<endl;

		//�᷽��
		for(int j=0;j<Maxreq;j++)
			flowL[j]->propose(gv,flowL);

		//���۷���
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
		double happiness=0;//һ��������������Ⱥͣ�Խ��Խ��,0<=�����<=1
		for(int j=1;j<=Maxreq;j++)
			happiness += gv.cost_best[j-1]/table[j][winner];//��þ�������/��ǰ��������
		happiness_sum += happiness;

		//���㷽�������ǰ�ܵ�cost�������û�б����Ž����磬�����ӳͷ�cost
		
		//ͳ������latency
		double latencyVoting=0;
		latencyVoting=judge_sum_function(gv,flowL,winner-1);

		for(int j=0;j<Maxreq;j++)
			if(table[j+1][winner]==10000) {
				cout<<"�����ͷ�����"<<endl;
				latencyVoting += Maxpath * reqL[j]->flow;
			}
		cout << "��" << i << "����������ȣ� " << happiness/Maxreq << endl;
		cout << "��������ȣ�" << happiness_sum / ((i+1)*Maxreq) << endl;
		cout << "����������ʱ��: " << latencyVoting << endl;
		
		double maxUtil_Voting=0;
		for(int j=0;j<gp.m;j++)
		{
			int src=gv.incL[j]->src;
			int dst=gv.incL[j]->dst;
			double capacity=gv.incL[j]->capacity;
			if(maxUtil_Voting<(flowL[winner-1]->adj[src][dst]/capacity))
				maxUtil_Voting=flowL[winner-1]->adj[src][dst]/capacity;
		}
		cout<<"�����·������: "<<maxUtil_Voting<<endl;

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
		
		//���������
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
		cout<<"��������ȣ� "<<judge_LP/Maxreq<<endl;
		cout<<"��������ȣ� "<<judge_sum_LP/(Maxreq*(i+1))<<endl;

		double latency_LP=0;
		latency_LP=judge_sum_LP_function(gp,flowL);
		cout << "����������ʱ��: " << latency_LP << endl;
		cout<<"�����·������: "<<result_LP<<endl;
		
		/*
		cout<<"�������ʣ����·������: "<<result_LP<<endl;
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
			outfile<<"��������ȣ� "<<judge_LP/Maxreq<<endl;
			outfile<<"��������ȣ� "<<judge_sum_LP/(Maxreq*(i+1))<<endl;
			outfile<<"����������ۺ�: "<<result_sum_LP<<endl;
		}
		*/
		

	}//һ��case����
	getchar();
	return 0;
}