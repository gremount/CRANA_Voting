#include"app_voting.h"

//ABILENE2.txt图测试参数
const int APPNUM = 10;//app num
const int NETNUM = 1;//net num 这里只有TE为目标的网络模块
const int MAXREQ = 100;//req num
const int MAXFLOW = 30;//max flow size
const int MINFLOW = 10;//min flow size
const int TESTNUM = 1000;//test num

/*
//graph_t3.txt图测试参数
int APPNUM = 3;//app num
int MAXREQ = 2;//req num
int MAXFLOW = 20;//max flow size
int MINFLOW = 10;//min flow size
int TESTNUM = 10;//test num
*/

const double INF = 1e7;
const double RINF = 1e-7;
int N;//nodes num
int M;//edges num
const int DECNUM = APPNUM + NETNUM;//decider num (app + net)

int main()
{
	srand((unsigned)time(NULL));
	string graph_address="Abilene2.txt";
	string req_address="req.txt";
	string result_address="result.txt";
	vector<vector<double> > teL;
	vector<double> teL_avg;
	vector<vector<double> > haL;
	vector<double> haL_avg;

	teL.resize(TESTNUM);
	teL_avg.resize(MAXREQ);
	haL.resize(TESTNUM);
	haL_avg.resize(MAXREQ);
	
	for (int i = 0; i < TESTNUM; i++){
		cout << "TEST " << i << endl;
		app_net_voting(graph_address, req_address, result_address, teL[i], haL[i]);
		for (int j = 0; j < MAXREQ; j++){
			teL_avg[j] += teL[i][j];
			haL_avg[j] += haL[i][j];
		}
	}

	ofstream resultFile(result_address);
	for (int i = 0; i < MAXREQ; i++){
		teL_avg[i] /= TESTNUM;
		haL_avg[i] /= TESTNUM;
		resultFile << haL_avg[i] <<" "<<teL_avg[i] << endl;
	}

	getchar();
	return 0;
}