#include"app_voting.h"

const int Inf=9999;
const int Rinf=0.001;
int APPNUM=0;//app num
int N=0;//nodes num
int M=0;//edges num
int Maxreq=0;//req num

//如果改图，需要修改： 上面的参数 + 图输入 + req输入的部分

int main()
{
	srand((unsigned)time(NULL));

	app_voting(graph_address, req_address, path_address);
	
	return 0;
}