#include"common.h"
#include"resources.h"
#include"LP.h"

int main()
{
	srand((unsigned)time(NULL));
	Graph g("d:\\github\\CRANA_Voting\\graph2.txt");
	int caseN=10;
	double judge=0,judge_sum=0;
	int req_sum=0,result_sum=0;
	
	for(int icase=1;icase<=caseN;icase++)
	{
		//case����
		int reqN,a,b,c;
		vector<Req*> reqL;
		reqN=10;
		reqL.clear();
		for(int i=0;i<reqN;i++)
		{
			a=0;b=0;c=0;
			while(a==b || c==0){
				a=rand() % (g.n-1)+1;
				b=rand() % (g.n-1)+1;
				c=rand() % MAXFLOW + 1;
			}
			Req* r=new Req(a,b,c);
			reqL.push_back(r);
		}
		
		//���Ų������
		g.cost_best.clear();
		g.cost_LP.clear();
		//cout<<"35 line"<<endl;
		for(int i=0;i<reqN;i++)
		{
			//cout<<reqL[i]->src<<" "<<reqL[i]->dst<<" "<<reqL[i]->flow<<endl;
			g.cost_best.push_back(reqL[i]->flow * g.dijkstra(reqL[i]->src,reqL[i]->dst,reqL[i]->flow));
		}

		//���Թ滮����
		cout<<"--------------------------------------------------"<<endl;
		cout<<"case "<<icase<<endl;
		double result=0;
		result=LP(&g,reqL);

		//�����Թ滮�����cost
		result_sum += result;
		cout<<"cost of this case is: "<<result<<endl;
		cout<<"cost of all cases is: "<<result_sum<<endl;
		
		//���������
		if(result==999999)
			judge=0;
		else
		{
			judge=0;
			//for(int i=0;i<reqN;i++)
			//	cout<<g.cost_best[i]<<" "<<g.cost_LP[i]<<endl;
			for(int i=0;i<reqN;i++)
				judge += g.cost_best[i]/g.cost_LP[i];
		}
		judge_sum += judge;
		req_sum += reqN;
		cout<<"��������ȣ� "<<judge/reqN<<endl;
		cout<<"��������ȣ� "<<judge_sum/req_sum<<endl;
	}

	cout<<"End of the Program"<<endl;
	
	getchar();
	return 0;
}