#pragma once
#include <ThreadJob.h>
#include <TinyRetail_Julius.h>
#include <stdio.h>

class TinyRetail_Julius;

class Julius_Thread : public CThreadJob{

public:
    Julius_Thread(Recog *tmp);
    ~Julius_Thread();

    //基底クラスの関数をオーバーライドする
    virtual unsigned int DoWork();

    volatile bool    fIamZombie;

private:

    Recog *m_recog;   //認識コアのインスタンス変数

};