
#include "Angel.h" // include point types and operations
#include <ctime>
#include <vector>
#include <time.h>

using namespace std;
typedef vec4 point4; 
typedef vec4 color4;

#define CHECK_ONLY false
#define CHECK_AND_SET true
#define WHITE_PIECE 1
#define BLACK_PIECE 2
#define TIE 0
#define MINIMUM -1000

#define ROUND 50

#define SEARCH_DEPTH 9

#define STABLE_VALUE_WEIGHT 4.0
#define MOBILITY_VALUE_WEIGHT 3.0
#define FLIPED_PIECE_WEIGHT 2.0
#define PATTERN_WEIGHT 6.0

clock_t t_begin, t_end;
float time_sum = 0, step_sum = 0, aver_time_sum = 0, time_cost;
int round = 0;

int white_times = 0;
int white_win_times = 0;
int black_times = 0;
int black_win_times = 0;

int bw_trigger = 0; //1 for my turn, 2 for enemy's turn

color4 red_frame = color4( 0.6, 0.0, 0.0, 1.0 );
color4 black_pea = color4( 0.0, 0.0, 0.0, 1.0 );
color4 white_pea= color4( 0.8, 0.8, 0.8, 1.0 );
color4 mark_color = color4( 0.0, 0.0, 0.0, 1.0 );
color4 my_color = color4( 1.0, 1.0, 1.0, 1.0 );
color4 enemy_color = color4( 0.0, 0.0, 0.0, 1.0 );

/////Camera unv basis-----------------------------------------------

Angel::vec4 v = vec4(0.0, 1.0, 0.0, 0.0);
Angel::vec4 u = vec4(1.0, 0.0, 0.0, 0.0);
Angel::vec4 eye = vec4(0.0, 0.0, 2.0, 1.0);
Angel::vec4 n = vec4(0.0, 0.0, 1.0, 0.0);
//-----------------------------------------------------------------

//Perspective ------------------------------------------------------
GLfloat fovy   = 18.0;
GLfloat aspect = 1.0; 
GLfloat zNear  = 1.0; 
GLfloat zFar   = 5.0;
//------------------------------------------------------------------

mat4 ctmat = mat4(1.0);
mat4 location;

GLuint color_loc;
GLuint model_view_loc;
GLuint Projection;
GLuint buffer[3];
GLuint loc;

int win_w, win_h;
GLfloat wx, wy;

bool turn = false;
bool finish = false;
bool clicked = false;
int wins = 0;
int mypiece;
int chessboard[64];
int WhitePieceCount = 2;
int BlackPieceCount = 2;
bool newpiece = false;
bool wait = false;
const int ChessBoardSize = 8;

bool Human, NetBattle;


vector<point4> point_location;
vector<int> validSpots;
point4 *points;
point4 piece[33];

class boardStatusTree{
private:

public:
	int pieces[64];
	int player;
	int fliped;
	int putPiece;
	int totalPieceCount;

	vector<int> validSpots;
	boardStatusTree *child;
	boardStatusTree *parent;

	boardStatusTree();
	boardStatusTree( int[] , boardStatusTree *, int);
	boardStatusTree( int[] , vector<int>, int );

	int getPieces( int );
	int evaluate( void );
	int getValidSpotSize( void );
	int countPiece();
	bool cornerExist();
	float position();
};

bool AIMove( int );
void humanMove( int );

int stablePiece(int [], int);
unsigned char stability( int [], int, int);
float AlphaBetaPruning( int, int, float alpha, float beta, boardStatusTree, int, bool );

int get_Cell_Number( float x, float y);

int checkValidity( int, int, bool, int [] );
int checkNeighbor( int, int, char, bool, int [] );
void setNeighbor( int piece, int count, int index, int offset, int []);
int checkAvailableSpot( int, int [], vector<int>& );

void drawChessboard( void );
void displayWinner( int );
void setupConnection( void );

boardStatusTree::boardStatusTree()
{
	for (int i = 0; i < 64; i++)
	{
		pieces[i] = 0;
	}
	parent = NULL;
	child = NULL;
	player = 2;
}

boardStatusTree::boardStatusTree( int board[], boardStatusTree *p, int whoesTurn)
{
	for (int i = 0; i < 64; i++)
	{
		pieces[i] = board[i];
	}
	parent = p;
	child = NULL;

	player = whoesTurn;
}

boardStatusTree::boardStatusTree( int board[], vector<int> spots, int whoesTurn)
{
	for (int i = 0; i < 64; i++)
	{
		pieces[i] = board[i];
	}

	parent = NULL;
	child = NULL;
	player = whoesTurn;
	validSpots = spots;
}

int boardStatusTree::getPieces( int piece)
{
	int sum = 0;
	for (int i = 0; i < 64; i++)
		if( pieces[i] == piece )
			sum ++;
	return sum;
}

int boardStatusTree::evaluate()
{
	float evaluation = 0;

	//how many pieces are there in the board
	int pieceCount = countPiece();

	//position evaluation
	evaluation -= PATTERN_WEIGHT * position();

	//mobility evaluation
	evaluation += MOBILITY_VALUE_WEIGHT * validSpots.size();

	//if there's stable piece
	if ( cornerExist() )
	{
		//stable pieces
		evaluation += stablePiece( pieces, player)* STABLE_VALUE_WEIGHT;
	}	

	//at the beginning of the game
	if ( pieceCount < 25 )
	{
		evaluation += fliped* FLIPED_PIECE_WEIGHT;
	}


	return evaluation;
}

int boardStatusTree :: countPiece()
{
	int sum = 0;

	for (int i = 0; i < 64; i++)
		if( pieces[i] != 0 )
			sum ++;

	return sum;
}

bool boardStatusTree :: cornerExist()
{
	//corner spot, heightest value
	if( pieces[ 0 ] == player 
		|| pieces[ 7 ] == player 
		|| pieces[ 56 ] == player 
		|| pieces[ 63 ] == player )
		return true;
	else return false;
}

float boardStatusTree :: position()
{
	//float sum;

	int values[] = {
		99 ,-48,  8, -6, -6,  8,-48, 99,
		-48,-48, 16, -3, -3, 16,-48,-48,
		8, 16, -4, -4, -4, -4, 16,  8,
		-6, -3, -4,  0,  0, -4, -3, -6,
		-6, -3, -4,  0,  0, -4, -3, -6,
		8, 16, -4, -4, -4, -4, 16,  8,
		-48,-48, 16, -3, -3, 16,-48,-48,
		99 ,-48,  8, -6, -6,  8,-48, 99,
	};

	int pivots[12] = {
		1,8,9,
		6,14,15,
		48,49,57,
		54,55,62};
	int corners[4] = {0,7,56,63};
	bool valFlag;
	int boardSize = sizeof( pieces )/sizeof(int);
	int pivotSize = sizeof( pivots )/sizeof(int);
	int retVal = 0;

	for (int i = 0; i < 4; i++)
	{
		if ( corners[ i ] == player)
		{
			int point = 0;
			for (int j = 0; j < 3; j++)
			{
				values[ pivots[ i * 3 + point++ ] ] = 36;
			}
		}
	}

	for(int i = 0; i < boardSize; i++)
	{
		retVal + values[i];
	}
	return retVal;
}


point4 marksqure[4] = {
	point4(1.0, 1.0, 0.0, 1.0),
	point4(1.0, -1.0, 0.0, 1.0),
	point4(-1.0, -1.0, 0.0, 1.0),
	point4(-1.0, 1.0, 0.0, 1.0)
};
