//**********************************************************************;
// Copyright (c) 2015, Intel Corporation
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//**********************************************************************;

//
// NOTE:  this file is only used when the TPM simulator is being used
// as the TPM device.  It is used in two places:  application SAPI (to
// communicate platform commands to the RM) and when RM needs
// to send platform commands to the simulator.
//

//
// NOTE:  uncomment following if you think you need to see all
// socket communications.
//
//#define DEBUG_SOCKETS

#include <stdio.h>
#include <stdlib.h>   // Needed for _wtoi

#include "tpm20.h"
#include "tpmsockets.h"
#include "tss2_sysapi_util.h"
#include "debug.h"
#include "tss2_tcti.h"
#include "tss2_tcti_util.h"


#ifdef  _WIN32
typedef HANDLE TPM_MUTEX;
#elif __linux || __unix
#include <semaphore.h>
typedef sem_t TPM_MUTEX;
#else
#error Unsupported OS--need to add OS-specific support for threading here.        
#endif                                    

#ifndef SAPI_CLIENT    
extern TPM_MUTEX tpmMutex;
#endif

TSS2_RC PlatformCommand(
    TSS2_TCTI_CONTEXT *tctiContext,     /* in */
    char cmd )
{
    int iResult = 0;            // used to return function results
    char sendbuf[] = { 0x0,0x0,0x0,0x0 };
    char recvbuf[] = { 0x0, 0x0, 0x0, 0x0 };
	TSS2_RC rval = TSS2_RC_SUCCESS;
    
#ifndef SAPI_CLIENT
    UINT8 mutexAcquired = 1;
#ifdef  _WIN32
    DWORD mutexWaitRetVal;
#elif __linux || __unix
    int mutexWaitRetVal;
    struct timespec semWait = { 2, 0 };
#else
#error Unsupported OS--need to add OS-specific support for threading here.        
#endif             
#endif

    if( simulator )
    {
        sendbuf[3] = cmd;

        OpenOutFile( &outFp );

#ifndef SAPI_CLIENT    
        if( cmd != MS_SIM_CANCEL_ON && cmd != MS_SIM_CANCEL_OFF &&
            cmd != MS_SIM_POWER_ON && cmd != MS_SIM_POWER_OFF )
        {
            // Critical section starts here
#ifdef  _WIN32
            mutexWaitRetVal = WaitForSingleObject( tpmMutex, 2000 );

            if( mutexWaitRetVal != WAIT_OBJECT_0 )
            {
#ifdef DEBUG_MUTEX                
                (*printfFunction)(NO_PREFIX, "In PlatformCommand, failed to acquire mutex error: %d\n", mutexWaitRetVal );
#endif            
                rval = TSS2_TCTI_RC_TRY_AGAIN;
            }
#elif __linux || __unix
            mutexWaitRetVal = sem_timedwait( &tpmMutex, &semWait );
            if( mutexWaitRetVal != 0 )
            {
#ifdef DEBUG_MUTEX                
                (*printfFunction)(NO_PREFIX, "In PlatformCommand, failed to acquire mutex error: %d\n", errno );
#endif            
                rval = TSS2_TCTI_RC_TRY_AGAIN;
            }
#else    
#error Unsupported OS--need to add OS-specific support for threading here.        
#endif         
            else
            {  
#ifdef DEBUG_MUTEX                
                (*printfFunction)(NO_PREFIX, "In PlatformCommand, acquired mutex\n" );
#endif                     
                mutexAcquired = 1;
            }
        }
        else
        {
            mutexAcquired = 1;
        }

        if( mutexAcquired )
#endif
        {
            // Send the command
            iResult = send( TCTI_CONTEXT_INTEL->otherSock, sendbuf, 4, 0 );

            if (iResult == SOCKET_ERROR) {
                (*printfFunction)(NO_PREFIX, "In PlatformCommand, send failed with error: %d\n", WSAGetLastError() );
                rval = TSS2_TCTI_RC_IO_ERROR;
            }
            else
            {
#ifdef DEBUG_SOCKETS
                (*printfFunction)( rmDebugPrefix, "In PlatformCommand, send Bytes to socket #0x%x: \n", TCTI_CONTEXT_INTEL->otherSock );
                DebugPrintBuffer( (UINT8 *)sendbuf, 4 );
#endif

                // Read result
                iResult = recv( TCTI_CONTEXT_INTEL->otherSock, recvbuf, 4, 0);
                if (iResult == SOCKET_ERROR) {
                    (*printfFunction)(NO_PREFIX, "In PlatformCommand, recv failed (socket: 0x%x) with error: %d\n",
                            TCTI_CONTEXT_INTEL->otherSock, WSAGetLastError() );
                    rval = TSS2_TCTI_RC_IO_ERROR;
                }
                else if( recvbuf[0] != 0 || recvbuf[1] != 0 || recvbuf[2] != 0 || recvbuf[3] != 0 )
                {
                    (*printfFunction)(NO_PREFIX, "PlatformCommand failed with error: %d\n", recvbuf[3] );
                    rval = TSS2_TCTI_RC_IO_ERROR;
                }
                else
                {
#ifdef DEBUG_SOCKETS
                    (*printfFunction)(NO_PREFIX, "In PlatformCommand, receive bytes from socket #0x%x: \n", TCTI_CONTEXT_INTEL->otherSock );
                    DebugPrintBuffer( (UINT8 *)recvbuf, 4 );
#endif
                }
            }
        }

#ifndef SAPI_CLIENT    
        if( cmd != MS_SIM_CANCEL_ON && cmd != MS_SIM_CANCEL_OFF &&
            cmd != MS_SIM_POWER_ON && cmd != MS_SIM_POWER_OFF )
        {
        // Critical section ends here
#ifdef  _WIN32
            if( 0 == ReleaseMutex( tpmMutex ) )
            {
#ifdef DEBUG_MUTEX                
                (*printfFunction)(NO_PREFIX, "In PlatformCommand, failed to release mutex error: %d\n", GetLastError() );
#endif            
                rval = TSS2_TCTI_RC_TRY_AGAIN;
            }
#elif __linux || __unix
            if( 0 != sem_post( &tpmMutex ) )
            {
#ifdef DEBUG_MUTEX                
                (*printfFunction)(NO_PREFIX, "In PlatformCommand, failed to release mutex error: %d\n", errno );
#endif            
                rval = TSS2_TCTI_RC_TRY_AGAIN;
            }
#else    
#error Unsupported OS--need to add OS-specific support for threading here.        
#endif
#ifdef DEBUG_MUTEX                
            (*printfFunction)(NO_PREFIX, "In PlatformCommand, released mutex\n" );
#endif
        }
#endif 
        CloseOutFile( &outFp );
    }
    return rval;
} 
