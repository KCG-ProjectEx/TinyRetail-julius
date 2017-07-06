#pragma once
#include <pthread.h>

class CThreadJob
{
public:
	CThreadJob(void);
	~CThreadJob(void);

	bool 	Begin();		// スレッドを開始する
	void 	End();		// スレッドを停止・停止するまで待つ

	static void *ThreadFunc(void *pvoid);

	pthread_t    m_idThread;

protected:
	volatile bool	m_fStopFlag;

	virtual	unsigned int	DoWork(){return 0;};	// 実行するジョブでオーバライド
};
