#ifndef VOTER_H
#define VOTER_H

#include "common.h"
#include "graph.h"
#include "res.h"
#include "LP_Voting.h"
#include <ilcplex/ilocplex.h>

class Voter
{
public:
	int id;//voter num
    int id_kind;//0 is flow voter; 1 is network voter;

    vector<vector<double> > adj;
    vector<Path*> path_record;
    vector<double> judge;
	
	Voter(int id2, int id_kind2)
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
		judge.resize(Maxreq+2);//+2 ����Ϊ������������ͶƱ��
	}
	
	//�޸�ͶƱ�ߵ���Ϣ
	virtual void modify(int a, int b, int c)=0;

	//�������
    virtual void propose(VGraph &g,vector<Voter*> &flowL)=0;
    
	//���۷���
	virtual void evaluate(VGraph &g, vector<Voter*> &flowL)=0;
    
	//���ղ��𷽰�
	virtual void end_implement(VGraph &g,int winner, vector<Voter*> &voterL)=0;

};


#endif