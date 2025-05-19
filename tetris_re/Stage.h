#pragma once

//각 스테이지마다의 난이도 설정
class Stage {
private:
    int speed;        //숫자가 낮을수록 속도가 빠르다
    int stickRate;    //막대가 나오는 확률 0~99 , 99면 막대기만 나옴
    int clearLine;

public:
    Stage(int s, int rate, int line) : speed(s), stickRate(rate), clearLine(line) {}


    // Getter 매서드
    int getSpeed() const { return speed; }
    int getStickRate() const { return stickRate; }
    int getClearLine() const { return clearLine; }

    // setter 매서드
    void setSpeed(int s) { speed = s; }
    void setStickRate(int r) { stickRate = r; }
    void setClearLine(int l) { clearLine = l; }
};

extern Stage stage_data[10];  //선언하고 정의 cpp파일에서 해야함