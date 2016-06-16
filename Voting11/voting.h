#include "common.h"
//M2 is the number of voters
//N2 is the number of candidates

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
			else if(k==4) return Ranked_Pairs_Voting();
			else {cout<<"error"<<endl;return 0;}
		}

		//M2 is the number of voters
		//N2 is the number of candidates
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
						if (t[j][i] < t[j][h]) d[i][h]+=rank[j];//数字越小越好p
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

			for(int j=1;j<=N2;j++)
			{
				for(int i=1;i<=M2;i++)
					sum+=1/t[i][j];
				for(int i=1;i<=M2;i++)
					small_score[i][j]=(1/t[i][j])/sum;
			}
			for(int i=1;i<=M2;i++)
				for(int j=1;j<=N;j++)
					big_score[i]+=small_score[i][j]*rank[j];

			//find the NO.1 and if there is more than 1 person, return the random one
			max_score = big_score[1];
			for (int i = 2; i <= M2; i++)
				if (big_score[i] >= max_score) max_score = big_score[i];
			for (int i = 1; i <= M2; i++)
				if (big_score[i] == max_score) winners.push_back(i);
			if (winners.size() == 1) return winners[0];
			else
			{
				int randnum;
				randnum = rand() % winners.size();
				return winners[randnum];
			}
		}

		int Ranked_Pairs_Voting()
		{
			int small_win[M2C + 1][M2C + 1] = { 0 };
			int big_win[M2C + 1] = { 0 };
			int max_win = 0;//record the score of final winner
			vector<int> winners;
			vector<pair<int,pair<int,int> > > comp;
			int winner=0;//the last winner
			for (int j = 1; j <= N2; j++)
			{
				for (int i = 1; i <= M2 - 1; i++)
					for (int h = i + 1; h <= M2; h++)
						if (t[i][j] <= t[h][j]) small_win[i][h]+=rank[j];
						else small_win[h][i]+=rank[j];
			}
			for (int i = 1; i <= M2-1; i++)
				for (int j = i+1; j <= M2; j++)
				{
					if (i == j) continue;
					if(small_win[i][j]<ceil(N2/2.0)) 
					{
						pair<int, int> p0(j,i);
						pair<int, pair<int,int> > p((M2-1-small_win[i][j]),p0);
						comp.push_back(p);
					}
					else
					{
						pair<int, int> p0(i,j);
						pair<int, pair<int,int> > p(small_win[i][j],p0);
						comp.push_back(p);
					}
					/*small_win[i][j]=M-1-small_win[i][j];
					！！！一旦这样换值，如何确定初始值呢-->所以不能换值
				    不用换值，直接把所需要比较的值存入一个list里,然后就可以给这个list排序,
					这里要存储的是三个值，谁和谁比较就有两个值，所获票数比例的值，
					所以这里是一个三元组，用一个类来表示是可以的，或者用pair的嵌套也是可行的
					*/
				}
			//如何使用sort函数来给comp这个list里的元素排序呢？详情见sort_cmp模板
			sort(comp.begin(),comp.end(),cmp);
			vector<Node2*> nodes;//存储点
			list<Edge2*> incident_list;//存储边
			//接下来就是Ranked Pairs 里的 lock操作,这里才用了拓扑排序来判断是否有环路
			for(int k=0;k<(M2-1)*M2/2;k++)
			{
				nodes.clear();
				list<int> zero_list;//存储源点
				int src, dst;
				int nodes_num=M2;//点数量
				int edges_num=(M2-1)*M2/2;//边数量
				nodes.push_back(new Node2(0));//把第一个空间填上，之后就会从1开始填充vector
				//存储点
				for(int i=1;i<=nodes_num;i++)
					nodes.push_back(new Node2(i));
				//存储边
				src=comp[k].second.first;
				dst=comp[k].second.second;
				Edge2* e=new Edge2(src,dst);
				incident_list.push_back(e);
				//记录所有点的入度
				list<Edge2*>::iterator it,iend;
				it=incident_list.begin();
				iend=incident_list.end();
				for(;it!=iend;it++)
				{
    				nodes[(*it)->dst]->degree_in++;
    				nodes[(*it)->src]->neighbors.push_back((*it)->dst);
				}
				//初始化图结束

				//找到开始时所有的源节点
				for(int i=1;i<=nodes_num;i++)
					if(nodes[i]->degree_in==0)zero_list.push_back(i);
				//处理源节点
				while(zero_list.size()!=0)
				{
					int zero;
					zero=zero_list.front();//待处理的源节点
					zero_list.pop_front();//从源节点链表中删除
					//将该源节点关联的所有边删除，具体做法就是将相邻节点的入度减去1
					list<int>::iterator it,iend;
					iend=nodes[zero]->neighbors.end();
					for(it=nodes[zero]->neighbors.begin();it!=iend;it++)
					{nodes[*it]->degree_in--;if(nodes[*it]->degree_in==0)zero_list.push_back(*it);}
				}
				//处理完成后，根据入度判断是否有环
				int flag=0;//flag=1 --> has circle or flag=0--> no circle
				for(int i=1;i<=nodes_num;i++)
					if(nodes[i]->degree_in!=0){flag=1;break;}
				if(flag==1) {incident_list.pop_back();break;}
			}
			//记录所有点的入度
			list<Edge2*>::iterator it,iend;
			it=incident_list.begin();
			iend=incident_list.end();
			for(;it!=iend;it++)
    			nodes[(*it)->dst]->degree_in++;
			for(int i=1;i<=M2;i++)
				if(nodes[i]->degree_in==0){winner=i;break;}
			return winner;
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
						if (t[i][j] <= t[h][j]) small_win[i][h]+=rank[j];//越小越好
						else small_win[h][i]+=rank[j];
			}
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