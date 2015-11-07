#include"common.h"
#include"resources.h"
#include"LP.h"

int main()
{
	
	Graph g("d:\\github\\CRANA_Voting\\graph2.txt");
	
	ifstream reqfile("d:\\github\\CRANA_Voting\\req1.txt");
	int reqN,a,b,c;
	vector<Req*> reqL;
	reqfile>>reqN;
	reqL.clear();
	for(int i=1;i<=reqN;i++)
	{
		reqfile>>a>>b>>c;
		Req* r=new Req(a,b,c);
		reqL.push_back(r);
	}

	double result=0;
	result=LP(&g,reqL);
	cout<<result<<endl;

	cout<<"End of the Program"<<endl;
	

	getchar();
	return 0;
}