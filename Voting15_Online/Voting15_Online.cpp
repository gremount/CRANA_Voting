#include"app_voting.h"

const double INF=1e7;
const double RINF=1e-7;
int N;//nodes num
int M;//edges num

//ABILENE2.txt图测试参数
const int APPNUM = 20;//app num
const int MAXREQ = 200;//req num
const int MAXFLOW = 30;//max flow size
const int MINFLOW = 10;//min flow size

/*
//graph_t3.txt图测试参数
int APPNUM = 3;//app num
int MAXREQ = 2;//req num
int MAXFLOW = 20;//max flow size
int MINFLOW = 10;//min flow size
*/

//如果改图，需要修改： 上面的参数 + 图输入 + req输入的部分

int main()
{
	srand((unsigned)time(NULL));
	string graph_address="Abilene2.txt";
	string req_address="req.txt";
	string result_address="result.txt";

	app_voting(graph_address, req_address, result_address);

	getchar();
	return 0;
}