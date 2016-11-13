#include"common.h"
#include"resources.h"
#include"LP.h"

int main()
{
	Graph g("graph_t3.txt");
	ifstream reqfile("req.txt");
	int reqN=4;
	int a,b,c;
	vector<Req*> reqL;
	reqL.clear();
	for(int i=0;i<reqN;i++)
	{
		reqfile>>a>>b>>c;
		Req* r=new Req(a,b,c);
		reqL.push_back(r);
	}
	//线性规划部署
	cout<<"--------------------------------------------------"<<endl;
	double result=0;
	result=LP(&g,reqL);

	cout<<"TE of this case is: "<<result<<endl;
	cout<<"End of the Program"<<endl;
	
	getchar();
	return 0;
}