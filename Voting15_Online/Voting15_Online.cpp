#include"app_voting.h"

const double INF=9999;
const double RINF=0.001;
int N;//nodes num
int M;//edges num

int APPNUM = 10;//app num
int MAXREQ = 100;//req num
int MAXFLOW = 20;//max flow size
int MINFLOW = 10;//min flow size

//�����ͼ����Ҫ�޸ģ� ����Ĳ��� + ͼ���� + req����Ĳ���

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