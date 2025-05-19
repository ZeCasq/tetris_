#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <Windows.h>
#include <time.h>
#include <iostream>
#include <string>
#include "Block.h"
#include "Stage.h"

//*********************************
//상수 선언
//*********************************

#define EXT_KEY			0xffffffe0	//확장키 인식값
#define KEY_LEFT		0x4b
#define KEY_RIGHT		0x4d
#define KEY_UP			0x48
#define KEY_DOWN		0x50


//stage 구조체 제거

//c++스타일로 고침
enum class Color : int {
	Black = 0,       // 까망
	DarkBlue,        // 어두운 파랑
	DarkGreen,       // 어두운 초록
	DarkSkyBlue,     // 어두운 하늘
	DarkRed,         // 어두운 빨강
	DarkViolet,      // 어두운 보라
	DarkYellow,      // 어두운 노랑
	Gray,            // 회색
	DarkGray,        // 어두운 회색
	Blue,            // 파랑
	Green,           // 초록
	SkyBlue,         // 하늘
	Red,             // 빨강
	Violet,          // 보라
	Yellow,          // 노랑
	White            // 하양
};

//Color c = Color::Red;
//
//if (c == Color::Red) {
//	// 빨강일 때 실행
//}

//*********************************
//전역변수선언
//*********************************
int level;
int ab_x, ab_y;	//화면중 블럭이 나타나는 좌표의 절대위치
//int block_shape, block_angle, block_x, block_y;

int next_block_shape;
int score;
int lines;
char total_block[21][14];		//화면에 표시되는 블럭들

//*********************************
//함수 선언
//*********************************
int gotoxy(int x, int y);	//커서옮기기..0
void SetColor(Color color);	//색표현..0
int init();					//각종변수 초기화
int show_cur_block(Block& block);	//진행중인 블럭을 화면에 표시한다..0
void show_cur_block(BlockShape shape, int angle, int x, int y); //함수 오버로드..0
int erase_cur_block(Block& block);	//블럭 진행의 잔상을 지우기 위한 함수..0
int show_total_block();	//쌓여져있는 블럭을 화면에 표시한다..0
int show_next_block(BlockShape shape);
BlockShape make_new_block();	//return값으로 block의 모양번호를 알려줌
int strike_check(Block& block);	//블럭이 화면 맨 아래에 부닥쳤는지 검사 부닥치면 1을리턴 아니면 0리턴
int merge_block(Block& block);	//블럭이 바닥에 닿았을때 진행중인 블럭과 쌓아진 블럭을 합침
void block_start(BlockShape shape, Block& block);	//블럭이 처음 나올때 위치와 모양을 알려줌..0
int move_block(Block& block, BlockShape& nextShape);	//게임오버는 1을리턴 바닥에 블럭이 닿으면 2를 리턴
int rotate_block(Block& block);
int show_gameover();
int show_gamestart();
int show_logo();
int input_data();
int check_full_line();


int main(int argc, char* argv[])
{
	srand(static_cast<unsigned>(time(nullptr)));
	init();
	show_logo();

	while (true)
	{
		input_data();
		show_total_block();

		BlockShape nextShape = static_cast<BlockShape>(make_new_block());
		BlockShape shape = static_cast<BlockShape>(make_new_block());

		Block currentBlock;
		block_start(shape, currentBlock);

		show_next_block(nextShape);
		show_gamestart();

		int is_gameover = 0;

		for (int i = 1; true; ++i)
		{
			if (_kbhit())
			{
				char key = _getche();
				if (key == EXT_KEY)
				{
					key = _getche();
					switch (key)
					{
					case KEY_UP:
						rotate_block(currentBlock);
						break;
					case KEY_LEFT:
						if (currentBlock.getX() > 1)
						{
							erase_cur_block(currentBlock);
							currentBlock.setPosition(currentBlock.getX() - 1, currentBlock.getY());
							if (strike_check(currentBlock))
								currentBlock.setPosition(currentBlock.getX() + 1, currentBlock.getY());
							show_cur_block(currentBlock);
						}
						break;
					case KEY_RIGHT:
						if (currentBlock.getX() < 14)
						{
							erase_cur_block(currentBlock);
							currentBlock.setPosition(currentBlock.getX() + 1, currentBlock.getY());
							if (strike_check(currentBlock))
								currentBlock.setPosition(currentBlock.getX() - 1, currentBlock.getY());
							show_cur_block(currentBlock);
						}
						break;
					case KEY_DOWN:
						is_gameover = move_block(currentBlock, nextShape);
						show_cur_block(currentBlock);
						break;
					}
				}
				else if (key == 32)  // space bar
				{
					while (is_gameover == 0)
						is_gameover = move_block(currentBlock, nextShape);
					show_cur_block(currentBlock);
				}
			}

			if (i % stage_data[level].getSpeed() == 0)
			{
				is_gameover = move_block(currentBlock, nextShape);
				show_cur_block(currentBlock);
			}

			if (stage_data[level].getClearLine() <= lines)
			{
				level++;
				lines = 0;
				show_gamestart();
			}

			if (level == 10)
			{
				system("cls");
				SMALL_RECT windowSize = { 0, 0, 59, 19 };
				COORD bufferSize = { 60, 20 };
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
				SetConsoleScreenBufferSize(hConsole, bufferSize);

				SetColor(Color::Green);
				gotoxy((60 - 30) / 2, 10);
				printf("모든 레벨을 클리어했습니다! 축하합니다!");
				gotoxy((60 - 10) / 2, 12);
				system("pause");
				exit(0);
			}

			if (is_gameover == 1)
			{
				show_gameover();
				is_gameover = 0;
				SetColor(Color::Gray);
				break;
			}

			gotoxy(77, 23);
			Sleep(15);
			gotoxy(77, 23);
		}

		init();
	}

	return 0;
}



//UI 함수/////////////////////////////////////////////////////
int gotoxy(int x, int y)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.Y = y;
	pos.X = x;
	SetConsoleCursorPosition(hConsole, pos);
	return 0;
}

void SetColor(Color color)
{
	static HANDLE std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(std_output_handle, static_cast<int>(color));
}
//////////////////////////////////////////////////////////////

int init()
{
	int i, j;

	srand((unsigned)time(NULL));


	for (i = 0; i < 20; i++)
	{
		for (j = 0; j < 14; j++)
		{
			if ((j == 0) || (j == 13))
			{
				total_block[i][j] = 1;
			}
			else {
				total_block[i][j] = 0;
			}
		}
	}

	for (j = 0; j < 14; j++)			//화면의 제일 밑의 줄은 1로 채운다.
		total_block[20][j] = 1;

	//전역변수 초기화
	level = 0;
	lines = 0;
	ab_x = 5;
	ab_y = 1;

	return 0;
}

int show_cur_block(Block& block)
{
	// 1. 색상 지정
	switch (block.getShape())
	{
	case BlockShape::I: SetColor(Color::Red);     break;
	case BlockShape::J: SetColor(Color::Blue);    break;
	case BlockShape::Z: SetColor(Color::SkyBlue); break;
	case BlockShape::O: SetColor(Color::White);   break;
	case BlockShape::S: SetColor(Color::Yellow);  break;
	case BlockShape::T: SetColor(Color::Violet);  break;
	case BlockShape::L: SetColor(Color::Green);   break;
	default:            SetColor(Color::Black);   break;
	}

	int(&shape)[4][4] = block.getRotation(block.getAngle());
	int x = block.getX();
	int y = block.getY();

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if ((j + y) < 0) continue;

			int(&shape)[4][4] = block.getRotation(block.getAngle());
			// block.getRotation(angle)이 반환한 int[4][4] 배열을 shape 에 받음

			if (shape[j][i] == 1) {
				gotoxy((i + x) * 2 + ab_x, j + y + ab_y);
				printf("■");
			}  //1이면 콘솔에 출력한다.
		}
	}

	SetColor(Color::Black);
	gotoxy(77, 23);
	return 0;
}

void show_cur_block(BlockShape shape, int angle, int x, int y)
{
	Block temp(shape);
	temp.setAngle(angle);
	temp.setPosition(x, y);
	show_cur_block(temp);
}


int erase_cur_block(Block& block)
{
	int(&shape)[4][4] = block.getRotation(block.getAngle());
	int x = block.getX();
	int y = block.getY();

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (shape[j][i] == 1)
			{
				gotoxy((i + x) * 2 + ab_x, j + y + ab_y);
				printf("  ");  // 출력된 블럭을 덮어쓰기
			}
		}
	}

	return 0;
}


int show_total_block()
{
	SetColor(Color::DarkGray);

	for (int i = 0; i < 21; i++)
	{
		for (int j = 0; j < 14; j++)
		{
			if (j == 0 || j == 13 || i == 20)
			{
				int colorIndex = (level % 6) + 1;
				SetColor(static_cast<Color>(colorIndex));
			}
			else
			{
				SetColor(Color::DarkGray);
			}

			gotoxy((j * 2) + ab_x, i + ab_y);

			if (total_block[i][j] == 1)
			{
				printf("■");
			}
			else
			{
				printf("  ");
			}
		}
	}

	SetColor(Color::Black);
	gotoxy(77, 23);
	return 0;
}


BlockShape make_new_block()
{
	int i = rand() % 100;
	if (i <= stage_data[level].getStickRate())
		return BlockShape::I;

	int shape = (rand() % 6) + 1;  // 1~6
	BlockShape result = static_cast<BlockShape>(shape);

	show_next_block(result);
	return result;
}


int strike_check(Block& block)
{
	int(&shape)[4][4] = block.getRotation(block.getAngle());
	int x = block.getX();
	int y = block.getY();

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (shape[i][j] == 0)
				continue;

			if ((y + i) < 0)
				continue;

			//왼쪽이나 오른쪽 벽에 충돌한 경우
			if ((x + j) <= 0 || (x + j) >= 13)
				return 1;

			//바닥이나 다른 블럭과 충돌한 경우
			if (total_block[y + i][x + j] == 1)
				return 1;
		}
	}

	return 0;  //충돌 없이 무사히,,
}

int merge_block(Block& block)
{
	int(&shape)[4][4] = block.getRotation(block.getAngle());
	int x = block.getX();
	int y = block.getY();

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			total_block[y + i][x + j] |= shape[i][j];
		}
	}

	check_full_line();     //한 줄 클리어
	show_total_block();

	return 0;
}


void block_start(BlockShape shape, Block& block)
{
	block = Block(shape);             // 블럭 형태 초기화
	block.setPosition(5, -3);         // 시작 위치
	block.setAngle(0);                // 초기 회전
}


int show_gameover()
{
	SetColor(Color::Red);
	gotoxy(15, 8);
	printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	gotoxy(15, 9);
	printf("┃**************************┃");
	gotoxy(15, 10);
	printf("┃*        GAME OVER       *┃");
	gotoxy(15, 11);
	printf("┃**************************┃");
	gotoxy(15, 12);
	printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
	fflush(stdin);
	Sleep(1000);

	_getche();
	system("cls");
	score = 0;
	return 0;
}

int move_block(Block& block, BlockShape& nextShape)
{
	erase_cur_block(block);

	// 1. 아래로 한 칸 이동
	block.setPosition(block.getX(), block.getY() + 1);

	// 2. 충돌 확인
	if (strike_check(block))
	{
		// 게임 오버 조건 (블럭이 초기 y<0에서 바로 충돌)
		if (block.getY() < 0)
			return 1;

		// 충돌났으니 다시 한 칸 올리고, 블럭 고정
		block.setPosition(block.getX(), block.getY() - 1);
		merge_block(block);

		// 새로운 블럭으로 교체
		BlockShape currentShape = nextShape;
		nextShape = make_new_block();

		// 새 블럭 초기화
		block_start(currentShape, block);  // or block.reset(currentShape);
		show_next_block(nextShape);

		return 2;  // 블럭 고정됨
	}

	// 3. 충돌 없이 한 칸 내려감
	return 0;
}


int rotate_block(Block& block)
{
	// 1. 회전된 상태의 임시 블럭을 만들어 충돌 검사
	Block testBlock = block;  // 복사 생성자
	testBlock.setAngle((block.getAngle() + 1) % 4);

	if (strike_check(testBlock) == 0)
	{
		// 2. 충돌이 없다면 회전 적용
		erase_cur_block(block);
		block.setAngle((block.getAngle() + 1) % 4);
		show_cur_block(block);
	}

	return 0;
}

int check_full_line()
{
	int i, j, k;
	for (i = 0; i < 20; i++)
	{
		for (j = 1; j < 13; j++)
		{
			if (total_block[i][j] == 0)
				break;
		}
		if (j == 13)	//한줄이 다 채워졌음
		{
			lines++;
			show_total_block();
			SetColor(Color::Blue);
			gotoxy(1 * 2 + ab_x, i + ab_y);
			for (j = 1; j < 13; j++)
			{
				printf("□");
				Sleep(10);
			}
			gotoxy(1 * 2 + ab_x, i + ab_y);
			for (j = 1; j < 13; j++)
			{
				printf("  ");
				Sleep(10);
			}

			for (k = i; k > 0; k--)
			{
				for (j = 1; j < 13; j++)
					total_block[k][j] = total_block[k - 1][j];
			}
			for (j = 1; j < 13; j++)
				total_block[0][j] = 0;
			score += 100 + (level * 10) + (rand() % 10);
			show_gamestart();
		}
	}
	return 0;
}

int show_next_block(BlockShape shape)
{
	// 1. 박스 그리기
	SetColor(static_cast<Color>((level + 1) % 6 + 1));
	for (int i = 1; i < 7; i++)
	{
		gotoxy(33, i);
		for (int j = 0; j < 10; j++)
		{
			if (i == 1 || i == 6 || j == 0 || j == 5)
				printf("■");
			else
				printf("  ");
		}
	}

	Block preview(shape);
	preview.setAngle(0);
	preview.setPosition(15, 1);
	show_cur_block(preview);

	return 0;
}


int show_gamestart()
{
	static bool printedText = false;
	SetColor(Color::Gray);

	if (!printedText)
	{
		gotoxy(35, 7);  printf("STAGE");
		gotoxy(35, 9);  printf("SCORE");
		gotoxy(35, 12); printf("LINES");
		printedText = true;
	}

	gotoxy(41, 7);  printf("%d", level + 1);
	gotoxy(35, 10); printf("%10d", score);
	gotoxy(35, 13); printf("%10d", stage_data[level].getClearLine() - lines);

	return 0;
}


int input_data()
{
	int selectedLevel = 0;
	std::string input;

	while (true)
	{
		system("cls");

		SetColor(Color::Gray);
		gotoxy(10, 7);  printf("┏━━━━━━━━━<GAME KEY>━━━━━━━━━┓");
		Sleep(10); gotoxy(10, 8);  printf("┃ UP   : Rotate Block        ┃");
		Sleep(10); gotoxy(10, 9);  printf("┃ DOWN : Move One-Step Down  ┃");
		Sleep(10); gotoxy(10, 10); printf("┃ SPACE: Move Bottom Down    ┃");
		Sleep(10); gotoxy(10, 11); printf("┃ LEFT : Move Left           ┃");
		Sleep(10); gotoxy(10, 12); printf("┃ RIGHT: Move Right          ┃");
		Sleep(10); gotoxy(10, 13); printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");

		gotoxy(10, 3);
		printf("Select Start level [1-8]:       \b\b\b\b\b\b\b");

		getline(cin, input);

		// 공백 제거 및 유효성 검사
		if (input.length() != 1 || !isdigit(input[0]))
			continue;

		selectedLevel = input[0] - '0';  // '1'~'8' → 1~8

		if (selectedLevel >= 1 && selectedLevel <= 8)
			break;
	}

	system("cls");
	level = selectedLevel - 1;  // 내부에서는 0-based로 저장
	return 0;
}


int show_logo()
{
	srand((unsigned)time(NULL));
	int i, j;
	gotoxy(13, 3);  //전각문자는 2칸으로 인식하는 것을 감안해서 오류를 수정함..tetris 글자 깨진거 오류 수정 완료
	printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	Sleep(100);
	gotoxy(13, 4);
	printf("┃     ■■■   ■■■   ■■■    ■■      ■    ■■     ┃");
	Sleep(100);
	gotoxy(13, 5);
	printf("┃      ■    ■      ■     ■ ■     ■   ■       ┃");
	Sleep(100);
	gotoxy(13, 6);
	printf("┃      ■    ■■■    ■     ■■      ■    ■      ┃");
	Sleep(100);
	gotoxy(13, 7);
	printf("┃      ■    ■      ■     ■ ■     ■     ■     ┃");
	Sleep(100);
	gotoxy(13, 8);
	printf("┃      ■    ■■■    ■     ■  ■    ■   ■■      ┃");
	Sleep(100);
	gotoxy(13, 9);
	printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");

	gotoxy(28, 20);
	printf("Please Press Any Key~!");

	for (i = 0; i >= 0; i++) {
		if (i % 40 == 0)
		{


			for (j = 0; j < 5; j++)
			{
				gotoxy(18, 14 + j);
				printf("                                                          ");


			}
			show_cur_block(static_cast<BlockShape>(rand() % 7), rand() % 4, 7, 14);
			show_cur_block(static_cast<BlockShape>(rand() % 7), rand() % 4, 12, 14);
			show_cur_block(static_cast<BlockShape>(rand() % 7), rand() % 4, 19, 14);
			show_cur_block(static_cast<BlockShape>(rand() % 7), rand() % 4, 24, 14);
		}
		if (_kbhit())
			break;
		Sleep(30);
	}

	_getche();
	system("cls");

	return 0;
}