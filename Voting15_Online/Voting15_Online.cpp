#include"app_voting.h"

const int INF=9999;
const int RINF=0.001;
int APPNUM=3;//app num
int N=0;//nodes num
int M=0;//edges num
int MAXREQ=0;//req num

//�����ͼ����Ҫ�޸ģ� ����Ĳ��� + ͼ���� + req����Ĳ���

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