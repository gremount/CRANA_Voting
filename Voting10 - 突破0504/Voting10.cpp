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
const int N=3;//���еĵ���
const int M=6;//�����������
const int Maxreq=3;//һ��case������������
const int Voternum=4;//flow+network
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=2;//case����
const int Maxflow=5;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=10;//���Ĵ�С��ʼ��Χ
*/


//graph_all
const int Inf=99999;
const int N=20;//���еĵ���
const int M=380;//�����������
const int Maxreq=10;//һ��case������������
const int Voternum=11;//flow+network
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=10;//case����
const int Maxflow=30;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=1;//���Ĵ�С��ʼ��Χ


/*
//graph_Compuserve
const int Inf=99999;
const int N=11;//���еĵ���
const int M=28;//�����������
const int Maxreq=10;//һ��case������������
const int Voternum=11;//flow+network
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=6;//case����
const int Maxflow=10;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=1;//���Ĵ�С��ʼ��Χ
*/

/*
//graph_ATT
const int Inf=99999;
const int N=25;//���еĵ���
const int M=112;//�����������
const int Maxreq=10;//һ��case������������
const int Voternum=11;//flow+network
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=8;//case����
const int Maxflow=15;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=5;//���Ĵ�С��ʼ��Χ
*/

//�����ͼ����Ҫ�޸ģ� ����Ĳ��� + ͼ���� + req����Ĳ���

int main()
{
	srand((unsigned)time(NULL));
	VGraph gv("graph_all.txt");//Voting�õ�ͼ
	PGraph gp("graph_all.txt");//LP�õ�ͼ
	vector<Voter*> flowL;//��¼���е���ʵ��
	vector<Voter*> voterL;//��¼���е�ͶƱ��
	ofstream outfile("result.txt");//���һ��case�Ľ��
	ofstream req_outfile("req_outfile.txt");

	outfile<<"graph_all�������� caseN: "<<caseN<<endl;
	outfile<<"flow Range: "<<Begin_num<<"--"<<Maxflow+Begin_num-1<<endl<<endl;

	
	
	vector<Req*> reqL;
	double table[M2C+1][N2C+1] = {0};
	int ranking[N2C+1]={0};//��¼һ�������ͶƱ����
	double happiness_sum=0;
	
	for(int j=1;j<=Voternum;j++)
		ranking[j]=1;//ÿ��ͶƱ�����1��voter,���Ϊ2��˵���÷����еõ�����voter��Ʊ

	Voter* net_lb = new Network_LB(Maxreq,1);// Maxreq��ͶƱ��id, 1 ��ʾ����ͶƱ��
	ranking[Voternum]=5;//������ͶƱ�߸����Ʊ��

	for(int i=0;i<caseN;i++)
	{
		cout<<endl<<"*************************"<<" case "<<i<<"*************************"<<endl;
		outfile<<endl<<"*************************"<<" case "<<i<<"*************************"<<endl;
		//��ʼ��
		for(int j=0;j<Voternum;j++)
			for(int k=0;k<Voternum;k++)
				table[j][k]=0;
		
		req_outfile<<"case "<<i<<endl;

		
		//���³���ʱ��ֹÿ��case���ᴴ����Req���µ��ڴ�й¶
		vector<Req*>::iterator it,iend;
		iend=reqL.end();
		for(it=reqL.begin();it!=iend;it++)	
			delete(*it);
		
		//ɾ����һ��case��������
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

		//�����������������е�·����һ���ߵõ��ķ���
		for(int j=0;j<Maxreq;j++)
			gv.cost_best[j] = gv.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow,flowL[0]->adj);

		//for(int j=0;j<Maxreq;j++)
			//cout<<"gv.cost_best "<<j<<" : "<<gv.cost_best[j]<<endl;

		//�᷽��
		for(int j=0;j<voterL.size();j++)
			voterL[j]->propose(gv,flowL);
		//cout<<"�����������"<<endl;

		//���۷���
		for(int j=0;j<voterL.size();j++)
			voterL[j]->evaluate(gv,voterL);
		//cout<<"���۷�������"<<endl;

		//ͶƱ�㷨, ͶƱ�㷨�������Ǵ�1��ʼ�ģ�����judge�Ǵ�0��ʼ�ģ�������Ҫ����һ��
		for(int j=0;j<voterL.size();j++)
			for(int k=0;k<voterL.size();k++)
			{
				if(voterL[j]->judge[k]==0) table[j+1][k+1]=10000;//�����0��˵����û�а���������
				else table[j+1][k+1]=voterL[j]->judge[k];
			}
		cout<<endl<<"voting uses ";
		int choice=1;//ѡ��һ��ͶƱ�㷨
		int winner=0;
		Voting vv(table,ranking,voterL.size(),voterL.size());
		winner=vv.voting(choice);
		winner=winner-1;//��������ǰ��մ�0��ʼ���

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


		//���������
		double happiness=0;//һ��������������Ⱥͣ�Խ��Խ��,0<=�����<=1
		for(int j=1;j<=Maxreq;j++)
			happiness += gv.cost_best[j-1]/table[j][winner+1];//��þ�������/��ǰ��������
		happiness_sum += happiness;

		//���㷽�������ǰ�ܵ�cost�������û�б����Ž����磬�����ӳͷ�cost
		
		//ͳ������latency
		double latencyVoting=0;
		latencyVoting=judge_sum_function(gv,voterL,winner);

		for(int j=0;j<Maxreq+1;j++)
			if(table[j+1][winner+1]==10000) {
				cout<<"�����ͷ�����"<<endl;
				latencyVoting += Maxpath * reqL[j]->flow;
			}
		cout << "winner = "<<winner<<endl;
		cout << "��" << i << "����������ȣ� " << happiness/Maxreq << endl;
		cout << "��������ȣ�" << happiness_sum / ((i+1)*Maxreq) << endl;
		cout << "����������ʱ��: " << latencyVoting << endl;

		double maxUtil_Voting=0;
		maxUtil_Voting=voterL[Maxreq]->judge[winner];
		cout<<"�����·������: "<<maxUtil_Voting<<endl;

		//�ļ���¼
		outfile<<"Case "<<i<<" Voting Result"<<endl;
		outfile << "winner = "<<winner<<endl;
		outfile << "��" << i << "����������ȣ� " << happiness/Maxreq << endl;
		outfile << "��������ȣ�" << happiness_sum / ((i+1)*Maxreq) << endl;
		outfile << "����������ʱ��: " << latencyVoting << endl;
		outfile <<"�����·������: "<<maxUtil_Voting<<endl;

		//ʤ���ķ�������
		for(int j=0;j<voterL.size();j++)
			voterL[j]->end_implement(gv,winner,voterL);
		
		cout<<endl;

		//@@@@@@@@@@@@@@@@@End of Voting@@@@@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@@End of Voting@@@@@@@@@@@@@@@@@@@@@@@@@

		//�ֶι滮����
		cout<<endl<<"			LP result			"<<endl;
		
		//judge_LP��¼����������ܺͣ�judge_sum_LP��¼����������ܺ�
		double judge_LP=0;
		static double judge_sum_LP=0;

		//���Ų������
		for(int j=0;j<Maxreq;j++)
			gp.cost_best[j] =gp.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow);

		//���Թ滮����
		double result_LP=0;//TE���
		result_LP=LP(&gp,reqL);
		
		//���������
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
		cout<<"��������ȣ� "<<judge_LP/Maxreq<<endl;
		cout<<"��������ȣ� "<<judge_sum_LP/(Maxreq*(i+1))<<endl;

		double latency_LP=0;
		latency_LP=judge_sum_LP_function(gp);
		cout << "����������ʱ��: " << latency_LP << endl;
		cout<<"�����·������: "<<result_LP<<endl;
		
		//�ļ���¼
		outfile<<"Case "<<i<<" LP Result"<<endl;
		outfile<<"��������ȣ� "<<judge_LP/Maxreq<<endl;
		outfile<<"��������ȣ� "<<judge_sum_LP/(Maxreq*(i+1))<<endl;
		outfile << "����������ʱ��: " << latency_LP << endl;
		outfile<<"�����·������: "<<result_LP<<endl;

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