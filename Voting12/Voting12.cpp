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
const int N=3;//���еĵ���
const int M=6;//�����������
const int Maxreq=3;//һ��case������������
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
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=6;//case����
const int Maxflow=15;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=5;//���Ĵ�С��ʼ��Χ
*/
//�����ͼ����Ҫ�޸ģ� ����Ĳ��� + ͼ���� + req����Ĳ���

int main()
{
	srand((unsigned)time(NULL));
	string address="graph_all.txt";

	VGraph gv(address);//Voting�õ�ͼ
	TENetworkGraph gn_te(address);//TEȫ���Ż���ͼ
	DelayNetworkGraph gn_delay(address);//��ʱȫ���Ż���ͼ

	vector<Flow*> flowL;//��¼���е���ʵ��
	ofstream outfile("result.txt");//���һ��case�Ľ��
	ofstream req_outfile("req_outfile.txt");

	outfile<<"graph_ATT�������� caseN: "<<caseN<<endl;
	outfile<<"flow Range: "<<Begin_num<<"--"<<Maxflow+Begin_num-1<<endl<<endl;

	double happiness_sum_teLP=0;//te����Ķ��������
	double happiness_sum_delayLP=0;//delay����Ķ��������
	double happiness_sum=0;//voting�Ķ��������

	vector<Req*> reqL;//������
	double table[M2C+1][N2C+1] = {0};//ͶƱ�õ�����
	int ranking[N2C+1]={0};//��¼һ�������ͶƱ����
	
	
	for(int j=1;j<=Maxreq;j++)
		ranking[j]=1;//ÿ��ͶƱ�����1��voter,���Ϊ2��˵���÷����еõ�����voter��Ʊ

	
	for(int i=0;i<caseN;i++)
	{
		cout<<endl<<"*************************"<<" case "<<i<<"*************************"<<endl;
		//��ʼ��
		for(int j=0;j<Maxreq;j++)
			for(int k=0;k<Maxreq;k++)
				table[j][k]=0;
		
		req_outfile<<"case "<<i<<endl;

		
		//���³���ʱ��ֹÿ��case���ᴴ����Req���µ��ڴ�й¶
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

		//�����������������е�·����һ���ߵõ��ķ���
		for(int j=0;j<Maxreq;j++)
			gv.cost_best[j] = gv.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow,flowL[0]->adj);

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
				if(flowL[j]->judge[k]==0) table[j][k]=10000;//�����0��˵����û�а���������
				else table[j][k]=flowL[j]->judge[k];
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

		//���������
		double happiness=0;//һ��������������Ⱥͣ�Խ��Խ��,0<=�����<=1
		for(int j=0;j<Maxreq;j++)
			happiness += gv.cost_best[j]/table[j][winner];//��þ�������/��ǰ��������
		happiness_sum += happiness;

		double s2_voting=0;
		double happiness_avg=happiness_sum/Maxreq;
		for(int j=0;j<Maxreq;j++)
			s2_voting+=(gv.cost_best[j]/table[j][winner]-happiness_avg) * (gv.cost_best[j]/table[j][winner]-happiness_avg);
		s2_voting=s2_voting/Maxreq;

		//���㷽�������ǰ�ܵ�cost�������û�б����Ž����磬�����ӳͷ�cost
		
		//ͳ������latency
		double latencyVoting=0;
		latencyVoting=judge_sum_function(gv,flowL,winner);

		for(int j=0;j<Maxreq;j++)
			if(table[j][winner]==10000) {
				cout<<"�����ͷ�����"<<endl;
				latencyVoting += Maxpath * reqL[j]->flow;
			}
		cout << "��" << i << "����������ȣ� " << happiness/Maxreq << endl;
		cout << "��" << i << "�����������ȷ�� " << s2_voting << endl;
		cout << "��������ȣ�" << happiness_sum / ((i+1)*Maxreq) << endl;
		cout << "����������ʱ��: " << latencyVoting << endl;
		
		double maxUtil_Voting=0;
		for(int j=0;j<gv.m;j++)
		{
			int src=gv.incL[j]->src;
			int dst=gv.incL[j]->dst;
			double capacity=gv.incL[j]->capacity;
			if(maxUtil_Voting<(flowL[winner]->adj[src][dst]/capacity))
				maxUtil_Voting=flowL[winner]->adj[src][dst]/capacity;
		}
		cout<<"�����·������: "<<maxUtil_Voting<<endl;

		//ʤ���ķ�������
		
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

		double latency_teLP=0;
		latency_teLP=delay_TENetworkGraph(gn_te,flowL);
		cout << "����������ʱ��: " << latency_teLP << endl;
		cout<<"�����·������: "<<result_network_te<<endl;
		
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
		
		//���������
		double happiness_delayLP=0;//���������ͳ��
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

		cout<<"��������ȣ� "<<happiness_delayLP/Maxreq<<endl;
		cout<<"��������ȣ� "<<happiness_sum_delayLP/(Maxreq*(i+1))<<endl;
		cout << "��" << i << "�����������ȷ�� " << s2_delay << endl;

		double latency_delayLP=0;
		latency_delayLP=delay_DelayNetworkGraph(gn_delay,flowL);
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

		//@@@@@@@@@@@@@@@@@End of network_delay@@@@@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@@End of network_delay@@@@@@@@@@@@@@@@@@@@@@@@@

	}//һ��case����
	getchar();
	return 0;
}