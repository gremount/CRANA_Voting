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
		int beat_record[N2C+1];
		Voting(double table[][N2C+1], int ranking[],int m2,int n2)
		{
			M2 = m2;
			N2 = n2;
			for(int i=1;i<=N2;i++)
				rank[i]=ranking[i];
			for(int i=1;i<=M2;i++)
				for(int j=1;j<=N2;j++)
					t[i][j]=table[i][j];
		}
		int voting(int k)
		{
			srand((unsigned)time(NULL));
			if(k==1) return Schulze_Voting();
			else if(k==2) return Cumulative_Voting();
			else if(k==3) return Condorcet_Voting();
			else {cout<<"error"<<endl;return 0;}
		}

		int Schulze_Voting()
		{
			int d[M2C + 1][M2C + 1] = { 0 };
			int p[M2C+1][M2C+1]={0};
			int max_win = 0;//record the score of final winner
			vector<int> winners;
			for (int j = 1; j <= M2; j++)
			{
				for (int i = 1; i <= N2 - 1; i++)
					for (int h = i + 1; h <= N2; h++)
						if (t[j][i] < t[j][h]) d[i][h]+=rank[j];//排名数字越小越好
						else if(t[j][i] > t[j][h])d[h][i]+=rank[j];
			}

			//the initial of p[i][j]
			for(int i=1;i<=N2;i++)
			{
				for(int j=1;j<=N2;j++)
					if(i!=j)
					{
						if(d[i][j]>=d[j][i])
							p[i][j]=d[i][j];
						else
							p[i][j]=0;
					}
			}

			//floyd
			for(int k=1;k<=N2;k++)
			{
				for(int i=1;i<=N2;i++)
				{
					if(i==k)continue;
					for(int j=1;j<=N2;j++)
					{
						if(j==k || j==i)continue;
						p[i][j]=max(p[i][j],min(p[i][k],p[k][j]));
					}
				}
			}

			//find the winner
			int beat[M2C+1]={0};
			for(int i=1;i<=N2;i++)
			{
				for(int j=1;j<=N2;j++)
				{
					if(i==j) continue;
					if(p[i][j]>=p[j][i]) beat[i]++;
				}
			}

			cout<<"beat num: ";
			for(int i=1;i<=N2;i++)
			{
				beat_record[i]=beat[i];
				cout<<beat[i]<<" ";
			}
			cout<<endl;

			for(int i=N2-1;i>=1;i--)
			{
				for(int j=1;j<=N2;j++)
					if(beat[j]==i) return j;
			}

			cout<<"no winner"<<endl;
			return 0;
		}

		int Cumulative_Voting()
		{
			double small_score[M2C+1][N2C+1]={0};
			double big_score[M2C+1]={0};
			double max_score = 0;//record the score of final winner
			vector<int> winners;
			double sum=0;
			//change 0 to 1
			for(int i=1;i<=M2;i++)
				for(int j=1;j<=N2;j++)
					if(t[i][j]==0) t[i][j]=1;

			for(int j=1;j<=M2;j++)
			{
				for(int i=1;i<=N2;i++)
					sum+=1/t[j][i];
				for(int i=1;i<=N2;i++)
					small_score[j][i]=rank[j]*(1/t[j][i])/sum;
			}

			for(int i=1;i<=N2;i++)
				for(int j=1;j<=M2;j++)
					big_score[i]+=small_score[j][i];

			cout<<"beat num: ";
			for(int i=1;i<=N2;i++)
			{
				beat_record[i]=big_score[i];
				cout<<big_score[i]<<" ";
			}
			cout<<endl;

			//find the NO.1 and if there is more than 1 person, return the random one
			max_score = big_score[1];
			for (int i = 2; i <= N2; i++)
				if (big_score[i] >= max_score) max_score = big_score[i];
			for (int i = 1; i <= N2; i++)
				if (big_score[i] == max_score) winners.push_back(i);
			if (winners.size() == 1) return winners[0];
			else
			{
				int randnum;
				randnum = rand() % winners.size();
				return winners[randnum];
			}
		}

		int Condorcet_Voting()
		{
			int small_win[M2C + 1][M2C + 1] = { 0 };
			int big_win[M2C + 1] = { 0 };
			int max_win = 0;//record the score of final winner
			vector<int> winners;
			for (int j = 1; j <= M2; j++)
			{
				for (int i = 1; i <= N2 - 1; i++)
					for (int h = i + 1; h <= N2; h++)
						if (t[j][i] < t[j][h]) small_win[i][h]+=rank[j];//越小越好
						else if (t[j][i] > t[j][h]) small_win[h][i]+=rank[j];
			}
			cout<<"condorcet voting"<<endl;
			for (int i = 1; i <= N2; i++)
				for (int j = 1; j <= N2; j++)
				{
					if (i == j) continue;
					//胜利+1，平均+0，失败-1
					if(small_win[i][j]>small_win[j][i])
						big_win[i] = big_win[i] + 1;
					else if(small_win[i][j]<small_win[j][i])
						big_win[i] = big_win[i] - 1;
				}
			cout<<"condorcet voting 2"<<endl;
			cout<<"beat num: ";
			for(int i=1;i<=N2;i++)
			{
				beat_record[i]=big_win[i];
				cout<<big_win[i]<<" ";
			}
			cout<<endl;

			//find the NO.1 and if there is more than 1 person, return the random one
			max_win = big_win[1];
			for (int i = 2; i <= N2; i++)
				if (big_win[i] >= max_win) max_win = big_win[i];
			for (int i = 1; i <= N2; i++)
				if (big_win[i] == max_win) winners.push_back(i);
			if (winners.size() == 1) return winners[0];
			else
			{
				int randnum;
				randnum = rand() % winners.size();
				return winners[randnum];
			}
		}

		
};