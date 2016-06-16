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
const int N=3;//���еĵ���
const int M=6;//�����������
const int Maxreq=3;//һ��case������������
const int Voternum=4;//flow+network
const int Voternum_MAX=1000;//flow+network
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=2;//case����
const int Maxflow=5;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=10;//���Ĵ�С��ʼ��Χ
*/

/*
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
*/

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


//graph_ATT
const int Inf=99999;
const int N=25;//���еĵ���
const int M=112;//�����������
const int Maxreq=21;//һ��case������������
const int Voternum_MAX=100;//flow+network
const int Maxpath=N-1;//���ܵ��·��: N-1

const int caseN=8;//case����
const int Maxflow=15;//���Ĵ�С�ɱ䷶Χ
const int Begin_num=5;//���Ĵ�С��ʼ��Χ


//�����ͼ����Ҫ�޸ģ� ����Ĳ��� + ͼ���� + req����Ĳ���

int main()
{
	srand((unsigned)time(NULL));
	string graph_address="graph_ATT_big.txt";
	ifstream req_file("req_20_04.txt");
	VGraph gv(graph_address);//Voting�õ�ͼ
	PGraph gp(graph_address);//LP�õ�ͼ
	vector<Voter*> flowL;//��¼���е���ʵ��
	vector<Voter*> voterL;//��¼���е�ͶƱ��
	vector<Voter*> candiL;//��¼���еĺ�ѡ��

	
	ofstream outfile("result.txt");//���һ��case�Ľ��
	ofstream req_outfile("req_outfile.txt");
	outfile<<"graph_ATT�������� caseN: "<<caseN<<endl;
	outfile<<"flow Range: "<<Begin_num<<"--"<<Maxflow+Begin_num-1<<endl<<endl;
	
	vector<Req*> reqL;
	double table[M2C+1][N2C+1] = {0};
	int ranking[N2C+1]={0};//��¼һ�������ͶƱ����
	double happiness_sum=0;
	
	for(int j=1;j<=Voternum_MAX;j++)
		ranking[j]=1;//ÿ��ͶƱ�����1��voter,���Ϊ2��˵���÷����еõ�����voter��Ʊ

	

	//******** Flow Voting Variables *******
	VGraph gv_flow(graph_address);//Flow Voting�õ�ͼ
	cout<<"flow voting"<<endl;
	vector<Voter*> flowL_flow;//��¼Flow Voting���е���ʵ��
	vector<Voter*> voterL_flow;//��¼Flow Voting���е�ͶƱ��
	vector<Voter*> candiL_flow;//��¼Flow Voting���еĺ�ѡ��
	double table_flow[M2C+1][N2C+1] = {0};
	int ranking_flow[N2C+1]={0};//��¼һ�������ͶƱ����
	double happiness_sum_flow=0;
	

	
	for(int j=0;j<Voternum_MAX;j++)
		for(int k=0;k<Voternum_MAX+3;k++)
		{
			table[j][k]=0;
			table_flow[j][k]=0;//table_flow����Ч���ݷ�Χ��table���Ӽ�
		}
	req_outfile<<endl;

	
	//ɾ����һ��case��������
	reqL.clear();
	gv.reqL.clear();
	gv_flow.reqL.clear();

	
	int flow_rate=0;
	int cnt=0;

	for(int i=0;i<gv_flow.n;i++)
		for(int j=0;j<gv_flow.n;j++)
		{
			req_file>>flow_rate;
			if(flow_rate==0) continue;
			
			/* t3
			if(i==0 && j==1) flow_rate=10;
			else if(i==2 && j==1) flow_rate=10;
			else continue;
			*/

			Req* r = new Req(cnt,i,j,flow_rate);
			Voter* flow_now = new Flow(cnt,0,i,j,flow_rate);
			Voter* flow_now_flow =  new Flow(cnt,0,i,j,flow_rate);//Flow Voting use

			reqL.push_back(r);
			gv.reqL.push_back(r);
			gv_flow.reqL.push_back(r);
		
			flowL.push_back(flow_now);
			voterL.push_back(flow_now);
			candiL.push_back(flow_now);

			flowL_flow.push_back(flow_now_flow);
			voterL_flow.push_back(flow_now_flow);
			candiL_flow.push_back(flow_now_flow);

			cnt++;
		}
    ranking[flowL.size()]=5;//������ͶƱ�߸����Ʊ��

	Voter* net_lb = new Network_LB(flowL.size(),1);// Maxreq��ͶƱ��id, 1 ��ʾ����ͶƱ��
	

	//Maxreq+1��ͶƱ��id��2��ʾ����ͶƱ��
	vector<Voter*> neutral;
	for(int i=0;i<10;i++)
	{
		neutral.push_back(new Neutral(flowL.size()+i+1, 2) );
		neutral[i]->te = (i+1)/10.0;
	}

	voterL.push_back(net_lb);
	candiL.push_back(net_lb);

	for(int i=0;i<10;i++)
		candiL.push_back(neutral[i]);
		
	//ÿ��ͶƱ�����1��voter,���Ϊ2��˵���÷����еõ�����voter��Ʊ
	for(int j=1;j<=flowL.size();j++)
		ranking_flow[j]=1;

	cout<<"init completed"<<endl;
		
	

	//**************************************  Flow Voting  **************************************
	//**************************************  Flow Voting  **************************************
	//only flows' proposal can be voted for a winner
		
	//�����������������е�·����һ���ߵõ��ķ���
	for(int j=0;j<flowL.size();j++)
		gv_flow.cost_best[j] = gv_flow.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow,flowL_flow[0]->adj);

	//�᷽��
	for(int j=0;j<candiL_flow.size();j++)
			candiL_flow[j]->propose(gv_flow);
	//cout<<"�����������"<<endl;

	
	//���۷���
	for(int j=0;j<voterL_flow.size();j++)
		voterL_flow[j]->evaluate(gv_flow,candiL_flow);
	//cout<<"���۷�������"<<endl;

	//ͶƱ�㷨, ͶƱ�㷨�������Ǵ�1��ʼ�ģ�����judge�Ǵ�0��ʼ�ģ�������Ҫ����һ��
	for(int j=0;j<voterL_flow.size();j++)
		for(int k=0;k<candiL_flow.size();k++)
		{
			//�ò�����evaluateʱ��ͳͷ�----����if(voterL[j]->judge[k]==0) table[j+1][k+1]=10000;//�����0��˵����û�а���������
			table_flow[j+1][k+1]=voterL_flow[j]->judge[k];
		}
	cout<<endl<<"voting uses ";
	int choice_flow=1;//ѡ��һ��ͶƱ�㷨
	int winner_flow=0;
	Voting vv_flow(table_flow,ranking_flow,voterL_flow.size(),candiL_flow.size());
	winner_flow=vv_flow.voting(choice_flow);
	winner_flow=winner_flow-1;//��������ǰ��մ�0��ʼ���

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


	//���������
	double happiness_flow=0;//һ��������������Ⱥͣ�Խ��Խ��,0<=�����<=1
	cout<<"calculate happiness"<<endl;
	for(int j=1;j<=flowL.size();j++)
	{
		//cout<<gv_flow.cost_best[j-1]<<" "<<table_flow[j][winner_flow+1]<<endl;
		happiness_flow += gv_flow.cost_best[j-1]/table_flow[j][winner_flow+1];//��þ�������/��ǰ��������
	}	
	happiness_sum_flow += happiness_flow;
	

	//���㷽�������ǰ�ܵ�cost�������û�б����Ž����磬�����ӳͷ�cost
		
	//ͳ������latency
	double latencyVoting_flow=0;
	latencyVoting_flow=judge_sum_function(gv_flow,candiL_flow,winner_flow);

	cout << "winner = "<<winner_flow<<endl;
	cout << "��������ȣ� " << happiness_flow/flowL.size() << endl;
	//cout << "��������ȣ�" << happiness_sum_flow/ Maxreq << endl;
	cout << "����������ʱ��: " << latencyVoting_flow << endl;

	double maxUtil_Voting_flow=0;
	for(int j=0;j<gp.m;j++)
	{
		int src=gv_flow.incL[j]->src;
		int dst=gv_flow.incL[j]->dst;
		double capacity=gv_flow.incL[j]->capacity;
		if(maxUtil_Voting_flow<(flowL_flow[winner_flow]->adj[src][dst]/capacity))
			maxUtil_Voting_flow=flowL_flow[winner_flow]->adj[src][dst]/capacity;
	}
	cout<<"�����·������: "<<maxUtil_Voting_flow<<endl;

	//�ļ���¼
	outfile <<" Flow Voting Result"<<endl;
	outfile << "winner = "<<winner_flow<<endl;
	outfile  << "��������ȣ� " << happiness_flow/flowL.size() << endl;
	//outfile << "��������ȣ�" << happiness_sum_flow/ Maxreq << endl;
	outfile << "����������ʱ��: " << latencyVoting_flow << endl;
	outfile <<"�����·������: "<<maxUtil_Voting_flow<<endl;

	
	//ʤ���ķ�������
	//for(int j=0;j<candiL_flow.size();j++)
		//candiL_flow[j]->end_implement(gv_flow,winner_flow,candiL_flow);

	cout<<"End of Flow Voting"<<endl;

	//**************************************   End of Flow Voting    **************************************
	//**************************************   End of Flow Voting    **************************************

	
	//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@
	//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@

	//�����������������е�·����һ���ߵõ��ķ���
	for(int j=0;j<flowL.size();j++)
		gv.cost_best[j] = gv.dijkstra(reqL[j]->src,reqL[j]->dst,reqL[j]->flow,flowL[0]->adj);

	//for(int j=0;j<Maxreq;j++)
		//cout<<"gv.cost_best "<<j<<" : "<<gv.cost_best[j]<<endl;

	//�᷽��
	for(int j=0;j<candiL.size();j++)
		candiL[j]->propose(gv);
	//cout<<"�����������"<<endl;

	//���۷���
	for(int j=0;j<voterL.size();j++)
		voterL[j]->evaluate(gv,candiL);
	//cout<<"���۷�������"<<endl;

	//ͶƱ�㷨, ͶƱ�㷨�������Ǵ�1��ʼ�ģ�����judge�Ǵ�0��ʼ�ģ�������Ҫ����һ��
	for(int j=0;j<voterL.size();j++)
		for(int k=0;k<candiL.size();k++)
		{
			//�ò�����evaluateʱ��ͳͷ�----����if(voterL[j]->judge[k]==0) table[j+1][k+1]=10000;//�����0��˵����û�а���������
			table[j+1][k+1]=voterL[j]->judge[k];
		}
	cout<<endl<<"voting uses ";
	int choice=1;//ѡ��һ��ͶƱ�㷨
	int winner=0;
	Voting vv(table,ranking,voterL.size(),candiL.size());
	winner=vv.voting(choice);
	winner=winner-1;//��������ǰ��մ�0��ʼ���

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
	for(int i=0;i<=voterL.size();i++)
	{
		outfile<<"flow ";
		outfile.setf(ios::right);
		outfile.width(3);
		outfile<<i-1<<" judge: ";
		if(i==0)
		{
			for(int j=1;j<=candiL.size();j++){
				outfile.setf(ios::left);
				outfile.width(10);
				outfile<<j-1<<" ";
			}
			outfile<<endl;
			for(int j=0;j<(candiL.size()+3)*10;j++)
				outfile<<"-";
			outfile<<endl;
			continue;
		}
		for(int j=1;j<=candiL.size();j++){
			outfile.setf(ios::left);
			outfile.width(10);
			outfile<<table[i][j]<<" ";
		}
		outfile<<endl;
	}
	outfile<<endl;


	//���������
	double happiness=0;//һ��������������Ⱥͣ�Խ��Խ��,0<=�����<=1
	for(int j=1;j<=flowL.size();j++)
		happiness += gv.cost_best[j-1]/table[j][winner+1];//��þ�������/��ǰ��������
	happiness_sum += happiness;

	//���㷽�������ǰ�ܵ�cost�������û�б����Ž����磬�����ӳͷ�cost
		
	//ͳ������latency
	double latencyVoting=0;
	latencyVoting=judge_sum_function(gv,candiL,winner);

	cout << "winner = "<<winner<<endl;
	cout <<  "����������ȣ� " << happiness/flowL.size() << endl;
	//cout << "��������ȣ�" << happiness_sum / Maxreq << endl;
	cout << "����������ʱ��: " << latencyVoting << endl;

	double maxUtil_Voting=0;
	maxUtil_Voting=voterL[flowL.size()]->judge[winner];
	cout<<"�����·������: "<<maxUtil_Voting<<endl;

	//�ļ���¼
	outfile<<" Voting Result"<<endl;
	outfile << "winner = "<<winner<<endl;
	outfile << "��������ȣ� " << happiness/flowL.size() << endl;
	//outfile << "��������ȣ�" << happiness_sum / Maxreq << endl;
	outfile << "����������ʱ��: " << latencyVoting << endl;
	outfile <<"�����·������: "<<maxUtil_Voting<<endl;

	//ʤ���ķ�������
	//for(int j=0;j<candiL.size();j++)
		//candiL[j]->end_implement(gv,winner,candiL);
		
	//�޸�neutral��teֵ
	//neutral_low->te = maxUtil_Voting + 0.05;
	//neutral_middle->te = maxUtil_Voting + 0.1;
	//neutral_high->te = maxUtil_Voting + 0.2;

	cout<<endl;

	//@@@@@@@@@@@@@@@@@End of Voting@@@@@@@@@@@@@@@@@@@@@@@@@
	//@@@@@@@@@@@@@@@@@End of Voting@@@@@@@@@@@@@@@@@@@@@@@@@

	//�ֶι滮����
	cout<<endl<<"			LP result			"<<endl;
		
	//judge_LP��¼����������ܺͣ�judge_sum_LP��¼����������ܺ�
	double judge_LP=0;
	static double judge_sum_LP=0;

	//���Ų������
	for(int j=0;j<flowL.size();j++)
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
		for(int j=0;j<flowL.size();j++)
			judge_LP += gp.cost_best[j]/gp.cost_LP[j];
	}
	judge_sum_LP += judge_LP;
	cout<<"��������ȣ� "<<judge_LP/flowL.size()<<endl;
	//cout<<"��������ȣ� "<<judge_sum_LP/(Maxreq*(i+1))<<endl;

	double latency_LP=0;
	latency_LP=judge_sum_LP_function(gp);
	cout << "����������ʱ��: " << latency_LP << endl;
	cout<<"�����·������: "<<result_LP<<endl;
		
	//�ļ���¼
	outfile<<" LP Result"<<endl;
	outfile<<"��������ȣ� "<<judge_LP/flowL.size()<<endl;
	outfile<<"��������ȣ� "<<judge_sum_LP/flowL.size()<<endl;
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
		
	
		

	
	getchar();
	return 0;
}