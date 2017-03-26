#ifndef APP_VOTING_H
#define APP_VOTING_H

#include"app.h"
#include"voting.h"

//������� 1/(c-x)
double linearCal(double load, double capacity)
{
	double util=load/capacity;
	if(util<0.3333) return load/capacity;
	else if(util<0.6667) return 3*load/capacity - 2.0/3.0;
	else if(util<0.9) return 10*load/capacity - 16.0/3.0;
	else return 70*load/capacity - 178.0/3.0;
}

//Votingͳ������latency
double judge_sum_function(VGraph &g, vector<APP*> &appL, int winner)
{
	double latency=0;
	for(int i=0;i<M;i++)
	{
		int src,dst;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		double x=appL[winner]->adj[src][dst];
		double capacity= g.incL[i]->capacity;
		latency += x*linearCal(x,capacity);
	}
	return latency;
}

int selectMinS2Proposal(VGraph gv, double table[][M2C+1], int winner)
{
	//****************************   �������ȷ�������С����ȷ���� ����
	//���� �������ȷ��� �� ��С����ȷ����
	double happinessVotingMax=0;//��������
	double happinessVotingTemp=0;
	int happinessVotingLoc=0;
	double s2Min=Inf;//��С����ȷ���
	double s2Temp=0;
	int s2Loc=0;//��С����ķ���
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
	/*
	cout <<"�������ȷ����Ľ��"<<endl;
	cout << "��������ȣ� " << happinessVotingMax << endl;
	cout << "���������ȷ�� " << s2VotingMax << endl;
	cout <<endl;

	cout <<"��С����� ���� �����Ľ��"<<endl;
	cout << "��������ȣ� " << s2Min_Happiness << endl;
	cout << "���������ȷ�� " << s2Min << endl;
	cout <<endl;
	*/
	//���ѡ�񷽲���С�ķ�������������˳�����30%���Ͳ���schulze method��ͶƱ���
	if(happinessVotingMax-s2Min_Happiness<=0.3) return s2Loc;
	else return winner;
	//****************************   END OF �������ȷ�������С����ȷ���� ����  
}

void app_voting(string graph_address, string req_address, string path_address)
{
	

	//��ʼ��������
	vector<Req*> reqL;//������
	reqL.clear();

	//������������: (id app_id src dst flow)
	ifstream infile(req_address);
	string str;
	int cnt=0;
	getline(infile,str);
	vector<double> e8;
	e8.resize(8);
	while(infile!=NULL){
		for(int i=0;i<8;i++)
			infile>>e8[i];
		cnt++;
		int app_id=e8[0];
		int src=e8[3];
		int dst=e8[5];
		double flow=e8[7];
		Req* r = new Req(app_id,app_id,src,dst,flow);
		reqL.push_back(r);
	}
	cnt--;

	int reqNum=cnt;
	APPNUM=reqNum;
	REQNUM=reqNum;

	cout<<reqNum<<endl;

	//����ͼ
	VGraph gv(graph_address);
	gv.reqL.clear();
	for(int i=0;i<REQNUM;i++)
		gv.reqL.push_back(reqL[i]);

	//************************  ��ʼ��  ***********************
	//ͶƱϵͳ��ʼ��
	double table[M2C+1][N2C+1] = {0};//ͶƱ�õ�����
	int ranking[N2C+1]={0};//��¼һ�������ͶƱ����
	for(int j=0;j<reqNum;j++)
		ranking[j]=1;//ÿ��ͶƱ�����1��voter,���Ϊ2��˵���÷����еõ�����voter��Ʊ
	for(int j=0;j<reqNum;j++)
		for(int k=0;k<reqNum;k++)
			table[j][k]=0;

	//����APP������ʼ��
	vector<APP*> appL;//��¼���е�APP
	for(int k=0;k<APPNUM;k++)
		appL.push_back(new APP(k));
	for(int j=0;j<APPNUM;j++)
		appL[j]->init();

	
	//************************  ͶƱ���ƿ�ʼ  **********************
	//�����������������е��Լ���������һ���ߵõ��ķ���
	for(int j=0;j<APPNUM;j++)
	{
		vector<Req*> reqPL_app;
		for(int i=0;i<reqL.size();i++)
		{	
			if(j==reqL[i]->app_id) reqPL_app.push_back(reqL[i]);
		}
		gv.network_delay_lp(reqPL_app,j);
	}

	//for(int j=0;j<REQNUM;j++)
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
			if(appL[j]->judge[k]==0) table[j][k]=10000000;//�����0��˵����û�а���������
			else table[j][k]=appL[j]->judge[k];
		}
	
	int choice=1;//ѡ��һ��ͶƱ�㷨
	int winner=0;
	Voting vv(table,ranking,APPNUM,APPNUM);
	winner=vv.voting(choice);
	cout<<"schulze winner = "<<winner<<endl;

	//���ѡ�񷽲���С�ķ�������������˳�����30%���Ͳ���schulze method��ͶƱ���
	//winner=selectMinS2Proposal(gv, table, winner);
	cout<<"crana winner = "<<winner<<endl;
	
	ofstream pathFile(path_address);
	pathFile<<"id	path"<<endl;
	for(int i=0;i<REQNUM;i++)
	{
		pathFile<<i<<"	";
		vector<int> realPath;
		int src=reqL[i]->src;
		Edge* empty_edge=new Edge(-1,-1,-1,-1,-1,-1,-1);
		pathFile<<reqL[i]->src<<"_1,";
		while(src!=reqL[i]->dst)
		{
			for(int j=0;j<appL[winner]->path_record[i]->pathL.size();j++)
				if(appL[winner]->path_record[i]->pathL[j]->src==src){
					
					int src2=appL[winner]->path_record[i]->pathL[j]->src;
					int srcPort=appL[winner]->path_record[i]->pathL[j]->srcPort;
					int dst=appL[winner]->path_record[i]->pathL[j]->dst;
					int dstPort=appL[winner]->path_record[i]->pathL[j]->dstPort;
					pathFile<<src2<<"_"<<srcPort<<","<<dst<<"_"<<dstPort<<",";

					src=appL[winner]->path_record[i]->pathL[j]->dst;
					appL[winner]->path_record[i]->pathL[j]=empty_edge;
					break;
				}
		}
		pathFile<<reqL[i]->dst<<"_1"<<endl;
	}

	//����ͶƱwinner�����
	double happiness=0;//һ��������������Ⱥͣ�Խ��Խ��,0<=�����<=1
	for(int j=0;j<APPNUM;j++)
		happiness += gv.cost_best[j]/table[j][winner];//��þ�������/��ǰ�������� 

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
	cout <<"Voting Winner �����Ľ��"<<endl;
	cout << "��������ȣ� " << happiness/APPNUM << endl;
	cout << "���������ȷ�� " << s2_voting << endl;
	cout << "����������ʱ��: " << latencyVoting << endl;
	*/
	double maxUtil_Voting=0;
	for(int j=0;j<gv.m;j++)
	{
		int src=gv.incL[j]->src;
		int dst=gv.incL[j]->dst;
		double capacity=gv.incL[j]->capacity;
		if(maxUtil_Voting<(appL[winner]->adj[src][dst]/capacity))
			maxUtil_Voting=appL[winner]->adj[src][dst]/capacity;
	}
	cout<<"max utilization rate = "<<maxUtil_Voting<<endl;
	
	//ʤ���ķ�����������Ӧ�õ�adj��
	appL[winner]->end_implement(gv,appL);
	cout<<endl;
	
}

#endif