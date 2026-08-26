// Amazons sample 简单交互样例程序
// random strategy (improved by zhouhy)
// Writer: dgf123/syys
// Informations: http://www.botzone.org/games#Amazons


#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <fstream>
#include <thread>
#include <chrono>


#define GRIDSIZE 8
#define OBSTACLE 2
#define judge_black 0
#define judge_white 1
#define grid_black 1
#define grid_white -1

using namespace std;

int currBotColor; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[GRIDSIZE][GRIDSIZE] = { 0 }; // 先x后y，记录棋盘状态
int dx[] = { -1,-1,-1,0,0,1,1,1 };
int dy[] = { -1,0,1,-1,1,-1,0,1 };
int D1[3][10][10], D2[3][10][10];
int sumvis, turnID;
struct Move {
	int x0, y0, x1, y1, x2, y2;
	double score;
};
ifstream input_from_file;
ofstream output_to_file;//file input and output stream

// is it in the map
inline bool inMap(int x, int y)
{
	if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
		return false;
	return true;
}


// set the queen, check if it is legal or just a simulation
bool ProcStep(int x0, int y0, int x1, int y1, int x2, int y2, int color, bool check_only)
{
	if ((!inMap(x0, y0)) || (!inMap(x1, y1)) || (!inMap(x2, y2)))
		return false;
	if (gridInfo[x0][y0] != color || gridInfo[x1][y1] != 0)
		return false;
	if ((gridInfo[x2][y2] != 0) && !(x2 == x0 && y2 == y0))
		return false;
	if (!check_only)
	{
		gridInfo[x0][y0] = 0;
		gridInfo[x1][y1] = color;
		gridInfo[x2][y2] = OBSTACLE;
	}
	return true;
}
//_________________________________________________________________________________

char printqueen(int x){
	if(x == 0) return '.';
	if(x == -1) return 'W';
	if(x == 1) return 'B';
	if(x == OBSTACLE) return 'O';
	return 6;
}

void printboard(){
	cout << "   0  1  2  3  4  5  6  7  --> x" << '\n' << '\n';
	for(int i = 0; i <= 7; ++i){
		cout << i;
		for(int j = 0; j <= 7; ++j){
			cout << "  " << printqueen(gridInfo[j][i]);
		}
		cout << '\n' << '\n';
	}
	cout << '|' << '\n' << 'v' << '\n' << 'y' << '\n' << '\n';
}

void printmove(int x0, int y0, int x1, int y1, int x2, int y2){
	int cl = gridInfo[x0][y0];
	
	int xx, yy;
	
	if(x1 > x0)xx = 1;
	if(x1 == x0)xx = 0;
	if(x1 < x0)xx = -1;
	if(y1 > y0)yy = 1; 
	if(y1 == y0)yy = 0;
	if(y1 < y0)yy = -1;
	
	int i = x0;
	int j = y0;
	while(i != x1 or j != y1){
		system("cls");
		printboard();
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		gridInfo[i][j] = 0;
		gridInfo[i + xx][j + yy] = cl;
		i += xx; j += yy;
	}
	system("cls");
	printboard();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	
	gridInfo[x2][y2] = OBSTACLE;
	system("cls");
	printboard();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	
	gridInfo[x2][y2] = 0;
	system("cls");
	printboard();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	
	gridInfo[x2][y2] = OBSTACLE;
	system("cls");                  
	printboard();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
                                                             
bool canmove(int p, int q){
	bool flag = 0;
	for(int i = -1; i <= 1; ++i){
		for(int j = -1; j <= 1; ++j){
			if(inMap(p+i, q+j) and !gridInfo[p+i][q+j])return true;
		}
	}
	return false;
}

bool gameover(int c){
	bool flag = 1;
	for(int i = 0; i < GRIDSIZE; ++i){
		for(int j = 0; j < GRIDSIZE; ++j){
			if(gridInfo[j][i] == c){
				if(canmove(j, i))flag = 0;
			}
		}
	}
	return flag;
}

void D1_ij(int cl){
	
	bool flag[10][10];
	for(int i = 0; i < GRIDSIZE; ++i){
		for(int j = 0; j < GRIDSIZE; ++j)flag[i][j] = 0;
	}
	struct map{
		int x, y;
		int d = 1000;
	}q[100];
	
	int l = 1; int r = 0;
	for(int x = 0; x < GRIDSIZE; ++x){
		for(int y = 0; y < GRIDSIZE; ++y){
			if(gridInfo[x][y] != cl)continue;
			q[++r].x = x, q[r].y = y; q[r].d = 0;
			flag[x][y] = 1;
		}
	}
	//bfs
	
	
	while(l <= r){
		int x0 = q[l].x; int y0 = q[l].y;
		
		//search for 8 orientations
		for(int k = 0; k < 8; ++k){
			for(int i = 1; i < GRIDSIZE; ++i){
				if(!inMap(x0 + i * dx[k], y0 + i * dy[k]))break;
				if(gridInfo[x0 + i * dx[k]][y0 + i * dy[k]] != 0)break;
				if(flag[x0 + i * dx[k]][y0 + i * dy[k]])continue;
				
				r++;
				D1[cl + 1][x0 + i * dx[k]][y0 + i * dy[k]] = q[l].d + 1;
				q[r].d = q[l].d + 1;
				q[r].x = x0 + i * dx[k]; 
				q[r].y = y0 + i * dy[k];
				flag[x0 + i * dx[k]][y0 + i * dy[k]] = 1;
			}
		}
		l++;
	}
	
}

void D2_ij(int cl){
	bool flag[10][10];
	for(int i = 0; i < GRIDSIZE; ++i){
		for(int j = 0; j < GRIDSIZE; ++j)flag[i][j] = 0;
	}
	struct map1{
		int x, y;
		int d = 1000;
	}q[100];
	
	int l = 1; int r = 0;
	for(int x = 0; x < GRIDSIZE; ++x){
		for(int y = 0; y < GRIDSIZE; ++y){
			if(gridInfo[x][y] != cl)continue;
			q[++r].x = x, q[r].y = y; q[r].d = 0;
			flag[x][y] = 1;
		}
	}
	//bfs
	
	
	while(l <= r){
		int x0 = q[l].x; int y0 = q[l].y;
		
		//search for 8 orientations
		for(int k = 0; k < 8; ++k){
			if(!inMap(x0 + dx[k], y0 + dy[k]))continue;
			if(gridInfo[x0 + dx[k]][y0 + dy[k]] != 0)continue;
			if(flag[x0 + dx[k]][y0 + dy[k]])continue;
			
			r++;
			D2[cl + 1][x0 + dx[k]][y0 + dy[k]] = q[l].d + 1;
			q[r].d = q[l].d + 1;
			q[r].x = x0 + dx[k]; 
			q[r].y = y0 + dy[k];
			flag[x0 + dx[k]][y0 + dy[k]] = 1;
		
		}
		l++;
	}
}

double evaluation(bool flag){
	for(int i = 0; i <= 2; ++i){
		for(int j = 0; j < GRIDSIZE; ++j){
			for(int k = 0; k < GRIDSIZE; ++k){
				D1[i][j][k] = 100;
				D2[i][j][k] = 100;
			}
		}
	}
	D1_ij(1); D1_ij(-1);
	D2_ij(1); D2_ij(-1);

//	printD2();
	if(flag){//is ending situation
		int myaccess = 0;
		int opaccess = 0;
		for(int i = 0; i < GRIDSIZE; ++i){
			for(int j = 0; j < GRIDSIZE; ++j){
				if(D1[currBotColor + 1][i][j] <= 500 and D1[-currBotColor + 1][i][j] > 500)myaccess ++;
				if(D1[currBotColor + 1][i][j] > 500 and D1[-currBotColor + 1][i][j] <= 500)opaccess ++;
			}
		}
		return myaccess - opaccess;
	}
	else{//isn't ending situation
		int t1 = 0;
		int t2 = 0;
		double c1 = 0;
		double c2 = 0;
		for(int i = 0; i < GRIDSIZE; ++i){
			for(int j = 0; j < GRIDSIZE; ++j){
				if(D1[currBotColor + 1][i][j] <= 500 and D1[-currBotColor + 1][i][j] <= 500){
					if(D1[currBotColor + 1][i][j] < D1[-currBotColor + 1][i][j])t1++;
					if(D1[currBotColor + 1][i][j] > D1[-currBotColor + 1][i][j])t1--;
					c1 += 2.0 / (double)(1 << min(10, D1[currBotColor + 1][i][j]));
					c1 -= 2.0 / (double)(1 << min(10, D1[-currBotColor + 1][i][j]));
				}
				if(D2[currBotColor + 1][i][j] <= 500 and D2[-currBotColor + 1][i][j] <= 500){
					if(D2[currBotColor + 1][i][j] < D2[-currBotColor + 1][i][j])t2++;
					if(D2[currBotColor + 1][i][j] > D2[-currBotColor + 1][i][j])t2--;
					c2 += min(1.0, max(-1.0, (double)(D2[-currBotColor + 1][i][j] - D2[currBotColor + 1][i][j]) / 6));
				}
			}
		}
		double s = 1.0 - (double)turnID / 50;
		double score = (turnID * t1 + (50 - turnID) * c1) * s / 50
			 + (turnID * t2 + (50 - turnID) * c2) * (1 - s) / 50;
		return score;
	}
	
}

vector<Move> get_all_moves(int color){//get all moves of current color
	vector<Move> moves;
	for(int x0 = 0; x0 < GRIDSIZE; ++x0){
		for(int y0 = 0; y0 < GRIDSIZE; ++y0){//start
			if(gridInfo[x0][y0] != color)continue;
			
			for (int k1 = 0; k1 < 8; k1++){
                for (int i = 1; i < GRIDSIZE; i++){
                    int x1 = x0 + i * dx[k1]; int y1 = y0 + i * dy[k1];//destination
                    if (!inMap(x1, y1) or gridInfo[x1][y1] != 0) break;
					//fake move
                    gridInfo[x0][y0] = 0;
                    gridInfo[x1][y1] = color;
					
                    for (int k2 = 0; k2 < GRIDSIZE; k2++) {
                        for (int j = 1; j < GRIDSIZE; j++) {
                            int x2 = x1 + j * dx[k2], y2 = y1 + j * dy[k2];//obstacle
                            if (!inMap(x2, y2) or gridInfo[x2][y2] != 0) break;
                            
                            moves.push_back({x0, y0, x1, y1, x2, y2, 0});//new move
                        }
                    }

                    // remove the fake move
                    gridInfo[x1][y1] = 0;
                    gridInfo[x0][y0] = color;
                }
            }
		}
	}
	return moves;
}

void EVstrategy(int cl){
	vector<Move> candidates = get_all_moves(cl);
	bool is_ending;
	if(turnID > 20)is_ending = true;
	else is_ending = false;
	
	double maxscore = -0x3fffffff;
	Move best = {0, 0, 0, 0, 0, 0, 0};
	for(Move m : candidates){
		gridInfo[m.x0][m.y0] = 0;
		gridInfo[m.x1][m.y1] = cl;
		gridInfo[m.x2][m.y2] = OBSTACLE;
		
		m.score = evaluation(is_ending);
		if(m.score > maxscore){
			best = m;
			maxscore = m.score;
		}
		
		gridInfo[m.x1][m.y1] = 0;
		gridInfo[m.x2][m.y2] = 0;
		gridInfo[m.x0][m.y0] = cl;
	}
	
	printmove(best.x0, best.y0, best.x1, best.y1, best.x2, best.y2);
	system("cls");
	
	cout << "Turn: " << turnID << endl << endl;
	printboard();
	
	cout << "AI's move: ";
	cout << best.x0 << " " << best.y0 << " " << best.x1 << " " 
		<< best.y1 << " " << best.x2 << " " << best.y2 << " " << endl;
	
	output_to_file.open("./data.txt",ios::app);
					
	output_to_file << currBotColor << " " << best.x0 << " " << best.y0 << " " << best.x1 << " " 
		<< best.y1 << " " << best.x2 << " " << best.y2 << " " << endl;
	
	output_to_file.close();
}

//_________________________________________________________________________________
int main()
{
	int x0, y0, x1, y1, x2, y2;
	
	
	// Initialize the board
	gridInfo[0][(GRIDSIZE - 1) / 3] = gridInfo[(GRIDSIZE - 1) / 3][0]
		= gridInfo[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][0]
		= gridInfo[GRIDSIZE - 1][(GRIDSIZE - 1) / 3] = grid_black;
	gridInfo[0][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = gridInfo[(GRIDSIZE - 1) / 3][GRIDSIZE - 1]
		= gridInfo[GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)][GRIDSIZE - 1]
		= gridInfo[GRIDSIZE - 1][GRIDSIZE - 1 - ((GRIDSIZE - 1) / 3)] = grid_white;
	
	
	turnID = 0;
	int ifcontinue;//new or stored
	int color, yourcolor;
	
	cout << "Start a new game, input 1; continue the stored game, input -1: ";
	cin >> ifcontinue ;
	
	if(ifcontinue == -1){
		input_from_file.open("./data.txt",ios::in);
		input_from_file >> yourcolor;
		
		
		while(input_from_file >> color)                                                         
		{
			//restore the board according to the input and the output to this turn
			if(color == 1 and x0 != -1)turnID ++;
			input_from_file >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
			if (x0 != -1)ProcStep(x0, y0, x1, y1, x2, y2, color, false);
		}
		
		input_from_file.close();
	}
	else{
		output_to_file.open("./data.txt");
		cout << "Black is 1 and White is -1. " << endl << "Please choose your color: ";
		cin >> yourcolor;
		output_to_file << yourcolor << endl;
		output_to_file.close();
	}
	
	//print the board before starting
	system("pause");
	system("cls");
	cout << "Turn: " << turnID << endl << endl;
	printboard();
	
	//start game
	if(yourcolor == 1)cout << "Your color is Black. " << endl;
	if(yourcolor == -1) cout << "Your color is White. " << endl;

	system("pause");
	
	output_to_file.open("./data.txt",ios::app);
	
	
	if(yourcolor == 1){
		output_to_file << 1 << ' ';
		for(int i = 1; i <= 6; ++i)output_to_file << -1 << ' ';
		output_to_file << endl;
	}
	output_to_file.close();
	
	int crtcolor;
	if(ifcontinue == -1)crtcolor = yourcolor;
	else crtcolor = grid_black;
	while(1){
		//gameover judge
		if(gameover(crtcolor)){
			system("cls");
			printboard();
			if(crtcolor == grid_black)cout << endl << "White wins!!" << endl << endl;
			else cout << endl << "Black wins!!" << endl;
			break;
		}
		
		
		//your turn
		if(crtcolor == yourcolor){
			turnID ++;
			while(1){
				cout << "If You want to stop ,input six -2. " << endl << "Input your move: ";
				cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
				
				if(x0 == -2 and y2 == -2){//stop the game
					turnID--;
					cout << "the game is stored successfully. welcome back to continue the game! ";
					return 0;
				}
				
				if(ProcStep(x0, y0, x1, y1, x2, y2, crtcolor, true)){
					printmove(x0, y0, x1, y1, x2, y2);				
					break;
				}
				else cout << "ERROR! Invalid Move!" << endl;
			}
			system("cls");
			cout << "Turn: " << turnID << '\n' << '\n';
			printboard();
			
			cout << "If you regret to make this move, Input r. else input c to continue: ";
			
			//regret to move
			char regret;
			cin >> regret;
			if(regret == 'r'){
				turnID --;
				gridInfo[x1][y1] = 0;
				gridInfo[x2][y2] = 0;
				gridInfo[x0][y0] = yourcolor;
				crtcolor = -crtcolor;
				system("cls");
				cout << "Turn: " << turnID << '\n' << '\n';
				printboard();
			}
			if(regret == 'c'){
				output_to_file.open("./data.txt",ios::app);
					
				output_to_file << yourcolor << ' ';
				output_to_file << x0 << ' ' << y0 << ' ' << x1 << ' '
					 << y1 << ' ' << x2 << ' ' << y2 << endl;
				
				output_to_file.close();
			}
			
		}
		//AI's turn
		else{
			currBotColor = crtcolor;
			EVstrategy(currBotColor);
		}
		
		//turn to opposite 
		crtcolor = -crtcolor;
	}

	return 0;
}