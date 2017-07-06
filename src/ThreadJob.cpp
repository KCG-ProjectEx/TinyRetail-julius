#include	<ThreadJob.h>

CThreadJob::CThreadJob(void)
{
    m_idThread	= 0;
    m_fStopFlag	= FALSE;
}

CThreadJob::~CThreadJob(void)
{
}

void *CThreadJob::ThreadFunc(void *pvoid)
{
	CThreadJob  *pCThreadJob = (CThreadJob *)pvoid;
	pCThreadJob->DoWork();
	pthread_exit(NULL);
}

BOOL CThreadJob::Begin()
{
	int		iHandle;
	BOOL    fRet;

	iHandle = pthread_create(&m_idThread, NULL, ThreadFunc, this);

	fRet = (iHandle != 0) ? FALSE : TRUE;
	return(fRet);
}

void CThreadJob::End()
{
	m_fStopFlag = TRUE;
	pthread_join(m_idThread, NULL);
}


