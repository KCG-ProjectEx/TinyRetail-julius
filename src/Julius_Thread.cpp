#include <Julius_Thread.h>

CJulius_Thread::CJulius_Thread(Recog *tmp){

    fprintf(stdout,"Welcome to Julius_Thread\n");
    
    m_recog = tmp;
}

CJulius_Thread::~CJulius_Thread(){

    fprintf(stdout,"See you Julius_Thread\n");

}

unsigned int CJulius_Thread::DoWork(){

    fprintf(stdout,"START : julius_stream\n");
    fIamZombie = false; //ゾンビではない

    // juliusストリームを開始する
    // エラーか強制終了を行わない限り出てこない
    j_recognize_stream(m_recog);

    fprintf(stdout,"END : julius_stream\n");
    
    //エラーか強制終了した場合、fIamZombieはTRUEとなる
    fIamZombie = true;

    return 0;
}