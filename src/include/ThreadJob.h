#ifndef _ThreadJob_h_20160613
#define _ThreadJob_h_20160613

#include	<stdThread.h>

class CThreadJob
{
public:
	CThreadJob(void);
	~CThreadJob(void);

	BOOL 	Begin();		// スレッドを開始する
	void 	End();		// スレッドを停止・停止するまで待つ

	static void *ThreadFunc(void *pvoid);

	pthread_t    m_idThread;

protected:
	volatile BOOL	m_fStopFlag;

	virtual UINT 	DoWork(){return 0;};	// 実行するジョブでオーバライド
};

#endif
