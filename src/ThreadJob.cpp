#include	<ThreadJob.h>

CThreadJob::CThreadJob(void)
{
    m_idThread	= 0;
    m_fStopFlag	= false;
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

bool CThreadJob::Begin()
{
	int		iHandle;
	bool    fRet;

	iHandle = pthread_create(&m_idThread, NULL, ThreadFunc, this);

	fRet = (iHandle != 0) ? false : true;
	return(fRet);
}

void CThreadJob::End()
{
	m_fStopFlag = true;
	pthread_join(m_idThread, NULL);
}


