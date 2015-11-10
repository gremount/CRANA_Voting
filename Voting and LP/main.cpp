#include"common.h"
#include"resources.h"
#include"LP.h"
#include"LP2.h"
#include"voting.h"
#include"single_flow.h"

int K;
CEdge::CEdge(int a, int b, int c, int d){
	tail=a;
	head=b;
	weight=c;
	capacity=d;
}

CEdge::CEdge(int a, int b, int c){
	head=b;
	tail=a;
	weight=c;
}

CEdge::CEdge(CEdge & x){
	tail=x.getTail();
	head=x.getHead();
	weight=x.getWeight();
	capacity=x.getCap();
}

CGraph::CGraph(list<CEdge*> listEdge,int node_num,int edge_num){
	IncidentList=listEdge;
	numVertex=node_num;
	numEdge=edge_num*2;
	for(int i=0;i<=K;i++)
		for(int j=0;j<=N;j++)
			for(int k=0;k<=N;k++)
				link_bw[i][j][k]=0;
}

int main()
{
	srand((unsigned)time(NULL));
	Graph g("d:\\github\\CRANA_Voting\\graph2.txt");
	int caseN=6;
	double judge=0,judge_sum=0;
	int req_sum=0,result_sum=0;
	vector<Req*> reqL_all;

	list<CEdge*> listEdge;
	int winner = 0;//No.1
	float happiness_sum = 0;//��¼���ַ����ۼ������������
	int K_sum = 0;//��¼һ���ж���������Ҫ����·��
	float table[M2C+1][N2C+1] = {0};
	int ranking[N2C+1]={0};//��¼һ�������ͶƱ����
	ofstream reqRecord("d:\\github\\reqRecord.txt");

	//ͼ�ĳ�ʼ��
	ifstream test("d:\\github\\CRANA_Voting\\graph2.txt");
	int node_num,edge_num;
	int src,dst,weight,cap;
	test>>node_num>>edge_num;
	for(int i=1;i<=edge_num;i++)
	{
		test>>src>>dst>>weight>>cap;
		CEdge* e1=new CEdge(src,dst,weight,cap);
		CEdge* e2=new CEdge(dst,src,weight,cap);
		listEdge.push_back(e1);
		listEdge.push_back(e2);
	}
	CGraph gv(listEdge,node_num,edge_num);
	gv.p3();
	gv.p4();
	int req_num,req_constant;
	req_num=6;
	req_constant = req_num+1;

	for(int icase=1;icase<=caseN;icase++)
	{
		cout<<"--------------------------------------------------"<<endl;
		cout<<"case "<<icase<<endl;

		//�����¼
		CReq* r1=new CReq(0,0,0);
		gv.r.push_back(r1);
	
		
		K=MAXREQ;

		if (req_num != (req_constant-1)) //��һ��req����ʱ�򣬲���Ҫ�޸�link_bw
		{
			for (int k = 1; k <= K; k++)
			{
				list<CEdge*>::iterator it, iend;
				iend = listEdge.end();
				for (it = listEdge.begin(); it != iend; it++)
				{
					gv.link_bw[k][(*it)->getTail()][(*it)->getHead()] = gv.link_bw[winner][(*it)->getTail()][(*it)->getHead()];
					//cout << (*it)->getTail() << " " << (*it)->getHead() << " " << g.link_bw[k][(*it)->getTail()][(*it)->getHead()]<<endl;
				}
			}
		}
		for (int i = 1; i <= K; i++)
			for (int j = 1; j <= K; j++)
			{
				CPath* pa = new CPath();
				gv.path_record[i][j] = pa;
			}

		//case����
		reqRecord<<"case "<<icase<<endl;
		int reqN,a,b,c;
		vector<Req*> reqL;
		reqN=MAXREQ;
		reqL.clear();
		a=1;b=17;
		for(int i=0;i<reqN;i++)
		{
			c=0;
			while(c==0){c = 3 + rand()%MAXFLOW;}	
			Req* r=new Req(a,b,c);
			reqL.push_back(r);
			CReq* r2=new CReq(a,b,c);
			gv.bw[i+1]=c;
			gv.r.push_back(r2);
			reqL_all.push_back(r);
			reqRecord<<c<<" ";
		}
		reqRecord<<endl<<endl;
		//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@  Voting  @@@@@@@@@@@@@@@@@@@@@@

		//�᷽��
		for(int i=1;i<=K;i++)
		{
			gv.single_flow_propose(i,K);
		}

		//����
		for(int i=1;i<=K;i++)
			for(int j=1;j<=K;j++)
			gv.judge[i][j]=0;
		for(int i=1;i<=K;i++)
			gv.judge_sum[i]=0;
		for(int i=1;i<=K;i++)
		{
			//cout<<"evaluate "<<i<<endl;
			gv.single_flow_evaluate(i,K);
			gv.cost_evaluate(i);
		}
		/*
		cout<<endl;
		cout<<"************ single judge **************"<<endl;
		for(int i=1;i<=K;i++)
		{
			cout<<"flow ";
			cout.setf(ios::right);
			cout.width(3);
			cout<<i<<" judge: ";
			for(int j=1;j<=K;j++){
				cout.setf(ios::left);
				cout.width(4);
				cout<<gv.judge[i][j]<<" ";
			}
			cout<<endl;
		}
		cout<<endl;
		cout<<"************ sum judge **************"<<endl;
	
		for(int i=1;i<=K;i++)
		{	
			cout<<"proposal "<<i<<" : "<<gv.judge_sum[i]<<endl;
		}
		*/

		//voting method
		for(int i=1;i<=K;i++)
			for(int j=1;j<=K;j++)
			{
				if(gv.judge[i][j]==0) {table[i][j]=10000;}//�����0��˵����û�а���������
				else table[i][j]=gv.judge[i][j];
			}
		for(int i=1;i<=K;i++)
			ranking[i]=1;

		/*
		cout<<"************ voting table **************"<<endl;
		for(int i=1;i<=K;i++)
		{
			cout<<"flow ";
			cout.setf(ios::right);
			cout.width(3);
			cout<<i<<" judge: ";
			for(int j=1;j<=K;j++){
				cout.setf(ios::left);
				cout.width(5);
				cout<<table[i][j]<<" ";
			}
			cout<<endl;
		}
		cout<<endl;
		*/
		cout<<"			voting result			";
		int choice = 1;
		Voting vv(table,ranking,K,K);
		winner=vv.voting(choice);
		cout<<endl;
		if (choice == 1)
			cout << "schulze method : " << winner << endl;
		else if (choice == 2)
			cout << "comulative method: " << winner << endl;
		else if (choice == 3)
			cout << "copeland condorcet method: " << winner << endl;
		else
			cout << "ranked pairs method: " << winner << endl;

		//���������
		float happiness=0;//һ��������������Ⱥͣ�Խ��Խ��,0<=�����<=1
		for (int i = 1; i <= K; i++)
			happiness += table[i][i] / table[i][winner];//��þ�������/��ǰ��������
		happiness_sum += happiness;
		K_sum += K;

		//�����û�б����Ž����磬�����ӳͷ�cost
		for(int i=1;i<=K;i++)
			if(table[i][winner]=10000) gv.judge_sum[winner]+=MAXPATH*gv.bw[i];

		cout << "��" << req_constant - req_num << "����������ȣ� " << happiness/K << endl;
		cout << "������������Ⱥͣ�" << happiness_sum / K_sum << endl;
		cout << "����������ۺ�: " << gv.judge_sum[winner] << endl;


		//@@@@@@@@@@@@@@@@@End of Voting@@@@@@@@@@@@@@@@@@@@@@@@@
		//@@@@@@@@@@@@@@@@@End of Voting@@@@@@@@@@@@@@@@@@@@@@@@@

		//���Ų������
		g.cost_best.clear();
		g.cost_LP.clear();
		//cout<<"35 line"<<endl;
		for(int i=0;i<reqN;i++)
		{
			//cout<<reqL[i]->src<<" "<<reqL[i]->dst<<" "<<reqL[i]->flow<<endl;
			g.cost_best.push_back(reqL[i]->flow * g.dijkstra(reqL[i]->src,reqL[i]->dst,reqL[i]->flow));
		}

		cout<<endl<<"			LP result			"<<endl;

		//���Թ滮����
		double result=0;
		result=LP(&g,reqL);


		
		//�����Թ滮�����cost
		result_sum += result;
		cout<<"cost of this case is: "<<result<<endl;
		cout<<"cost of all cases is: "<<result_sum<<endl;
		
		//���������
		if(result==999999)
			judge=0;
		else
		{
			judge=0;
			//for(int i=0;i<reqN;i++)
			//	cout<<g.cost_best[i]<<" "<<g.cost_LP[i]<<endl;
			for(int i=0;i<reqN;i++)
				judge += g.cost_best[i]/g.cost_LP[i];
		}
		judge_sum += judge;
		req_sum += reqN;
		cout<<"��������ȣ� "<<judge/reqN<<endl;
		cout<<"��������ȣ� "<<judge_sum/req_sum<<endl;
	}

	//ȫ�����Թ滮����
	cout<<endl<<"			LP_ALL result			"<<endl;
	Graph g3("d:\\github\\CRANA_Voting\\graph2.txt");
	double result3=0;
	result3=LP(&g3,reqL_all);
	cout<<"cost of all cases is: "<<result3<<endl;

	cout<<"End of the Program"<<endl;
	test.close();
	reqRecord.close();
	getchar();
	return 0;
}