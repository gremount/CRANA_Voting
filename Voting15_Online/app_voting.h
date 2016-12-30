#ifndef APP_VOTING_H
#define APP_VOTING_H

#include"app.h"
#include"voting.h"

void app_voting(string graph_address, string req_address, string result_address)
{
	//����ͼ
	VGraph gv(graph_address);
	cout << "graph complete" << endl;

	//���� req.txt
	ofstream reqOutFile(req_address);
	reqOutFile << MAXREQ << endl;
	for (int i = 0; i < MAXREQ; i++){
		int app_id = rand() % APPNUM;
		int src = rand() % N;
		int dst = rand() % N;
		while (src == dst){
			dst = rand() % N;
		}
		double flow = 1.0 + MINFLOW + rand() % MAXFLOW;
		reqOutFile << i << " " << app_id << " " << src << " " 
			<< dst << " " << flow << endl;
	}

	//������������: (id app_id src dst flow)
	ifstream reqFile(req_address);
	int reqNum=0;
	reqFile>>reqNum;

	//************************  ��ʼ��  ***********************
	//ͶƱϵͳ��ʼ��
	double table[M2C+1][N2C+1] = {0};//ͶƱ�õ�����
	int ranking[N2C+1]={0};//��¼һ�������ͶƱ����
	for(int j=0;j<reqNum;j++)
		ranking[j]=1;//ÿ��ͶƱ�����1��voter,���Ϊ2��˵���÷����еõ�����voter��Ʊ
	for(int j=0;j<reqNum;j++)
		for(int k=0;k<reqNum;k++)
			table[j][k]=0;
	cout << "voting init complete" << endl;

	//����APP������ʼ��
	vector<APP*> appL;//��¼���е�APP
	for(int k=0;k<APPNUM;k++)
		appL.push_back(new APP(k,gv));
	for(int j=0;j<APPNUM;j++)
		appL[j]->init();
	cout << "APP init complete" << endl;

	//��ʼ��������
	vector<Req*> reqL;//������
	reqL.clear();
	for(int j=0;j<reqNum;j++)
	{
		int id = 0, app_id = 0, a = 0, b = 0;
		double c=0;
		reqFile>>id>>app_id>>a>>b>>c;
		Req* r = new Req(id,app_id,a,b,c);
		reqL.push_back(r);
	}
	cout << "read request complete" << endl<<endl;
	
	ofstream resultFile(result_address);

	//���ÿһ��req����ͶƱ
	for (int i = 0; i < reqL.size(); i++)
	{
		cout << "request " << i << endl;
		resultFile << "request " << i << endl;
		for (int j = 0; j<APPNUM; j++)
			appL[j]->init();
		//************************  ͶƱ���ƿ�ʼ  **********************
		//�᷽��
		for (int j = 0; j<APPNUM; j++){
			appL[j]->propose(*reqL[i]);
			//cout << "app " << j << " proposal is " << endl;
			//for (int k = 0; k < appL[j]->pathRecord.size(); k++)
				//cout << appL[j]->pathRecord[k] << " ";
			//cout << endl;
		}
		//cout << "propose complete" << endl;

		//���۷���
		for (int j = 0; j<APPNUM; j++)
			appL[j]->evaluate(*reqL[i], appL);
		//cout << "evaluate complete" << endl;

		
		//ͶƱ�㷨
		for (int j = 0; j<APPNUM; j++)
			for (int k = 0; k<APPNUM; k++)
			{
				if (appL[j]->judge[k] == 0) table[j][k] = RINF;//��ֹ���� ��0
				else table[j][k] = appL[j]->judge[k];
			}

		int choice = 1;//ѡ��һ��ͶƱ�㷨
		int winner = 0;
		Voting vv(table, ranking, APPNUM, APPNUM);
		winner = vv.voting(choice);
		cout << "schulze winner = " << winner << endl;
		resultFile << "schulze winner = " << winner << endl;

		// file record of table show
		resultFile << endl;
		for (int i = 0; i < appL.size(); i++)
		{
			resultFile << "flow ";
			resultFile.setf(ios::right);
			resultFile.width(3);
			resultFile << i << " judge: ";
			for (int j = 0; j < appL.size(); j++){
				resultFile.setf(ios::left);
				resultFile << setw(10) << table[i][j] << " ";
			}
			resultFile << endl;
		}
		resultFile << endl;

		//����ͶƱwinner�����
		double happiness_sum = 0;
		double happiness_avg = 0;
		for (int j = 0; j < APPNUM; j++)
			happiness_sum += table[j][j] / table[j][winner];
		happiness_avg = happiness_sum / APPNUM;
		cout << "happiness_avg = " << happiness_avg << endl;
		resultFile << "happiness_avg = " << happiness_avg << endl;
		
		//ʤ���ķ�������gvͼ��adj���req��Ӧ��APP��adjMyFlow��
		for (int i = 0; i < appL[winner]->pathRecord.size() - 1; i++){
			int tail = appL[winner]->pathRecord[i];
			int head = appL[winner]->pathRecord[i + 1];
			gv.adj[tail][head] += reqL[i]->flow;
			appL[reqL[i]->app_id]->adjMyFlow[tail][head] += reqL[i]->flow;
		}
		cout << endl;
		
	}
}

#endif