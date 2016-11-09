#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"
#include "graph.h"
#include "res.h"
#include "LP_Voting.h"
#include <ilcplex/ilocplex.h>

//�����Playerʵ�����Ǿ���ʵ�壬����Ϊ��ѡ�ߣ�Ҳ����ΪͶƱ��
class Player
{
public:
	int id;//player num
    int id_kind;//0 is app player; 1 is network player; 2 is neutral player
	double te; //Ϊneutral��¼��������������·������

    vector<vector<double> > adj;
    vector<Path*> path_record;
    vector<double> judge;
	
	Player(int id2, int id_kind2)
	{
		id=id2;
		id_kind=id_kind2;

		adj.clear();
		path_record.clear();
		judge.clear();

		adj.resize(N);
		for(int i=0;i<N;i++)
			adj[i].resize(N);
		path_record.resize(Maxreq);
		judge.resize(Maxreq+4);//+1 network+3 neutral
	}
	
	//�޸�ͶƱ�ߵ���Ϣ
	virtual void modify(int a, int b, int c)=0;

	//�������
    virtual void propose(VGraph &g)=0;
    
	//���۷���
	virtual void evaluate(VGraph &g, vector<Player*> &flowL)=0;
    
	//���ղ��𷽰�
	virtual void end_implement(VGraph &g,int winner, vector<Player*> &candiL)=0;

};


#endif