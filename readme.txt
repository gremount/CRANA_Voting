项目目的：
为网络中的流部署路由

项目开发顺序：
A. Voting1
最初始的voting项目，只有voting的部分；所有的流都采用dijkstra来算路，链路的延时函数是常数

B. LP1
最初是的LP项目，只有线性规划的部分

C. Voting and LP
基于Voting1和项目LP1，第一次将Voting和LP放在一起测试，比较；这里的voting对所有的流都采用dijkstra来计算最短路，第一次采用随机产生的流需求来测试

D. Voting2
基于项目Voting and LP，voting采用1 + (n-1)方案，即一条流走最短路，剩下的流用规划来计算，第一次可以测试任意拓扑。 

E. Voting3
基于项目Voting2，链路的延时函数改为非常数，流的目标保持延时最小，网络的目标改为最大链路利用率最小。

F. Voting5
流的目标是可用带宽最大(吞吐率最大)，网络目标是TE

G. Voting6
流的目标是延时最小，网络目标是节能

H. Voting7
流的目标是吞吐率最大，网络目标是节能

I. Voting10
流的目标是延时小，网络目标是最大链路利用率小，加入网络决策者，中立决策者，基本完成网络和应用的合作

J. Voting11
用Load Scale Factor 来测试 Voting10
(load scale factor 出自论文On seflish routing in Internet-like environtments ) 

K. Voting12
APP-Voting算法的优化，更加严谨的对比方案，自动化实验

L. Voting13
修改场景，一个应用有任意多个流量出现(之前是一个应用一条流)

M. Voting14
真正的离线场景，只有一波流参与