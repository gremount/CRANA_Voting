Voting10-突破0504 特点：

基于Voting3 项目

网络的目标是 TE
应用的目标是 延时小

网络和应用各有一定的决策权重
网络：应用 = 1 : 2 --- 网络的方案虽然不会胜出，但是选出的流方案是TE比较好的
网络：应用 = 1 : 1 --- 网络的方案都会胜出，网络的影响力过强

新增决策者：
Network_LB: (Load Balance) 希望网络的最大链路利用率最小
Neutral: 希望应用和网络都能满意

Voting10架构:
Voting10.cpp为程序入口

	* common.h + res.h (库函数和全局参数)
	* graph.h (图类)
	* 
		* VGraph---投票用的图类
		* PGraph---对比规划用的图类

	* voter.h (决策实体)
	* 
		* flow.h --- LP_Voting.h (应用候选者和投票者)
		* 
			* flow1: 先按照delay小部署flow1，然后按照其他流满意为目标安排其他流
			* flow2: 先按照delay小部署flow2，然后按照其他流满意为目标安排其他流
			* ...
			* flowN: 先按照delay小部署flowN，然后按照其他流满意为目标安排其他流

		* network.h (网络候选者和投票者)
		* neutral.h (应用和网络利益坚固的候选者)
		* 
			* neutral_low: winner_te + 0.05   (初始化为0.3)
			* neutral_middle: winner_te + 0.1  (初始化为0.4)
			* neutral_high: winner_te + 0.2  (初始化为0.5)


	* LP.h (用来对比的网络利益方案，以TE为目标)
	* voting.h (根据投票结果，算出胜者，这里有4种投票算法)
	* 
		* k==1 Schulze（目前使用）
		* k==2 Cumulative
		* k==3 Condorcet
		* k==4 Ranked Pairs


