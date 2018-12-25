#include <curses.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>

#include <vector>
#include <map>
#include <string>
//��Ҫ��2��ͷ�ļ�
#include <iostream>
#include <fstream>

#include <sstream>

//sort()
#include<algorithm>
using namespace std;

//������
#define N 4
// ÿ�����ӵ��ַ���  
#define WIDTH 5 
//������ʾƫ����
const int offset = 12;

const string file = "./rank.txt";
#define S_FAIL  0
#define S_WIN 1
#define S_NORMAL 2
#define S_QUIT 3

#define TARGET 2048
//��map����value����
typedef pair<string,int> pii;
//ʵ�ֱȽϺ���
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
		
		//��ȡ״̬
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
				//��Ҫ����Ƿ���±��̣����µĻ���������µ����֣�
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
		
		//���¿�ʼ
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
		//�������һ�����֣�
		bool randNew()
		{
			//Ϊ�˴ﵽ�����Ҫ�����п�λ�ö�Ҫ�ҳ�����Ȼ�����������
			vector<int> emptyPos;
			//����λ�õ�����ȫ������һ��
			for(int i = 0; i < N; i++)
				for(int j=0; j < N; j++)
				{
					if(data[i][j] == 0)
					{
						emptyPos.push_back(i * N + j);
					}
				}
			//�ж��Ƿ�������������Ϸ
			if(emptyPos.size() == 0)
				return false ;
			
			//���ѡλ��
			int value = emptyPos[rand() % emptyPos.size()];
			//10% ���ʲ��� 4
			data[value / N][value % N] = rand() % 10 == 4 ? 4 : 2;
			
			return true;
		}
			
		//��ͼ
		void draw ()
		{
			//������Ļ
			clear();
			//��ͼ
			for(int i =0; i<=N ;i++)
			{
				for(int j=0;j<=N;j++)
				{
					//�ཻ�㣬5�ı���
					drawItem(i * 2,j * WIDTH + offset,'+');
					//���� i�����������һ�����⴦��
					if(i!=N)
						drawItem(i*2+1, j* WIDTH + offset , '|');
					//����
					if(j!=N)
					{
						for(int k=1;k <=N;k++)
						{
							drawItem(i*2,j*WIDTH+offset+k,'-');
						}
					}
					//���֣���Ҫ���⴦����Ϊλ����ԭ��,��ߴﵽ2048��
					if(j!=N && i!=N)
					{
						drawNum(i*2+1,(j+1)*WIDTH+offset-1,data[i][j]);
					}
				}
			}	
				
			mvprintw(2*N + 2, (5*(N-4)-1)/2,"����������W(����),S(����),A(����),D(����),R(�ؿ�),Q(�˳�)");
			mvprintw(2*N + 3, 12 +5*(N-4)/2,"Q�뿪ʱ��ϵͳ���Զ�������߷֣�made by linwx");
			
			//��ʾrank��,Ҫ������ʾ��,��֪��Ϊʲô������+1����Ȼ���ӡ2���1�����ݣ�
			mvprintw(0,13*N+offset," ��rank��");
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
			s1 <<"Ŀǰ�÷֣�"<<Game2048::name <<" "<<score;
			mvprintw(10,13*N+offset,s1.str().c_str());	
			//ֱ�ӽ��ַ�׷�ӣ���ֱ�Ӹ��ǵ����û��������鵽�κβ��
			if(status == S_WIN)
			{
				mvprintw(N,5*N /2 -1 , "��ϲ���������ʤ����");
				
			}
			else if(status == S_FAIL){
				mvprintw(N,5*N /2 -1 ,"�ź�����Ϸ���������롱R�����¿�ʼ��");
			}
		}	
		
		void setName(string name)
		{
			Game2048::name = name;
		}
		//��ߵķ���
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
		
		
		//��ȡrank��
		bool rank()
		{
			string tmpName;
			int tmpScore;
			//��֤�ļ�����һ��Ŀ¼��
			//string file = "./rank.txt";
			//��ʼ�������ļ�,linux����Ҫ+c_str()
			ifstream readFile(file.c_str());
			
			if(!readFile.is_open())
			{
				return false;
			}
			//ѭ����ദ��һ�Σ�������4�Σ���Ϊ���벻���ظ��ģ�����map��3����vc��4�������һ��������2�Σ�
			while(!readFile.eof())
			{
				readFile >> tmpName >> tmpScore ;
				rankMap.insert(pair<string,int>(tmpName,tmpScore));
				//���ӵ�vector��
				//vc.push_back(pii(tmpName,tmpScore));
				//cout<<"---"<< endl;
			}
			//������������룡
			for(map<string,int>::iterator iter = rankMap.begin();iter!=rankMap.end();iter++)
			{
				vc.push_back(pii(iter->first,iter->second));
			}
			//����
			sort(vc.begin(),vc.end(),cmp);
			readFile.close();
			return true;
		}
		
		//д���ļ���
		bool saveTopScore()
		{
			ofstream writeFile;
			//out��ֱ�Ӹ���ԭ�ļ�
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
				//key�����˲������ٲ���,���Ը���
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
		//��ǰ�û����ֺͷ���
		int score;
		string name;
		//rank��
		map<string,int> rankMap;
		
		
		//�Ƿ����
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
		//��������
		void setTestData()
		{
			for(int i=0;i<N;i++)
				for(int j=0;j<N;j++)
				{
					data[i][j] = 16 << i<<j ;
				}
		}
		//���ַ�
		void drawItem(int row,int col, char c)
		{
			move(row,col);
			addch(c);
		}
		//������
		void drawNum(int row,int col ,int num)
		{
			while(num!=0)
			{
				//��Ҫת�����ַ�
				drawItem(row,col,num%10+'0');
				num/=10;
				col--;
			}
		}
		//�ؼ��㷨 �����������Ƿ����仯��
		bool  moveLeft(){
			int tmp [N][N];
			for(int i=0; i < N ;i++)
			{
				//��ǰд��λ��
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
							//���success�ж�
							if( data[i][currentWritePos] ==TARGET)
							{
								status = S_WIN;
							}
							
						}
						else
						{
							//��Ϊ���������ұߵ���Ҫ������
							data[i][currentWritePos] = lastValue;
							lastValue = data[i][j];
						}
						++currentWritePos;
					}
					//��ԭ������Ϊ0��
					data[i][j] = 0;
				}
				if(lastValue != 0){
					data[i][currentWritePos] = lastValue;
				}
			}
			
			//����Ƿ����
			for(int i = 0; i<N; i++){
				for(int j= 0;j < N;j++)
				{
					if(data[i][j] != tmp[i][j])
						return true;
				}
			}
			
			return false;
		}
		
		//��ʱ����ת90��,���ݽ�����������ȡ�෴������������ = ����+3��
		//1 2 3  3 6 9
		//4 5 6  2 5 8
		//7 8 9 1 4 7
		//��ת
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
	//ϵͳ����:����
	system("clear");
	cout<< "�����������ǳƣ�";
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
		//mvprintw(5,7,"������");
		game.draw();
		game.processInput();
	}
	//ͳһ���˳���ʱ�򱣴��ȥ
	game.saveTopScore();
	
	shutdown();
	return 0;
	
}