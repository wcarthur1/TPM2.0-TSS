/***********************************************************************;
 * Copyright (c) 2015, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 ***********************************************************************/

#include <tpm20.h>   
#include <tss2_sysapi_util.h>   

TPM_RC Tss2_Sys_CreatePrimary_Prepare(
    TSS2_SYS_CONTEXT *sysContext,
    TPMI_RH_HIERARCHY	primaryHandle,
    TPM2B_SENSITIVE_CREATE	*inSensitive,
    TPM2B_PUBLIC	*inPublic,
    TPM2B_DATA	*outsideInfo,
    TPML_PCR_SELECTION	*creationPCR
    )
{
    if( sysContext == NULL )
    {
        return( TSS2_SYS_RC_BAD_REFERENCE );
    }

    if( creationPCR == NULL  )
	{
		return TSS2_SYS_RC_BAD_REFERENCE;
	} 

    CommonPreparePrologue( sysContext, TPM_CC_CreatePrimary );

    Marshal_UINT32( SYS_CONTEXT->tpmInBuffPtr, SYS_CONTEXT->maxCommandSize, &(SYS_CONTEXT->nextData), primaryHandle, &(SYS_CONTEXT->rval) );

    if( inSensitive == 0 )
	{
		SYS_CONTEXT->decryptNull = 1;
	}
            
    Marshal_TPM2B_SENSITIVE_CREATE( sysContext, inSensitive );

    Marshal_TPM2B_PUBLIC( sysContext, inPublic );

    MARSHAL_SIMPLE_TPM2B( sysContext, &( outsideInfo->b ) );

    Marshal_TPML_PCR_SELECTION( sysContext, creationPCR );

    SYS_CONTEXT->decryptAllowed = 1;
    SYS_CONTEXT->encryptAllowed = 1;
    SYS_CONTEXT->authAllowed = 1;

    CommonPrepareEpilogue( sysContext );

    return SYS_CONTEXT->rval;
}

TPM_RC Tss2_Sys_CreatePrimary_Complete(
    TSS2_SYS_CONTEXT *sysContext,
    TPM_HANDLE	*objectHandle,
    TPM2B_PUBLIC	*outPublic,
    TPM2B_CREATION_DATA	*creationData,
    TPM2B_DIGEST	*creationHash,
    TPMT_TK_CREATION	*creationTicket,
    TPM2B_NAME	*name
    )
{
    if( sysContext == NULL )
    {
        return( TSS2_SYS_RC_BAD_REFERENCE );
    }

    Unmarshal_UINT32( SYS_CONTEXT->tpmOutBuffPtr, SYS_CONTEXT->maxResponseSize, &(SYS_CONTEXT->nextData), objectHandle, &(SYS_CONTEXT->rval) );

    CommonComplete( sysContext );

    Unmarshal_TPM2B_PUBLIC( sysContext, outPublic );

    Unmarshal_TPM2B_CREATION_DATA( sysContext, creationData );

    UNMARSHAL_SIMPLE_TPM2B( sysContext, &( creationHash->b ) );

    Unmarshal_TPMT_TK_CREATION( sysContext, creationTicket );

    UNMARSHAL_SIMPLE_TPM2B( sysContext, &( name->b ) );

    return SYS_CONTEXT->rval;
}

TPM_RC Tss2_Sys_CreatePrimary(
    TSS2_SYS_CONTEXT *sysContext,
    TPMI_RH_HIERARCHY	primaryHandle,
    TSS2_SYS_CMD_AUTHS const *cmdAuthsArray,
    TPM2B_SENSITIVE_CREATE	*inSensitive,
    TPM2B_PUBLIC	*inPublic,
    TPM2B_DATA	*outsideInfo,
    TPML_PCR_SELECTION	*creationPCR,
    TPM_HANDLE	*objectHandle,
    TPM2B_PUBLIC	*outPublic,
    TPM2B_CREATION_DATA	*creationData,
    TPM2B_DIGEST	*creationHash,
    TPMT_TK_CREATION	*creationTicket,
    TPM2B_NAME	*name,
    TSS2_SYS_RSP_AUTHS *rspAuthsArray
    )
{
    TSS2_RC     rval = TPM_RC_SUCCESS;

    if( creationPCR == NULL  )
	{
		return TSS2_SYS_RC_BAD_REFERENCE;
	} 

    rval = Tss2_Sys_CreatePrimary_Prepare( sysContext, primaryHandle, inSensitive, inPublic, outsideInfo, creationPCR );
    
    if( rval == TSS2_RC_SUCCESS )
    {
        rval = CommonOneCall( sysContext, cmdAuthsArray, rspAuthsArray );

        if( rval == TSS2_RC_SUCCESS )
        {
            rval = Tss2_Sys_CreatePrimary_Complete( sysContext, objectHandle, outPublic, creationData, creationHash, creationTicket, name );
        }
    }
    
    return rval;
}

