#include"app_voting.h"

const double INF=1e7;
const double RINF=1e-7;
int N;//nodes num
int M;//edges num

//ABILENE2.txt图测试参数
const int APPNUM = 10;//app num
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

//如果改图，需要修改： 上面的参数 + 图输入 + req输入的部分

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
		app_voting(graph_address, req_address, result_address, teL[i], haL[i]);
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