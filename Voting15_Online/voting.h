#include "common.h"
//M2 is the number of candidates
//N2 is how many kinds of ranking

class Node2
{
public:
	int id;
	int degree_in;
	list<int> neighbors;
	Node2(){;}
	Node2(int a){id=a;degree_in=0;}
};

class Edge2
{
public:
	int src;
	int dst;
	Edge2(){;}
	Edge2(int a, int b){src=a; dst=b;}
};

bool cmp(pair<int, pair<int,int> > a, pair<int, pair<int,int> > b)
{
	if(a.first>b.first)return true;
	else return false;    
}

class Voting
{
	private:
	public:
		double t[M2C+1][N2C+1];
		int rank[N2C+1];
		int M2, N2;
		Voting(double table[][N2C+1], int ranking[],int m2,int n2)
		{
			M2 = m2;
			N2 = n2;
			for(int i=0;i<N2;i++)
				rank[i]=ranking[i];
			for(int i=0;i<M2;i++)
				for(int j=0;j<N2;j++)
					t[i][j]=table[i][j];
		}

		int voting(int k)
		{
			srand((unsigned)time(NULL));
			if(k==1) return Schulze_Voting();
			else {cout<<"error"<<endl;return 0;}
		}

		int Schulze_Voting()
		{
			int d[M2C + 1][M2C + 1] = { 0 };
			int p[M2C+1][M2C+1]={0};
			int max_win = 0;//record the score of final winner
			vector<int> winners;
			for (int j = 0; j < N2; j++)
			{
				for (int i = 0; i < M2 - 1; i++)
					for (int h = i + 1; h < M2; h++)
						if (t[j][i] < t[j][h]) d[i][h]+=rank[j];//排名数字越小越好
						else if(t[j][i] > t[j][h])d[h][i]+=rank[j];
			}

			//the initial of p[i][j]
			for(int i=0;i<M2;i++)
			{
				for(int j=0;j<M2;j++)
					if(i!=j)
					{
						if(d[i][j]>=d[j][i])
							p[i][j]=d[i][j];
						else
							p[i][j]=0;
					}
			}

			//floyd
			for(int k=0;k<M2;k++)
			{
				for(int i=0;i<M2;i++)
				{
					if(i==k)continue;
					for(int j=0;j<M2;j++)
					{
						if(j==k || j==i)continue;
						p[i][j]=max(p[i][j],min(p[i][k],p[k][j]));
					}
				}
			}

			//find the winner
			int beat[M2C+1]={0};
			for(int i=0;i<M2;i++)
			{
				for(int j=0;j<M2;j++)
				{
					if(i==j) continue;
					if(p[i][j]>=p[j][i]) beat[i]++;
				}
			}

			for(int i=M2-1;i>=0;i--)
			{
				for(int j=0;j<M2;j++)
					if(beat[j]==i) return j;
			}

			cout<<"no winner"<<endl;
			return 0;
		}
		
};