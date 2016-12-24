#include"app_voting.h"

const int INF=9999;
const int RINF=0.001;
int APPNUM=3;//app num
int N=0;//nodes num
int M=0;//edges num
int MAXREQ=0;//req num

//如果改图，需要修改： 上面的参数 + 图输入 + req输入的部分

int main()
{
	srand((unsigned)time(NULL));
	string graph_address="graph_t3.txt";
	string req_address="req.txt";
	string result_address="result.txt";

	app_voting(graph_address, req_address, result_address);

	getchar();
	return 0;
}