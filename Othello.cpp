#define _WINSOCKAPI_

#include "GL_Objects.h" 
#include "Othello.h"

#include <WINSOCK2.H>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib")

#define SERVERIP "127.0.0.1"
#define PORTNUM 4000

SOCKET clientSocket;
SYSTEMTIME st;

long firstTime = 0;
long totalTime = 0;
long numMoves = 0;

bool sendToServer(int x, int y)
{

	char bytes[5];
	bytes[0]='M';
	bytes[1]= x + '0';
	bytes[2]= y + '0';
	bytes[3]= '\n';
	bytes[4]= '\0';
	const char *sendBytes = bytes;

	send(clientSocket, bytes, strlen(bytes)+ 1,0);

	return true;
}

char *setupSocket()
{
	int err;
	WORD versionRequired;
	WSADATA wsaData;

	versionRequired=MAKEWORD(1,1);
	err=WSAStartup(versionRequired,&wsaData);

	if (!err)
	{
		std::cout << "Client Socket Opened!\n";
	}
	else
	{
		std::cout << "Client Socket Open Failed!\n";
		return "0";
	}
	clientSocket = socket( AF_INET , SOCK_STREAM , IPPROTO_TCP );
	SOCKADDR_IN clientsock_in ;

	string IPin;
	const char * IPAddress;
	int port;

	cout << "Enter IP Address :";
	cin >> IPin ;
	IPAddress = IPin.c_str();
	cout << endl;
	cout << "Enter port number :";
	cin >> port;
	cout << endl;
	clientsock_in.sin_addr.S_un.S_addr=inet_addr( IPAddress );
	clientsock_in.sin_family=AF_INET ;
	clientsock_in.sin_port=htons(port) ;

	connect(clientSocket , (SOCKADDR*)& clientsock_in , sizeof( clientsock_in ));

	char *bytes = "NXiaoPao\n";
	send(clientSocket, bytes, (int)strlen(bytes), 0);

	char receiveBuf[4];
	recv(clientSocket,receiveBuf,4,0);

	return receiveBuf;
	//std::cin >> receiveBuf;
	//cout << "Please enter "

}

void closeSocket()
{
	WSACleanup();
	closesocket(clientSocket);
}

void setupConnection( void )
{
	char *setup = setupSocket();
	char player[3];
	player[0] = setup[0];
	player[1] = setup[1];
	player[2] = '\0';
	if (!strcmp( player, "U1"))
	{
		//if I go first
		char receiveBuf[67];
		mypiece = BLACK_PIECE;

		cout << "I'm black" << endl;

		my_color = black_pea;
		enemy_color = white_pea;
		bw_trigger = 1;

		black_times++;

		for (int i = 0; i < ChessBoardSize* ChessBoardSize; i++)
			chessboard[i]= 0;

		chessboard[27]= 2;
		chessboard[28]= 1;
		chessboard[35]= 1;
		chessboard[36]= 2;

		drawChessboard();

		recv(clientSocket,receiveBuf,67,0);
	}
	else
	{
		//if I go second
		char receiveBuf[67];

		//wait = true;
		mypiece = WHITE_PIECE;

		my_color = white_pea;
		enemy_color = black_pea;

		bw_trigger = 2;

		white_times++;

		cout << "I'm white" << endl;

		recv(clientSocket,receiveBuf,67,0);
		//wait = false;

		for (int i = 0; i < 64; i++)
		{
			chessboard[i] = receiveBuf[i + 1] - '0';
		}
	}
}

void init()
{


	GLint loop = 9;

	//row lines
	for( int i= 0; i< loop; i++ ) {
		point_location.push_back( point4( 0.8, 0.8- 0.2*(float)i, 0.0, 1.0));
		point_location.push_back( point4( - 0.8, 0.8- 0.2*(float)i, 0.0, 1.0));
	}

	//column lines
	for( int i= 0; i< loop; i++ ) {
		point_location.push_back( point4( 0.8 - 0.2*(float)i, 0.8, 0.0, 1.0));
		point_location.push_back( point4( 0.8 - 0.2*(float)i, -0.8, 0.0, 1.0));
	}

	for (int i = 0; i < ChessBoardSize* ChessBoardSize; i++)
		chessboard[i]= 0;

	chessboard[27]= 2;
	chessboard[28]= 1;
	chessboard[35]= 1;
	chessboard[36]= 2;

	circle( piece, 32, 1.0, 0.0);
	piece[32] = point4(1.0, 0.0, 0.0, 1.0);

	points = &point_location[0];

	// Create and initialize a buffer object

	glGenBuffers( 3, buffer );
	glBindBuffer( GL_ARRAY_BUFFER, buffer[0] );
	glBufferData( GL_ARRAY_BUFFER, point_location.size()* sizeof(point4), points, GL_STATIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, buffer[2] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(marksqure), marksqure, GL_STATIC_DRAW );

	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
	glUseProgram( program );

	loc = glGetAttribLocation(program, "vPosition");
	color_loc = glGetUniformLocation(program, "vColor");
	model_view_loc = glGetUniformLocation(program, "modelview");
	Projection = glGetUniformLocation( program, "Projection" );

	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	cout << endl << endl << "---------Othello Game------------" << endl
		<< "Black Turn!" << endl;

	cout << "Select Player:" << endl;
	cout << "1.Human" << endl;
	cout << "2.Computer" << endl;

	char selection;

	cin >> selection;

	switch( selection )
	{
	case '1': Human = true;
		NetBattle = true;
		break;
	case '2': Human = false;
		NetBattle = true;
		break;
	}

	if ( NetBattle )
	{
		setupConnection();
		t_begin = clock(); //begin time stamp
	}

	glClearColor(0.39, 0.49, 0.36, 1.0);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	points = &point_location[0];

	ctmat = Perspective( fovy, aspect, zNear, zFar);

	location = LookAt( eye, eye - n, v);

	//Draw chessboard
	glUniformMatrix4fv(Projection, 1, GL_TRUE, ctmat );
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, location);
	glBindBuffer( GL_ARRAY_BUFFER, buffer[0] );
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glUniform4fv(color_loc, 1, red_frame);
	glDrawArrays(GL_LINES , 0, point_location.size());

	drawChessboard();

	if ( Human && !finish )
	{
		humanMove( mypiece );
		drawChessboard();
	}
	else
	{
		AIMove( mypiece );
		drawChessboard();
	}

	if ( NetBattle && newpiece)
	{
		char receiveBuf[67];

		recv(clientSocket,receiveBuf,67,0);
		// Start Timer
		t_begin = clock();
		firstTime = st.wMilliseconds;
		newpiece = false;

		if(receiveBuf[0] == 'W')
		{
			finish = true;
			if(receiveBuf[1] == '0')
				//enemy wins
					wins = !(mypiece - 1) + 1;
			if(receiveBuf[1] == '1')
			{
				//I win
				wins = mypiece;
				if ( mypiece == WHITE_PIECE )
					white_win_times++;
				else if ( mypiece == BLACK_PIECE )
					black_win_times++;
			}

			if(receiveBuf[1] == '2')
				wins = TIE;

			finish = true;

			displayWinner( wins );
			cout << "Total time: " << time_sum << " seconds"<< endl;
			cout << "Total Moves: " << numMoves << endl;
			cout << "Avg time per move: " << time_sum/numMoves << " seconds/move" <<endl;
			aver_time_sum += time_sum/numMoves;
			time_sum = 0;
			numMoves = 0;
		}
		else{
			//update chessboard
			for (int i = 0; i < 64; i++)
				chessboard[i] = receiveBuf[i + 1] - '0';
			bw_trigger = 1;
		}
	}
}

int get_Cell_Number( GLfloat x, GLfloat y)
{
	int row = ((int)(y*10+ ChessBoardSize) ) / 2;
	int column = ((int)(x*10+ ChessBoardSize) ) / 2;
	return (column* 8 + row);
}

int checkValidity( int index, int piece, bool set, int board[])
{
	int count = 0;
	int sum = 0;

	if( (index % 8 + 1) < ChessBoardSize )
		//check up
			if ( count = checkNeighbor( index, piece, 'u', set, board) )
				sum += count;

	if( (index % 8 - 1) >= 0)
		//check down
			if ( count = checkNeighbor( index, piece, 'd', set, board))
				sum += count;

	if( index / 8 > 0)
		//check left
			if ( count = checkNeighbor( index, piece, 'l', set, board))
				sum += count;

	if( index / 8 < 7)
		//check left
			if ( count = checkNeighbor( index, piece, 'r', set, board))
				sum += count;

	if( ((index % 8 + 1) < ChessBoardSize) && (index / 8 < 7) )
		//check upper right
			if ( count = checkNeighbor( index, piece, 'w', set, board))
				sum += count;

	if( ((index % 8 + 1) < ChessBoardSize) && (index / 8 > 0) )
		//check upper left
			if ( count = checkNeighbor( index, piece, 'q', set, board))
				sum += count;

	if( (index % 8 - 1) >= 0 && (index / 8 < 7) )
		//check lower right
			if ( count = checkNeighbor( index, piece, 's', set, board))
				sum += count;

	if( (index % 8 - 1) >= 0 && (index / 8 > 0) )
		//check lower left
			if ( count = checkNeighbor( index, piece, 'a', set, board))
				sum += count;

	return sum;
}

int checkNeighbor( int index, int piece, char direction, bool set, int board[])
{
	int count = 0;
	int oppoIndex = index;

	switch(direction)
	{
	case 'u': 
		while ( board[ oppoIndex + 1] != piece &&
			board[ oppoIndex + 1] != 0 &&
			( oppoIndex % 8 + 1) < ChessBoardSize )
		{
			count ++;	oppoIndex ++;
		}
		if( board[ oppoIndex + 1] == piece && count > 0 && 
			( oppoIndex % 8 + 1) != ChessBoardSize)
		{
			if (set)
				setNeighbor( piece, count, index, 1, board);

			return count;
		}
		else return 0;
		break;

	case 'd': 
		while ( board[ oppoIndex - 1] != piece &&
			board[ oppoIndex - 1] != 0 &&
			( oppoIndex % 8 - 1) > 0)
		{
			count ++;	oppoIndex --;
		}
		if( board[ oppoIndex - 1] == piece && count > 0 &&
			( oppoIndex % 8 ) != 0)
		{
			if (set)
				setNeighbor( piece, count, index, -1, board);
			return count;
		}
		else return 0;
		break;

	case 'l': 
		while ( board[ oppoIndex - 8] != piece &&
			board[ oppoIndex - 8] != 0 &&
			( oppoIndex / 8 ) > 0)
		{
			count ++;	oppoIndex -= 8;
		}
		if( board[ oppoIndex - 8] == piece && count > 0 && 
			oppoIndex / 8 != 0)
		{
			if (set)
				setNeighbor( piece, count, index, -8, board);

			return count;
		}
		else return 0;
		break;

	case 'r': 
		while ( board[ oppoIndex + 8] != piece &&
			board[ oppoIndex + 8] != 0 &&
			( oppoIndex / 8 ) < 7)
		{
			count ++;	oppoIndex += 8;
		}
		if( board[ oppoIndex + 8] == piece && count > 0 &&
			( oppoIndex / 8 ) != 7)
		{
			if (set)
				setNeighbor( piece, count, index, 8, board);

			return count;
		}
		else return 0;
		break;

	case 'w': 
		while ( board[ oppoIndex + 9] != piece &&
			board[ oppoIndex + 9] != 0 &&
			( oppoIndex / 8 ) < 7 &&
			( oppoIndex % 8 + 1) < ChessBoardSize)
		{
			count ++;	oppoIndex += 9;
		}
		if( board[ oppoIndex + 9] == piece && count > 0 && 
			( oppoIndex / 8 ) != 7 &&
			( oppoIndex % 8 + 1) != ChessBoardSize )
		{
			if (set)
				setNeighbor( piece, count, index, 9, board);

			return count;
		}
		else return 0;
		break;

	case 'q': 
		while ( board[ oppoIndex - 7] != piece &&
			board[ oppoIndex - 7] != 0 &&
			( oppoIndex / 8 ) > 0 &&
			( oppoIndex % 8 + 1) < ChessBoardSize)
		{
			count ++;	oppoIndex -= 7;
		}
		if( board[ oppoIndex - 7] == piece && count > 0 && 
			( oppoIndex / 8 ) != 0 &&
			( oppoIndex% 8 + 1) != ChessBoardSize)
		{
			if (set)
				setNeighbor( piece, count, index, -7, board);

			return count;
		}
		else return 0;
		break;

	case 's': 
		while ( board[ oppoIndex + 7] != piece &&
			board[ oppoIndex + 7] != 0 &&
			( oppoIndex / 8 ) < 7 &&
			( oppoIndex % 8 - 1) > 0)
		{
			count ++;	oppoIndex += 7;
		}
		if( board[ oppoIndex + 7] == piece && count > 0 &&
			( oppoIndex / 8 ) != 7 &&
			( oppoIndex % 8 ) != 0)
		{
			if (set)
				setNeighbor( piece, count, index, 7, board);

			return count;
		}
		else return 0;
		break;

	case 'a': 
		while ( board[ oppoIndex - 9] != piece &&
			board[ oppoIndex - 9] != 0 &&
			( oppoIndex / 8 ) > 0 &&
			( oppoIndex % 8 - 1) > 0)
		{
			count ++;	oppoIndex -= 9;
		}
		if( board[ oppoIndex - 9] == piece && count > 0 &&
			( oppoIndex / 8 ) != 0 &&
			( oppoIndex % 8 ) != 0)
		{
			if (set)
				setNeighbor( piece, count, index, -9, board);

			return count;
		}
		else return 0;
		break;

	default: return 0;
	}
}

int checkAvailableSpot( int piece, int board[], vector<int>& spots)
{
	spots.clear();

	int weight = 0;

	for (int i = 0; i < 64; i++)
	{
		if(board[i] == 0)
			if( weight = checkValidity( i, piece, CHECK_ONLY, board) )
				spots.push_back( i );
	}

	return spots.size();
}

void setNeighbor( int piece, int count, int index, int offset, int board[])
{
	for (int i = 1; i <= count; i++)
		board[ index + i * offset] = piece;
}

void keyboard (unsigned char key, int x, int y)
{
	switch( key ) {
	case 033:  // Escape key
	case 'q': case 'Q':
		exit( EXIT_SUCCESS );
		break;
	case 'r':
		if ( finish )
		{
			setupConnection();
		}
		break;
	}
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	aspect = GLfloat(w)/h;
	mat4  projection = Perspective( fovy, aspect, zNear, zFar );
	glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
	win_w = w;
	win_h = h;
}

void idle(void){
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	wx = 2.0*x/win_w - 1.0;
	wy = 1.0-2.0*y/win_h; 
	if ( state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		clicked = true;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Othello Client");

	fprintf(stdout, "OpenGL Version: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc( keyboard );
	glutMouseFunc(mouse);
	glutIdleFunc(idle);

	glutMainLoop();

	closeSocket();
	return 0;  
}

void drawChessboard()
{
	//Draw Chessman
	for (int i = 0; i < 64; i++)
	{
		if ( chessboard[i] != 0)
		{
			GLfloat column = (GLfloat)((int)(i/8)*2 - 8)/10.0;
			GLfloat row = (GLfloat)((i%8)*2 - 8)/10.0;

			mat4 location_2 = location*
				Translate( column + 0.1, row + 0.1, 0.0)* Scale( 0.08, 0.08, 0.0);

			glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, location_2);
			glBindBuffer( GL_ARRAY_BUFFER, buffer[1] );
			glBufferData( GL_ARRAY_BUFFER, sizeof(piece), piece, GL_STATIC_DRAW );
			glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

			if (chessboard[i] == 1)
				glUniform4fv(color_loc, 1, white_pea);
			else 
				glUniform4fv(color_loc, 1, black_pea);

			glLineWidth( 1.2 );
			glDrawArrays(GL_TRIANGLE_FAN , 0, sizeof( piece )+ 1);
		}
	}

	//Draw Turn Marker
	if ( bw_trigger == 1 )
	{
		mark_color = my_color;
	}
	else if ( bw_trigger == 2 )
	{
		mark_color = enemy_color;
	}
	mat4 position = location *Translate(0.0, 0.9, 0.0)*Scale(0.8, 0.05, 1.0);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, position);
	glUniform4fv( color_loc, 1, mark_color );
	glBindBuffer( GL_ARRAY_BUFFER, buffer[2] );
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glDrawArrays(GL_QUADS, 0, sizeof(marksqure));

	glFlush();
}

bool AIMove( int piece )
{
	if (checkAvailableSpot( piece , chessboard, validSpots) != 0)
	{
		//AI start
		clock_t start, end;

		int cellnumber = validSpots[0];

		boardStatusTree root( chessboard, validSpots, piece);

		start = clock();
		end = start;

		int iterativeDepth = 6;

		while ( end - start < 1500 && iterativeDepth <= SEARCH_DEPTH)
		{
			cellnumber = AlphaBetaPruning( iterativeDepth, iterativeDepth, -1000, 1000, root, WHITE_PIECE, false);
			end = clock();
			iterativeDepth ++;
		}

		chessboard[ cellnumber ] = piece;

		checkValidity( cellnumber, piece, CHECK_AND_SET, chessboard);

		sendToServer( cellnumber/8, cellnumber%8 );
		time_sum += (clock() - t_begin)/CLOCKS_PER_SEC;
		bw_trigger = 2;

		newpiece = true;
		numMoves++;
	}
	else
		return false;
}

float AlphaBetaPruning( int depth, int iterativeDepth, float alpha, float beta, boardStatusTree node, int piece, bool pass)
{
	//current best value, set to -1000
	float bestValue = MINIMUM;
	int preBestPosition = 0;
	int bestPosition = 0;

	if ( node.validSpots.size() != 0 )
		bestPosition = node.validSpots[0];	

	//meet the depth
	if( depth <= 0)
		return node.evaluate();

	for ( int i = 0; i < node.validSpots.size(); i++ )
	{
		node.child = new boardStatusTree( node.pieces, &node, !(piece - 1) + 1 );

		//assume the opponent make a move
		node.child -> fliped = checkValidity( node.validSpots[i], piece, CHECK_AND_SET, node.child->pieces);
		node.child -> pieces[ node.validSpots[i] ] = piece;
		node.child -> putPiece = node.validSpots[i];


		//check the rest available spots
		checkAvailableSpot( !(piece - 1) + 1, node.child->pieces, node.child->validSpots );

		//determine search depth according to the size of available spots
		int deepthOffset = node.child->validSpots.size() / 6 + 1;

		float value = -AlphaBetaPruning( depth - 1, iterativeDepth, -beta, -alpha, *(node.child), !(piece - 1) + 1, false);

		if( value >= beta ){
			delete node.child;
			return value;
		}

		if( value > bestValue ){
			bestValue = value;
			preBestPosition = bestPosition;
			bestPosition = node.validSpots[i];
			if(value > alpha)
				alpha = value;
		}

		delete node.child;
	}

	if ( bestValue == MINIMUM )
	{
		if ( pass )
		{
			//if game comes to an end
			int dif = node.getPieces( 1 ) - node.getPieces( 2 );
			return 10 * dif;
		}
		else
		{
			pass = true;
			//one pass

			node.child = new boardStatusTree( node.pieces, &node, !(piece - 1) + 1 );

			//check the rest available spots
			checkAvailableSpot( !(piece - 1) + 1, node.child->pieces, node.child->validSpots );

			bestValue = -AlphaBetaPruning( depth, iterativeDepth, -beta, -alpha, *(node.child), !(piece - 1) + 1, true);
			delete node.child;
		}
	}

	if (depth == iterativeDepth)
	{
		return bestPosition;
	}
	else
		return bestValue;
}

int stablePiece( int board[], int piece)
{
	int value = 0;
	unsigned char stable= 0;

	for (int i = 0; i < 64; i++)
	{
		if ( board[i] == piece )
		{
			stable = stability( board, piece, i);
			if ( stable )
			{
				////move every bit left until meet a '1' at the beginning
				//while( !(stable & 0x80) )
				//	stable = ( stable << 1 ) | (stable >> 7 );

				////move every bit left until meet a '0' at the beginning
				////or every bit of stable is 1
				//while( !( ~stable & 0x80) && stable != 0xff)
				//	stable = ( stable << 1 ) | (stable >> 7 );

				//if there'smore than 4 consecutive 1s in stable
				if( (stable & 0x0f) == 0x0f 
					|| (stable & 0x1e) == 0x1e
					|| (stable & 0x3c) == 0x3c
					|| (stable & 0x78) == 0x78
					|| (stable & 0xf0) == 0xf0
					)
					value += 1;
			}	
		}
	}

	return value;
}

unsigned char stability( int board[], int piece, int index)
{
	unsigned char stable = 0;
	bool flag = true;
	int position = index;
	int x = position / 8;
	int y = position % 8;


	//check up
	while ( y < ChessBoardSize )
	{
		if( board[ x*8 + y ] != piece ){
			flag = false;
			break;
		}
		y ++;
	}
	if( flag )
		stable = stable | 0x80;




	//check down
	position = index;
	flag = true;
	x = position / 8;
	y = position % 8;

	while (	y >= 0 )
	{
		if( board[ x*8 + y ] != piece ){
			flag = false;
			break;
		}
		y --;
	}
	if( flag )
		stable = stable | 0x08;





	//check left
	position = index;
	flag = true;
	x = position / 8;
	y = position % 8;

	while ( x >= 0 )
	{
		if( board[ x*8 + y ] != piece ){
			flag = false;
			break;
		}
		x --;
	}
	if( flag )
		stable = stable | 0x02;

	position = index;
	flag = true;
	x = position / 8;
	y = position % 8;





	//check right
	while ( x < ChessBoardSize )
	{
		if( board[ x*8 + y ] != piece ){
			flag = false;
			break;
		}
		x ++;
	}
	if( flag )
		stable = stable | 0x20;

	position = index;
	flag = true;
	x = position / 8;
	y = position % 8;





	//check w
	while (	x < ChessBoardSize && y < ChessBoardSize)
	{
		if( board[ x*8 + y ] != piece ){
			flag = false;
			break;
		}
		x ++;
		y ++;
	}
	if( flag )
		stable = stable | 0x40;

	position = index;
	flag = true;
	x = position / 8;
	y = position % 8;




	//check q
	while ( x >= 0 && y < ChessBoardSize)
	{
		if( board[ x*8 + y ] != piece ){
			flag = false;
			break;
		}
		x --;
		y ++;
	}
	if( flag )
		stable = stable | 0x01;





	//check s
	position = index;
	flag = true;
	x = position / 8;
	y = position % 8;

	while ( x < ChessBoardSize && y >= 0)
	{
		if( board[ x*8 + y ] != piece ){
			flag = false;
			break;
		}
		x ++;
		y --;
	}
	if( flag )
		stable = stable | 0x10;




	//check a
	position = index;
	flag = true;
	x = position / 8;
	y = position % 8;

	while ( x >= 0 && y >= 0)
	{
		if( board[ x*8 + y ] != piece ){
			flag = false;
			break;
		}
		x --;
		y --;
	}
	if( flag )
		stable = stable | 0x04;

	return stable;
}

void humanMove( int mypiece )
{
	if ( clicked )
	{
		int cellnumber = get_Cell_Number(wx, wy);
		if (chessboard[cellnumber] == 0 && checkValidity( cellnumber, mypiece, CHECK_AND_SET, chessboard))
		{
			int row = ((int)(wy*10+ ChessBoardSize) ) / 2;
			int column = ((int)(wx*10+ ChessBoardSize) ) / 2;

			chessboard[cellnumber] = mypiece;

			sendToServer(column, row);
			 
			newpiece = true;
		}
		else
			cout << "invalid move!" << endl;
		clicked = false;
	}
}

void displayWinner( int win )
{
	if( win == 0 )
		cout << "Tie game!" << endl;
	if( win == 1 )
		cout << "White win!" << endl;
	if( win == 2 )
		cout << "Black win!" << endl;
}