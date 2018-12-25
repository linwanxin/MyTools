#include <curses.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>

#include <vector>
#include <map>
#include <string>
//需要这2个头文件
#include <iostream>
#include <fstream>

#include <sstream>

//sort()
#include<algorithm>
using namespace std;

//格子数
#define N 4
// 每个格子的字符数  
#define WIDTH 5 
//居中显示偏移量
const int offset = 12;

const string file = "./rank.txt";
#define S_FAIL  0
#define S_WIN 1
#define S_NORMAL 2
#define S_QUIT 3

#define TARGET 2048
//将map按照value排序
typedef pair<string,int> pii;
//实现比较函数
bool cmp(pii a, pii b){
	return a.second > b.second;
}
vector<pii> vc;

class Game2048{
	public:
		Game2048() : status(S_NORMAL),score(0),name("yulenka")
		{
			//setTestData();
			restart();
	
		}
		
		//获取状态
		int getStatus ()
		{
			return status;
		}
		
		void processInput()
		{
			char ch = getch();
			
			if(ch >= 'a' && ch <= 'z')
				ch -= 32;
			
			if(status == S_NORMAL){
				bool updated = false;
				if(ch == 'A')
					updated = moveLeft();
				else if(ch == 'S')
				{
					rotate();
					rotate();
					rotate();
					updated = moveLeft();
					rotate();
					
				}
				else if(ch == 'D')
				{
					rotate();
					rotate();
					updated = moveLeft();
					rotate();
					rotate();
				}
				else if(ch == 'W')
				{
					rotate();
					updated = moveLeft();
					rotate();
					rotate();
					rotate();
				}
				//需要检测是否更新表盘，更新的话随机产生新的数字！
				if(updated)
				{
					randNew();
					if(isOver())
					{
						status = S_FAIL;
					}
				}
				
			}
			
			if(ch == 'Q')
			{
				status = S_QUIT;
			}
			else if(ch == 'R')
			{
				restart();
			}
			
		}
		
		//重新开始
		void restart()
		{
			for(int i=0;i<N;i++)
			{
				for(int j=0;j<N;j++)
				{
					data[i][j] = 0;
				}
			}
			randNew();
			randNew();
			status = S_NORMAL;
		}
		//随机产生一个数字，
		bool randNew()
		{
			//为了达到随机，要将所有空位置都要找出来，然后随机产生！
			vector<int> emptyPos;
			//将空位置的坐标全部存入一个
			for(int i = 0; i < N; i++)
				for(int j=0; j < N; j++)
				{
					if(data[i][j] == 0)
					{
						emptyPos.push_back(i * N + j);
					}
				}
			//判断是否结束满格结束游戏
			if(emptyPos.size() == 0)
				return false ;
			
			//随机选位置
			int value = emptyPos[rand() % emptyPos.size()];
			//10% 概率产生 4
			data[value / N][value % N] = rand() % 10 == 4 ? 4 : 2;
			
			return true;
		}
			
		//画图
		void draw ()
		{
			//清理屏幕
			clear();
			//画图
			for(int i =0; i<=N ;i++)
			{
				for(int j=0;j<=N;j++)
				{
					//相交点，5的倍数
					drawItem(i * 2,j * WIDTH + offset,'+');
					//竖线 i的作用是最后一行特殊处理！
					if(i!=N)
						drawItem(i*2+1, j* WIDTH + offset , '|');
					//横线
					if(j!=N)
					{
						for(int k=1;k <=N;k++)
						{
							drawItem(i*2,j*WIDTH+offset+k,'-');
						}
					}
					//数字，需要特殊处理因为位数的原因,最高达到2048！
					if(j!=N && i!=N)
					{
						drawNum(i*2+1,(j+1)*WIDTH+offset-1,data[i][j]);
					}
				}
			}	
				
			mvprintw(2*N + 2, (5*(N-4)-1)/2,"操作方法：W(向上),S(向下),A(向左),D(向右),R(重开),Q(退出)");
			mvprintw(2*N + 3, 12 +5*(N-4)/2,"Q离开时，系统会自动保存最高分，made by linwx");
			
			//显示rank榜,要排序显示的,不知道为什么，这里+1，不然会打印2遍第1个数据！
			mvprintw(0,13*N+offset," ★rank榜");
			int loop=1;
			for(vector<pii>::iterator itr=vc.begin(); itr!=vc.end();itr++)
			{
				stringstream ss;
				ss << itr->first << " : "<<itr->second;
				mvprintw(++loop,13*N+offset,ss.str().c_str());
			}
			//
			getTopScore();
			stringstream s1;
			s1 <<"目前得分："<<Game2048::name <<" "<<score;
			mvprintw(10,13*N+offset,s1.str().c_str());	
			//直接将字符追加，会直接覆盖掉，用户不会体验到任何差别！
			if(status == S_WIN)
			{
				mvprintw(N,5*N /2 -1 , "恭喜，您获得了胜利！");
				
			}
			else if(status == S_FAIL){
				mvprintw(N,5*N /2 -1 ,"遗憾，游戏结束，输入”R“重新开始！");
			}
		}	
		
		void setName(string name)
		{
			Game2048::name = name;
		}
		//最高的分数
		int getTopScore()
		{
			for(int i =0; i<N; i++)
			{
				for(int j=0;j<N;j++)
				{
					if(score < data[i][j])
						score = data[i][j];
				}
			}
			return score;
		}
		
		
		//读取rank榜
		bool rank()
		{
			string tmpName;
			int tmpScore;
			//保证文件都在一个目录下
			//string file = "./rank.txt";
			//初始化读入文件,linux下需要+c_str()
			ifstream readFile(file.c_str());
			
			if(!readFile.is_open())
			{
				return false;
			}
			//循环会多处理一次，所以是4次，因为插入不了重复的，所以map是3个而vc是4个，最后一个数据是2次！
			while(!readFile.eof())
			{
				readFile >> tmpName >> tmpScore ;
				rankMap.insert(pair<string,int>(tmpName,tmpScore));
				//增加到vector中
				//vc.push_back(pii(tmpName,tmpScore));
				//cout<<"---"<< endl;
			}
			//在这里遍历加入！
			for(map<string,int>::iterator iter = rankMap.begin();iter!=rankMap.end();iter++)
			{
				vc.push_back(pii(iter->first,iter->second));
			}
			//排序
			sort(vc.begin(),vc.end(),cmp);
			readFile.close();
			return true;
		}
		
		//写入文件中
		bool saveTopScore()
		{
			ofstream writeFile;
			//out是直接覆盖原文件
			writeFile.open(file.c_str(),ios::out);
			if(!writeFile.is_open())
			{
				cout<<"Open file failure"<< endl;
				return false;
			}
			int value = 0;
			if(rankMap.count(name) > 0)
			{
				value = rankMap[name];
			}
			
			if(value < score)
			{
				//key存在了不可以再插入,所以更新
				if(rankMap.count(name) >0)
				{
					rankMap[name] = score;
				}
				else
				{
				rankMap.insert(pair<string,int>(name,score));
				}
			}
			for(map<string,int>::iterator itr = rankMap.begin(); itr!=rankMap.end();itr++)
			{
				writeFile << itr->first << " " <<itr->second << endl;
			}
			
			writeFile.close();
			return true;
		}
	

	private :
		int status;
		int data[N][N];
		//当前用户名字和分数
		int score;
		string name;
		//rank榜
		map<string,int> rankMap;
		
		
		//是否结束
		bool isOver()
		{
			for(int i= 0;i< N;i++)
				for(int j =0;j<N;j++)
				{
						if((j+1<N) &&(data[i][j]* data[i][j+1] == 0|| data[i][j] ==data[i][j] == data[i][j+1]))
							return false ;
						 if ((i + 1 < N) && (data[i][j] * data[i+1][j] == 0 || data[i][j] == data[i+1][j])) 
							return false ;
				}
				
			return true;
			
		}
		//测试数据
		void setTestData()
		{
			for(int i=0;i<N;i++)
				for(int j=0;j<N;j++)
				{
					data[i][j] = 16 << i<<j ;
				}
		}
		//画字符
		void drawItem(int row,int col, char c)
		{
			move(row,col);
			addch(c);
		}
		//画数字
		void drawNum(int row,int col ,int num)
		{
			while(num!=0)
			{
				//需要转化成字符
				drawItem(row,col,num%10+'0');
				num/=10;
				col--;
			}
		}
		//关键算法 ：（检测表盘是否发生变化）
		bool  moveLeft(){
			int tmp [N][N];
			for(int i=0; i < N ;i++)
			{
				//当前写入位置
				int currentWritePos = 0;
				int lastValue = 0;
				for(int j=0;j<N; j++)
				{
					tmp[i][j] = data[i][j];
					if(data[i][j] == 0)
					{
						continue;
					}
					
					if(lastValue == 0)
					{
						lastValue = data[i][j];
					}
					else
					{
						if(lastValue == data[i][j])
						{
							data[i][currentWritePos] = lastValue * 2;
							lastValue = 0;
							//添加success判断
							if( data[i][currentWritePos] ==TARGET)
							{
								status = S_WIN;
							}
							
						}
						else
						{
							//因为向左，所以右边的需要进来！
							data[i][currentWritePos] = lastValue;
							lastValue = data[i][j];
						}
						++currentWritePos;
					}
					//将原来的置为0！
					data[i][j] = 0;
				}
				if(lastValue != 0){
					data[i][currentWritePos] = lastValue;
				}
			}
			
			//检测是否更新
			for(int i = 0; i<N; i++){
				for(int j= 0;j < N;j++)
				{
					if(data[i][j] != tmp[i][j])
						return true;
				}
			}
			
			return false;
		}
		
		//逆时针旋转90度,横纵交换，并且纵取相反数！（代码里 = 负数+3）
		//1 2 3  3 6 9
		//4 5 6  2 5 8
		//7 8 9 1 4 7
		//旋转
		void rotate(){
			int tmp [N][N] ={0};
			for(int i = 0;i<N ;i++){
				for(int j = 0; j < N;j++){
					tmp[i][j] = data[j][N -1-i];
				}
			}
			for(int i=0;i< N;i++){
				for(int j = 0;j <N;j++)
					data[i][j] =tmp[i][j];
			}
		}
		
	
		
		
		
};
		
		
		

void initialize()
{
	
	initscr();
	
	cbreak();
	
	noecho();
	
	curs_set(0);
	
	srand(time(NULL));
}

void  shutdown()
{
	endwin();
}


int main()
{
	string tmpname;
	Game2048 game;
	//系统调用:清屏
	system("clear");
	cout<< "请输入您的昵称：";
	cin>> tmpname;
	if(tmpname.compare(""))
	{
		game.setName(tmpname);
	}
	initialize();
	game.rank();	
	while(game.getStatus() != S_QUIT)
	{
		//move(10,20);
		//addch(ch);
		//mvprintw(5,7,"林万新");
		game.draw();
		game.processInput();
	}
	//统一在退出的时候保存进去
	game.saveTopScore();
	
	shutdown();
	return 0;
	
}