#include"app_voting.h"

const double INF=1e7;
const double RINF=1e-7;
int N;//nodes num
int M;//edges num

//ABILENE2.txtͼ���Բ���
const int APPNUM = 20;//app num
const int MAXREQ = 200;//req num
const int MAXFLOW = 30;//max flow size
const int MINFLOW = 10;//min flow size

/*
//graph_t3.txtͼ���Բ���
int APPNUM = 3;//app num
int MAXREQ = 2;//req num
int MAXFLOW = 20;//max flow size
int MINFLOW = 10;//min flow size
*/

//�����ͼ����Ҫ�޸ģ� ����Ĳ��� + ͼ���� + req����Ĳ���

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