#pragma once
#include <array>
using namespace std;

enum class BlockShape {
    I, O, T, L, J, Z, S
};  //7개의 블럭 모양을 enum class로 선언해보았다. 

class Block {
private:
    int shapeData[4][4][4];  //shapeData[회전][x][y] , 모양은 위 코드에서 정의했다. (enum class)
    BlockShape shape;  // 블럭 모양에 이름을 붙인 Blockshape(위 enum class) 객체 선언. -> 블럭 종류
    int angle = 0;              // 회전 상태 (0~3)
    int block_x;
    int block_y;

public:

    Block() = default; //기본 생성자

    // 생성자: 블럭 종류를 받아서 내부 배열을 초기화함
    Block(BlockShape s) : shape(s) {
        initShape();
    }

    BlockShape getShape() const {
        return shape;
    }

    int(&getRotation(int angle))[4][4]{
        return shapeData[angle % 4];
    }

    void copyShape(int src[4][4][4]);
    void initShape();

    //getter setter 함수 -> 인라인으로
    int getAngle() const { return angle; }
    int getX() const { return block_x; }
    int getY() const { return block_y; }

    void setAngle(int a) { angle = a % 4; }
    void setPosition(int nx, int ny) {
        block_x = nx;
        block_y = ny;
    }
};
