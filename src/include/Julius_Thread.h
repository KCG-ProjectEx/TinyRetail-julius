#pragma once
#include <ThreadJob.h>
#include <TinyRetail_Julius.h>
#include <stdio.h>

class CTinyRetail_Julius;

class CJulius_Thread : public CThreadJob{

public:
    CJulius_Thread(Recog *tmp);
    ~CJulius_Thread();

    //基底クラスの関数をオーバーライドする
    virtual unsigned int DoWork();

    volatile bool    fIamZombie;

private:

    Recog *m_recog;   //認識コアのインスタンス変数

};