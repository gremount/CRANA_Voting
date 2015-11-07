#include"common.h"
#include"resources.h"
#include"LP.h"

int main()
{
	
	Graph g("d:\\github\\CRANA_Voting\\graph2.txt");
	int caseN=5;
	string sAddress;
	sAddress="d:\\github\\CRANA_Voting\\req1.txt";
	for(int icase=1;icase<=caseN;icase++)
	{
		ifstream reqfile(sAddress);
		sAddress[26]++;
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
		cout<<"--------------------------------------------------"<<endl;
		cout<<"case "<<icase<<endl;
		double result=0;
		result=LP(&g,reqL);
		cout<<"cost of this case is: "<<result<<endl;
	}
	

	cout<<"End of the Program"<<endl;
	

	getchar();
	return 0;
}