#include"common.h"
#include"app.h"
#include"graph.h"
#include"voting.h"
#include "res.h"
#include "network_te.h"
#include "network_delay.h"

const int Inf=9999;
const int Rinf=0.001;
const int APPNUM=5;//Ӧ������

/*
//graph_t3
const int N=3;//���еĵ���
const int M=6;//�����������
const int Maxreq=2;//һ��case������������
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=2;//case����
const int Maxflow=5;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=10;//���Ĵ�С��ʼ��Χ
*/

/*
//graph_t4
const int N=4;//���еĵ���
const int M=8;//�����������
const int Maxreq=3;//һ��case������������
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=2;//case����
const int Maxflow=5;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=10;//���Ĵ�С��ʼ��Χ
*/

/*
//graph_all
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
const int N=11;//���еĵ���
const int M=28;//�����������
const int Maxreq=10;//һ��case������������
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=6;//case����
const int Maxflow=10;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=1;//���Ĵ�С��ʼ��Χ
*/

/*
//graph_ATT
const int N=25;//���еĵ���
const int M=112;//�����������
const int Maxreq=10;//һ��case������������
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=6;//case����
const int Maxflow=15;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=5;//���Ĵ�С��ʼ��Χ
*/


//graph_ATT_big
const int N=25;//���еĵ���
const int M=112;//�����������
const int Maxreq=20;//һ��case������������
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=50;//case����
const int Maxflow=20;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=5;//���Ĵ�С��ʼ��Χ


/*
//graph_all_big
const int N=20;//���еĵ���
const int M=380;//�����������
const int Maxreq=20;//һ��case������������
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=50;//case����
const int Maxflow=40;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=5;//���Ĵ�С��ʼ��Χ
*/
//�����ͼ����Ҫ�޸ģ� ����Ĳ��� + ͼ���� + req����Ĳ���

int main()
{
	srand((unsigned)time(NULL));
	string address="graph_ATT_big.txt";
	ofstream outfile("result.txt");//ʵ����
	ofstream req_outfile("reqs.txt");//ʵ���õ�������
	int testNum=100;//ʵ�����

	outfile<<address<<"��������"<<"  testNum="<<testNum<<endl;
	outfile<<"APPNUM="<<APPNUM<<endl;
	outfile<<"Maxreq="<<Maxreq<<endl;
	outfile<<"caseN: "<<caseN<<endl;
	outfile<<"flow Range: "<<Begin_num<<"--"<<Maxflow+Begin_num-1<<endl<<endl;

	for(int test=0;test<testNum;test++)
	{
		//outfile<<"*********************  test "<<test<<"*********************"<<endl;
		vector<APP*> appL;//��¼���е�APP
		vector<Req*> reqL;//������

		VGraph gv(address);//Voting�õ�ͼ
		TENetworkGraph gn_te(address);//TEȫ���Ż���ͼ
		DelayNetworkGraph gn_delay(address);//��ʱȫ���Ż���ͼ

		double happiness_sum_teLP=0;//te����Ķ��������
		double happiness_sum_delayLP=0;//delay����Ķ��������
		double happiness_sum=0;//voting�Ķ��������

		double table[M2C+1][N2C+1] = {0};//ͶƱ�õ�����
		int ranking[N2C+1]={0};//��¼һ�������ͶƱ����
		
		for(int k=0;k<APPNUM;k++)
			appL.push_back(new APP(k));

		for(int j=1;j<=Maxreq;j++)
			ranking[j]=1;//ÿ��ͶƱ�����1��voter,���Ϊ2��˵���÷����еõ�����voter��Ʊ

		//ͳ������case���
		vector<double> oneCaseHappiness_Voting;//Voting����ĵ��������
		vector<double> multiCaseHappiness_Voting;//Voting����Ķ��������
		vector<double> oneCaseS2_Voting;//Voting����ĵ�������ȷ���
		vector<double> delay_Voting;//Voting�������ʱ
		vector<double> TE_Voting;////Voting����������·������

		vector<double> oneCaseHappiness_TENetwork;//��TEΪĿ�꼯���Ż�����ĵ��������
		vector<double> multiCaseHappiness_TENetwork;
		vector<double> delay_TENetwork;
		vector<double> TE_TENetwork;

		vector<double> oneCaseHappiness_DelayNetwork;//��DelayΪĿ�꼯���Ż�����ĵ��������
		vector<double> multiCaseHappines_DelayNetwork;
		vector<double> oneCaseS2_DelayNetwork;
		vector<double> delay_DelayNetwork;
		vector<double> TE_DelayNetwork;
	
		vector<double> s2_DelayNetwork_Voting;//DelayNetwork�ķ������ͶƱ�Ľ��

		//ÿ��case��һ����(ͬʱ�����Maxreq����)
		for(int i=0;i<caseN;i++)
		{
			cout<<endl<<"*************************"<<" case "<<i<<"*************************"<<endl;
			//��ʼ��
			for(int j=0;j<Maxreq;j++)
				for(int k=0;k<Maxreq;k++)
					table[j][k]=0;
			
			reqL.clear();
			gv.reqL.clear();

			for(int j=0;j<APPNUM;j++)
				appL[j]->init();

			//���³���ʱ��ֹÿ��case���ᴴ����Req���µ��ڴ�й¶
			vector<Req*>::iterator it,iend;
			iend=reqL.end();
			for(it=reqL.begin();it!=iend;it++)	
				delete(*it);
		
			//�����������: (id app_id src dst flow)
			req_outfile<<"case "<<i<<endl;
			for(int j=0;j<Maxreq;j++)
			{
				int app_id=0,a=0,b=0,c=0;
				while(1){
					c = Begin_num + rand()%Maxflow;
					a = rand()%N;
					b = rand()%N;
					app_id=rand()%APPNUM;
					if(a!=b && c!=0) break;
				}

				if(j==0){app_id=j;a=0;b=3;c=5;}
				else if(j==1){app_id=j;a=1;b=3;c=5;}
				else if(j==2){app_id=j;a=0;b=1;c=5;}

				Req* r = new Req(j,app_id,a,b,c);
				reqL.push_back(r);
				gv.reqL.push_back(r);
				req_outfile<<j<<" "<<app_id<<" "<<a<<" "<<b<<" "<<c<<endl;
			}
			

			//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@
			//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@

			//�����������������е�·����һ���ߵõ��ķ���

			for(int j=0;j<APPNUM;j++)
			{
				vector<Req*> reqPL_app;
				for(int i=0;i<reqL.size();i++)
				{	
					if(j==reqL[i]->app_id) reqPL_app.push_back(reqL[i]);
				}
				gv.network_delay(reqPL_app,j);
			}

			//for(int j=0;j<Maxreq;j++)
				//cout<<"gv.cost_best "<<j<<" : "<<gv.cost_best[j]<<endl;

			//�᷽��
			for(int j=0;j<APPNUM;j++){
				cout<<"app "<<j<<" proposal"<<endl;
				appL[j]->propose(gv,appL);
			}

			//���۷���
			for(int j=0;j<APPNUM;j++)
				appL[j]->evaluate(gv,appL);

			//ͶƱ�㷨
			for(int j=0;j<APPNUM;j++)
				for(int k=0;k<APPNUM;k++)
				{
					if(appL[j]->judge[k]==0) table[j][k]=10000;//�����0��˵����û�а���������
					else table[j][k]=appL[j]->judge[k];
				}
			cout<<endl<<"voting uses ";
			int choice=1;//ѡ��һ��ͶƱ�㷨
			int winner=0;
			Voting vv(table,ranking,APPNUM,APPNUM);
			winner=vv.voting(choice);

			//****************************   �������ȷ�������С����ȷ���� ����
			//���� �������ȷ��� �� ��С����ȷ����
			double happinessVotingMax=0;//��������
			double happinessVotingTemp=0;
			int happinessVotingLoc=0;
			double s2Min=Inf;//��С����ȷ���
			double s2Temp=0;
			int s2Loc=0;
			double s2Min_Happiness=0;//��С����ȷ�����������
		
			for(int k=0;k<APPNUM;k++)
			{
				happinessVotingTemp=0;
				for(int j=0;j<APPNUM;j++)
					happinessVotingTemp+= gv.cost_best[j]/table[j][k];//��þ�������/��ǰ��������
				if(happinessVotingMax<happinessVotingTemp/APPNUM){
					happinessVotingMax=happinessVotingTemp/APPNUM;
					happinessVotingLoc=k;
				}
			
				s2Temp=0;
				for(int j=0;j<APPNUM;j++)
					s2Temp+= (gv.cost_best[j]/table[j][k]-happinessVotingTemp/APPNUM) * (gv.cost_best[j]/table[j][k]-happinessVotingTemp/APPNUM);
				if(s2Min>s2Temp/APPNUM){
					s2Min=s2Temp/APPNUM;
					s2Loc=k;
					s2Min_Happiness=happinessVotingTemp/APPNUM;
				}
			}

			//���� �������ȷ��� �� Ӧ������ȷ���
			double s2VotingMax=0;
			//�������ȷ�����Ӧ������ȵ�ƽ��ֵ
			double happinessVotingMaxAvg=happinessVotingMax/APPNUM;
			for(int j=0;j<APPNUM;j++){
				if(gv.cost_best[j]==0)continue;//û�����������APP��û������ȵ�����
				s2VotingMax+=(gv.cost_best[j]/table[j][happinessVotingLoc]-happinessVotingMaxAvg) * (gv.cost_best[j]/table[j][happinessVotingLoc]-happinessVotingMaxAvg);
			}
			s2VotingMax=s2VotingMax/APPNUM;
		
			cout <<"�������ȷ����Ľ��"<<endl;
			cout << "��" << i << "����������ȣ� " << happinessVotingMax << endl;
			cout << "��" << i << "�����������ȷ�� " << s2VotingMax << endl;
			cout <<endl;

			cout <<"��С����� ���� �����Ľ��"<<endl;
			cout << "��" << i << "����������ȣ� " << s2Min_Happiness << endl;
			cout << "��" << i << "�����������ȷ�� " << s2Min << endl;
			cout <<endl;

			//****************************   END OF �������ȷ�������С����ȷ���� ����  

			winner=s2Loc;
			cout<<"crana voting method : " << winner << endl;
			/*
			if (choice == 1)
				cout << "schulze method : " << winner << endl;
			else if (choice == 2)
				cout << "comulative method: " << winner << endl;
			else if (choice == 3)
				cout << "copeland condorcet method: " << winner << endl;
			else
				cout << "ranked pairs method: " << winner << endl;
				*/
		
			// table show
			for(int i=0;i<APPNUM;i++)
			{
				cout<<"app ";
				cout.setf(ios::right);
				cout.width(3);
				cout<<i<<" judge: ";
				for(int j=0;j<APPNUM;j++){
					cout.setf(ios::left);
					cout.width(5);
					cout<<table[i][j]<<" ";
				}
				cout<<endl;
			}
			cout<<endl;

			//����ͶƱwinner�����
			double happiness=0;//һ��������������Ⱥͣ�Խ��Խ��,0<=�����<=1
			for(int j=0;j<APPNUM;j++)
				happiness += gv.cost_best[j]/table[j][winner];//��þ�������/��ǰ��������
			happiness_sum += happiness;

			//����ͶƱwinner��Ӧ������ȷ���
			double s2_voting=0;
			double happiness_avg=happiness/APPNUM;
			for(int j=0;j<APPNUM;j++)
			{
				if(gv.cost_best[j]==0) continue;
				s2_voting+=(gv.cost_best[j]/table[j][winner]-happiness_avg) * (gv.cost_best[j]/table[j][winner]-happiness_avg);
			}
			s2_voting=s2_voting/APPNUM;

			//ͳ������latency
			double latencyVoting=0;
			latencyVoting=judge_sum_function(gv,appL,winner);

			/*
			for(int j=0;j<Maxreq;j++)
				if(table[j][winner]==10000) {
					cout<<"�����ͷ�����"<<endl;
					latencyVoting += Maxpath * reqL[j]->flow;
				}
				*/

			cout <<"Voting Winner �����Ľ��"<<endl;
			cout << "��" << i << "����������ȣ� " << happiness/APPNUM << endl;
			cout << "��������ȣ�" << happiness_sum / ((i+1)*APPNUM) << endl;
			if(i==caseN-1){
				outfile << happiness_sum / ((i+1)*APPNUM) << ",";
			}
			cout << "��" << i << "�����������ȷ�� " << s2_voting << endl;
			cout << "����������ʱ��: " << latencyVoting << endl;
		
			double maxUtil_Voting=0;
			for(int j=0;j<gv.m;j++)
			{
				int src=gv.incL[j]->src;
				int dst=gv.incL[j]->dst;
				double capacity=gv.incL[j]->capacity;
				if(maxUtil_Voting<(appL[winner]->adj[src][dst]/capacity))
					maxUtil_Voting=appL[winner]->adj[src][dst]/capacity;
			}
			cout<<"�����·������: "<<maxUtil_Voting<<endl;

			//��¼������������
			oneCaseHappiness_Voting.push_back(happiness/APPNUM);
			multiCaseHappiness_Voting.push_back(happiness_sum / ((i+1)*APPNUM));
			oneCaseS2_Voting.push_back(s2_voting);
			delay_Voting.push_back(latencyVoting);
			TE_Voting.push_back(maxUtil_Voting);
		
			//ʤ���ķ�����������Ӧ�õ�adj��
			appL[winner]->end_implement(gv,appL);
			cout<<endl;

			//@@@@@@@@@@@@@@@@@End of Voting@@@@@@@@@@@@@@@@@@@@@@@@@
			//@@@@@@@@@@@@@@@@@End of Voting@@@@@@@@@@@@@@@@@@@@@@@@@

			//@@@@@@@@@@@@@@@@@Begin of network_te@@@@@@@@@@@@@@@@@@@@@@@@@
			//@@@@@@@@@@@@@@@@@Begin of network_te@@@@@@@@@@@@@@@@@@@@@@@@@

			//TEȫ���Ż��滮����
			cout<<endl<<"			network_te result			"<<endl;
		
			//���Ų������
			for(int j=0;j<Maxreq;j++)
				gn_te.cost_best[j] =gn_te.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow);

			//���Թ滮����
			double result_network_te=0;
			result_network_te=network_te(&gn_te,reqL);
		

			//���������
			double happiness_teLP=0;//���������ͳ��
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
			cout<<"��������ȣ� "<<happiness_teLP/Maxreq<<endl;
			cout<<"��������ȣ� "<<happiness_sum_teLP/(Maxreq*(i+1))<<endl;
			if(i==caseN-1) outfile<<happiness_sum_teLP/(Maxreq*(i+1))<<",";

			double latency_teLP=0;
			latency_teLP=delay_TENetworkGraph(gn_te);
			cout << "����������ʱ��: " << latency_teLP << endl;
			cout<<"�����·������: "<<result_network_te<<endl;
		
			//��¼������������
			oneCaseHappiness_TENetwork.push_back(happiness_teLP/Maxreq);
			multiCaseHappiness_TENetwork.push_back(happiness_sum_teLP/(Maxreq*(i+1)));
			delay_TENetwork.push_back(latency_teLP);
			TE_TENetwork.push_back(result_network_te);

			//@@@@@@@@@@@@@@@@@End of network_te@@@@@@@@@@@@@@@@@@@@@@@@@
			//@@@@@@@@@@@@@@@@@End of network_te@@@@@@@@@@@@@@@@@@@@@@@@@
		
			//@@@@@@@@@@@@@@@@@Begin of network_delay@@@@@@@@@@@@@@@@@@@@@@@@@
			//@@@@@@@@@@@@@@@@@Begin of network_delay@@@@@@@@@@@@@@@@@@@@@@@@@

			//Delayȫ���Ż��滮����
			cout<<endl<<"			network_delay result			"<<endl;
		
			//���Ų������
			for(int j=0;j<Maxreq;j++)
				gn_delay.cost_best[j] =gn_delay.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow);

			//���Թ滮����
			double result_network_delay=0;//������ʱ
			result_network_delay=network_delay(&gn_delay,reqL);
		
			/*
			vector<double> delay_cost, delay_bestcost;
			delay_cost.resize(APPNUM);
			delay_bestcost.resize(APPNUM);
			for(int j=0;j<Maxreq;j++)
			{
				delay_cost[reqL[j]->app_id]+=gn_delay.cost_LP[j];
				delay_bestcost[reqL[j]->app_id]+=gn_delay.cost_best[j];
			}*/

			//���������
			double happiness_delayLP=0;//���������ͳ��
			if(result_network_delay==Inf)happiness_delayLP=0;
			else
			{
				happiness_delayLP=0;
				//for(int i=0;i<reqN;i++)
				//	cout<<g.cost_best[i]<<" "<<g.cost_LP[i]<<endl;
				for(int j=0;j<APPNUM;j++)
				{
					if(gv.cost_best[j]==0)continue;
					happiness_delayLP += gv.cost_best[j]/gn_delay.cost_LP[j];
				}
			}
			happiness_sum_delayLP += happiness_delayLP;

			double s2_delay=0;
			double happiness_avg_delay=happiness_delayLP/APPNUM;
			for(int j=0;j<APPNUM;j++)
			{
				if(gv.cost_best[j]==0) continue;
				s2_delay+=(gv.cost_best[j]/gn_delay.cost_LP[j]-happiness_avg_delay) * (gv.cost_best[j]/gn_delay.cost_LP[j]-happiness_avg_delay);
			}
			s2_delay=s2_delay/APPNUM;

			cout<<"��������ȣ� "<<happiness_delayLP/APPNUM<<endl;
			cout<<"��������ȣ� "<<happiness_sum_delayLP/(APPNUM*(i+1))<<endl;
			if(i==caseN-1) outfile<<happiness_sum_delayLP/(APPNUM*(i+1))<<",";
			cout << "��" << i << "�����������ȷ�� " << s2_delay << endl;

			double latency_delayLP=0;
			latency_delayLP=delay_DelayNetworkGraph(gn_delay);
			cout << "����������ʱ��: " << latency_delayLP << endl;

			double maxUtil_DelayNetworkGraph=0;
			for(int j=0;j<gn_delay.m;j++)
			{
				int src=gn_delay.incL[j]->src;
				int dst=gn_delay.incL[j]->dst;
				double capacity=gn_delay.incL[j]->capacity;
				if(maxUtil_DelayNetworkGraph<(gn_delay.adj[src][dst]/capacity))
					maxUtil_DelayNetworkGraph=gn_delay.adj[src][dst]/capacity;
			}
			cout<<"�����·������: "<<maxUtil_DelayNetworkGraph<<endl;
			double s2_cmp=0;
			if(s2_voting==0 && s2_delay==0) s2_cmp=1;
			else if(s2_voting==0 && s2_delay!=0) s2_cmp=10;//�Լ�ƾֱ�������
			else s2_cmp=s2_delay/s2_voting;
			cout<<endl<<"Delay�����Ż�����/ͶƱ����="<<s2_cmp<<endl;

		   oneCaseHappiness_DelayNetwork.push_back(happiness_delayLP/APPNUM);
		   multiCaseHappines_DelayNetwork.push_back(happiness_sum_delayLP/(APPNUM*(i+1)));
		   oneCaseS2_DelayNetwork.push_back(s2_delay);
		   delay_DelayNetwork.push_back(latency_delayLP);
		   TE_DelayNetwork.push_back(maxUtil_DelayNetworkGraph);

		   s2_DelayNetwork_Voting.push_back(s2_cmp);

			//@@@@@@@@@@@@@@@@@End of network_delay@@@@@@@@@@@@@@@@@@@@@@@@@
			//@@@@@@@@@@@@@@@@@End of network_delay@@@@@@@@@@@@@@@@@@@@@@@@@

		}//һ��case����

		//outfile<<endl;

		//����ȷ��� APP-Voting DelayNetwork DelayNetwork/APP_Voting
		double sum1=0;
		for(int i=0;i<caseN;i++)
			sum1+=oneCaseS2_Voting[i];
		outfile<<sum1/caseN<<",";
		
		double sum2=0;
		for(int i=0;i<caseN;i++)
			sum2+=oneCaseS2_DelayNetwork[i];
		outfile<<sum2/caseN<<",";
		
		outfile<<sum2/sum1<<",";

		//��������ʱ APP-Voting TENetwork DelayNetwork
		double sum3=0;
		for(int i=0;i<caseN;i++)
			sum3+=delay_Voting[i];
		outfile<<sum3/caseN<<",";
		
		double sum4=0;
		for(int i=0;i<caseN;i++)
			sum4+=delay_TENetwork[i];
		outfile<<sum4/caseN<<",";

		double sum5=0;
		for(int i=0;i<caseN;i++)
			sum5+=delay_DelayNetwork[i];
		outfile<<sum5/caseN<<",";

		//�����·������ APP-Voting TENetwork DelayNetwork
		double sum6=0;
		for(int i=0;i<caseN;i++)
			sum6+=TE_Voting[i];
		outfile<<sum6/caseN<<",";
		
		double sum7=0;
		for(int i=0;i<caseN;i++)
			sum7+=TE_TENetwork[i];
		outfile<<sum7/caseN<<",";

		double sum8=0;
		for(int i=0;i<caseN;i++)
			sum8+=TE_DelayNetwork[i];
		outfile<<sum8/caseN<<",";

		outfile<<endl;
	}
	
	getchar();
	return 0;
}