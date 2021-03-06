/*
 *  FileEyeTVSocket.mm
 *  Plex
 *
 *  Created by Ryan Walklin on 2/28/09.
 *  Copyright 2009 Ryan Walklin. All rights reserved.
 *  
 *  Based on VLC's EyeTV integration module.
 *
 */

#import <Cocoa/Cocoa.h>

#include "FileEyeTVSocket.h"
#import "log.h"
#import <sys/un.h>


namespace XFILE
{
	
CFileEyeTVSocket::CFileEyeTVSocket()
{

}

CFileEyeTVSocket::~CFileEyeTVSocket()
{
	Close();
}
	
	
void CFileEyeTVSocket::EyeTVSocketCallback(CFSocketRef s, 
								CFSocketCallBackType    type, 
								CFDataRef               address, 
								const void *            data, 
								void *                  info)
{
#pragma unused(address)

    assert(s != NULL);
	
	CFileEyeTVSocket *socketObject = (CFileEyeTVSocket *)info;
	assert(socketObject != NULL);
    
    switch (type) {
        case kCFSocketDataCallBack:
		
			CFDataRef       newData;
			newData = (CFDataRef) data;
			assert(newData != NULL);
			assert(CFGetTypeID(newData) == CFDataGetTypeID());
			
			if ( CFDataGetLength(newData) == 0 )
			{
				// A zero length data indicates the end of the data stream; the client is dead 
				// so we just go and remove our record of it.
				CLog::Log(LOGDEBUG, "EOF from EyeTV socket");
				
				socketObject->Close();
			}
			else 
			{
				// Append the new data to whatever data we have already buffered 
				PaUtil_WriteRingBuffer(socketObject->streamBuffer, CFDataGetBytePtr(newData), CFDataGetLength(newData));
			}

            break;
        default:
            assert(false);
            break;
	}
}

	

//*********************************************************************************************
bool CFileEyeTVSocket::Open(const CURL& url, bool bBinary)
{
	
    struct sockaddr_un publicAddr, peerAddr;
    int publicSock, eyetvSock;
	
	initialised = false;
	
    //selectChannel( p_this, val );
	
    /* socket */
    memset(&publicAddr, 0, sizeof(publicAddr));
    publicAddr.sun_family = AF_UNIX;
    strncpy(publicAddr.sun_path, "/tmp/.plextvserver-bridge", sizeof(publicAddr.sun_path)-1);
	
    /* remove previous public path if it wasn't cleanly removed */
    if( (0 != unlink(publicAddr.sun_path)) && (ENOENT != errno) )
    {
		CLog::Log(LOGERROR, "local socket path is not usable (errno=%d)", errno);
        return false;
    }
	
    publicSock = socket(AF_UNIX, SOCK_STREAM, 0);
    if( publicSock == -1 )
    {
		CLog::Log(LOGERROR, "create local socket failed (errno=%d)", errno);
        return false;
    }
	
	// increase buffer size
	int oldSockBufLen, oldSockBufLenSize, newSockBufLen;
	if (getsockopt(publicSock, SOL_SOCKET, SO_RCVBUF, &oldSockBufLen, (socklen_t *)&oldSockBufLenSize) != 0)
	{
		perror(strerror(errno));
		return false;
	}
	newSockBufLen = 1024768;
	if (setsockopt(publicSock, SOL_SOCKET, SO_RCVBUF, &newSockBufLen, sizeof(int32_t)) != 0)
	{
		perror(strerror(errno));
		return false; 
	}
	
    if( bind(publicSock, (struct sockaddr *)&publicAddr, sizeof(struct sockaddr_un)) == -1 )
    {
		CLog::Log(LOGERROR, "bind local socket failed (errno=%d)", errno);
        close(publicSock);
        return false;
    }
	
    /* we are not expecting more than one connection */
    if( listen(publicSock, 1) == -1 )
    {
		CLog::Log(LOGERROR, "cannot accept connection (errno=%d)", errno);
        close( publicSock );
        return false;
    }
    else
    {
		
        socklen_t peerSockLen = sizeof(struct sockaddr_un);
        int peerSock;
		
		// initialise the receiver buffer
		uint32_t streamBufferSize = 1;
		while(streamBufferSize <= 1024768 * 10) // ensure power of 2, buffer 10MB
		{
			streamBufferSize <<= 1;
		}
		streamBuffer = (PaUtilRingBuffer *)malloc(sizeof(PaUtilRingBuffer));
		streamBufferData = malloc(streamBufferSize);
		
		PaUtil_InitializeRingBuffer(streamBuffer, 1, streamBufferSize, streamBufferData);
		
        /* tell the EyeTV plugin to open start sending */
        CFNotificationCenterPostNotification( CFNotificationCenterGetDistributedCenter (),
											 CFSTR("PlexTVServerStartDataSending"),
											 CFSTR("PlexTVServer"),
											 /*userInfo*/ NULL,
											 TRUE );
		
		CLog::Log(LOGINFO, "EyeTV client socket announced");
		
        peerSock = accept(publicSock, (struct sockaddr *)&peerAddr, &peerSockLen);
        if( peerSock == -1 )
        {
			CLog::Log(LOGERROR, "cannot wait for connection (errno=%d)", errno);
            close( publicSock );
            return false;
        }
		
		CLog::Log(LOGINFO, "EyeTV server connected" );
		initialised = true;
		
        eyetvSock = peerSock;
		
        /* remove public access */
        close(publicSock);
        unlink(publicAddr.sun_path);
    }
	
	if (initialised)
	{
		// Attach to CFSocket
		CFSocketContext context;
        
        memset(&context, 0, sizeof(context));
        context.info = this;
		
		
		eyetvSockRef = CFSocketCreateWithNative(kCFAllocatorDefault,
												eyetvSock, 
												kCFSocketDataCallBack,
												EyeTVSocketCallback, 
												&context);
		
		// Link to runloop
		
	}
	
	return initialised;
}



unsigned int CFileEyeTVSocket::Read(void *lpBuf, __int64 uiBufSize)
{
	if (initialised)
	{
		uint64_t len;
		
		/* Read data */
		//len = read(eyetvSock, lpBuf, uiBufSize);
		len = PaUtil_ReadRingBuffer(streamBuffer, lpBuf, uiBufSize);

		if (len < uiBufSize)
		{
			CLog::Log(LOGERROR, "short read from EyeTV server socket (%i KB of %li KB requested)", len / 1024, uiBufSize / 1024);
		}
    	
		return len;
	}
	return 0;
}

void CFileEyeTVSocket::Close()
{
	
	CLog::Log(LOGINFO, "closing EyeTV socket");
	
    /* tell the EyeTV plugin to close its msg port and stop sending */
    CFNotificationCenterPostNotification( CFNotificationCenterGetDistributedCenter (),
										 CFSTR("PlexTVServerStopDataSending"),
										 CFSTR("PlexTVServer"),
										 /*userInfo*/ NULL,
										 TRUE );
	
	CLog::Log(LOGINFO, "EyeTV server notified of shutdown" );
	
	CFSocketInvalidate(eyetvSockRef);
	
	CLog::Log(LOGINFO, "EyeTV socket closed and freed" );

	initialised = false;

}
	
}
