#include"app_voting.h"

const double INF=9999;
const double RINF=0.001;
int N;//nodes num
int M;//edges num
int MAXREQ;//req num

int APPNUM = 5;//app num

//如果改图，需要修改： 上面的参数 + 图输入 + req输入的部分

int main()
{
	srand((unsigned)time(NULL));
	string graph_address="ABILENE2.txt";
	string req_address="req.txt";
	string result_address="result.txt";

	app_voting(graph_address, req_address, result_address);

	getchar();
	return 0;
}