#include"app_net_voting.h"

const int Inf=9999;
const int Rinf=0.001;
int APPNUM=0;//app num
int N=0;//nodes num
int M=0;//edges num
int REQNUM=0;//req num

//�����ͼ����Ҫ�޸ģ� ����Ĳ��� + ͼ���� + req����Ĳ���

int main()
{
	srand((unsigned)time(NULL));
	string graph_address="C:\\CRANA\\fl\\floodlight\\src\\main\\java\\net\\floodlightcontroller\\crana\\files\\status\\sflowStatistic.tsv";
	string req_address="C:\\CRANA\\fl\\floodlight\\src\\main\\java\\net\\floodlightcontroller\\crana\\files\\info\\Demands.tsv";
	string path_address="C:\\CRANA\\fl\\floodlight\\src\\main\\java\\net\\floodlightcontroller\\crana\\files\\cache\\path.tsv";

	app_net_voting(graph_address, req_address, path_address);
	
	return 0;
}