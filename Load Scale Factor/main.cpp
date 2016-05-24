#include "common.h"
#include "graph.h"

//利用随机数大小的不同，产生不同的load scale factor
//load scale factor的定义见 “On Selfish Routing in Internet-Like Environment”

int main()
{
	srand((unsigned)time(NULL));
	Graph G("graph_ATT_big.txt");
	cout<<"node_num = "<<G.get_node_num()<<endl;
	cout<<"edge_num = "<<G.get_edge_num()<<endl;
	double te=0;
	double te_all=0;
	double te_ave=0;
	for(int m=1;m<=100;m++)
	{
		te_ave=0;
		te_all=0;
		for(int i=0;i<10000;i++)
		{
			G.tm_gen(m);
			G.floyd();
			G.implement();
			te = G.te_cal();
			te_all += te;
			G.clean_ds();
		}
		te_ave=te_all/10000;
		cout<<"m =  "<<m<<" and ave te = "<<te_ave<<endl;
	}
	
	
	cout<<"main completed"<<endl;
	getchar();
	return 0;
}