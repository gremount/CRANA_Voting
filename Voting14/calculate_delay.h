#ifndef CALCULATE_DELAY_H
#define CALCULATE_DELAY_H

#include "graph.h"
#include "app.h"
#include "common.h"

//Voting统计网络latency
double judge_sum_function(VGraph &g, vector<APP*> &appL, int winner)
{
	double latency=0;
	for(int i=0;i<M;i++)
	{
		int src,dst;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		double x=appL[winner]->adj[src][dst];
		double capacity= g.incL[i]->capacity;
		latency += linearCal(x,capacity);
	}
	return latency;
}

//TE全局优化后统计网络latency
double delay_TENetworkGraph(TENetworkGraph &g)
{
	double latency=0;
	for(int i=0;i<M;i++)
	{
		int src,dst;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		double capacity=g.incL[i]->capacity;
		latency += linearCal(g.adj[src][dst],capacity);
	}
	return latency;
}

//Delay全局优化后统计网络latency
double delay_DelayNetworkGraph(DelayNetworkGraph &g)
{
	double latency=0;
	for(int i=0;i<M;i++)
	{
		int src,dst;
		src=g.incL[i]->src;dst=g.incL[i]->dst;
		double capacity=g.incL[i]->capacity;
		latency += linearCal(g.adj[src][dst],capacity);
	}
	return latency;
}

#endif