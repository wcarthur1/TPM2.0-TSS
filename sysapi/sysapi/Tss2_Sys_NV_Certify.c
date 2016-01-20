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

TPM_RC Tss2_Sys_NV_Certify_Prepare(
    TSS2_SYS_CONTEXT *sysContext,
    TPMI_DH_OBJECT	signHandle,
    TPMI_RH_NV_AUTH	authHandle,
    TPMI_RH_NV_INDEX	nvIndex,
    TPM2B_DATA	*qualifyingData,
    TPMT_SIG_SCHEME	*inScheme,
    UINT16	size,
    UINT16	offset
    )
{
    if( sysContext == NULL )
    {
        return( TSS2_SYS_RC_BAD_REFERENCE );
    }

    if( inScheme == NULL  )
	{
		return TSS2_SYS_RC_BAD_REFERENCE;
	} 

    CommonPreparePrologue( sysContext, TPM_CC_NV_Certify );

    Marshal_UINT32( SYS_CONTEXT->tpmInBuffPtr, SYS_CONTEXT->maxCommandSize, &(SYS_CONTEXT->nextData), signHandle, &(SYS_CONTEXT->rval) );

    Marshal_UINT32( SYS_CONTEXT->tpmInBuffPtr, SYS_CONTEXT->maxCommandSize, &(SYS_CONTEXT->nextData), authHandle, &(SYS_CONTEXT->rval) );

    Marshal_UINT32( SYS_CONTEXT->tpmInBuffPtr, SYS_CONTEXT->maxCommandSize, &(SYS_CONTEXT->nextData), nvIndex, &(SYS_CONTEXT->rval) );

    if( qualifyingData == 0 )
	{
		SYS_CONTEXT->decryptNull = 1;
	}
            
    MARSHAL_SIMPLE_TPM2B( sysContext, &( qualifyingData->b ) );

    Marshal_TPMT_SIG_SCHEME( sysContext, inScheme );

    Marshal_UINT16( SYS_CONTEXT->tpmInBuffPtr, SYS_CONTEXT->maxCommandSize, &(SYS_CONTEXT->nextData), size, &(SYS_CONTEXT->rval) );

    Marshal_UINT16( SYS_CONTEXT->tpmInBuffPtr, SYS_CONTEXT->maxCommandSize, &(SYS_CONTEXT->nextData), offset, &(SYS_CONTEXT->rval) );

    SYS_CONTEXT->decryptAllowed = 1;
    SYS_CONTEXT->encryptAllowed = 1;
    SYS_CONTEXT->authAllowed = 1;

    CommonPrepareEpilogue( sysContext );

    return SYS_CONTEXT->rval;
}

TPM_RC Tss2_Sys_NV_Certify_Complete(
    TSS2_SYS_CONTEXT *sysContext,
    TPM2B_ATTEST	*certifyInfo,
    TPMT_SIGNATURE	*signature
    )
{
    if( sysContext == NULL )
    {
        return( TSS2_SYS_RC_BAD_REFERENCE );
    }

    CommonComplete( sysContext );

    UNMARSHAL_SIMPLE_TPM2B( sysContext, &( certifyInfo->b ) );

    Unmarshal_TPMT_SIGNATURE( sysContext, signature );

    return SYS_CONTEXT->rval;
}

TPM_RC Tss2_Sys_NV_Certify(
    TSS2_SYS_CONTEXT *sysContext,
    TPMI_DH_OBJECT	signHandle,
    TPMI_RH_NV_AUTH	authHandle,
    TPMI_RH_NV_INDEX	nvIndex,
    TSS2_SYS_CMD_AUTHS const *cmdAuthsArray,
    TPM2B_DATA	*qualifyingData,
    TPMT_SIG_SCHEME	*inScheme,
    UINT16	size,
    UINT16	offset,
    TPM2B_ATTEST	*certifyInfo,
    TPMT_SIGNATURE	*signature,
    TSS2_SYS_RSP_AUTHS *rspAuthsArray
    )
{
    TSS2_RC     rval = TPM_RC_SUCCESS;

    if( inScheme == NULL  )
	{
		return TSS2_SYS_RC_BAD_REFERENCE;
	} 

    rval = Tss2_Sys_NV_Certify_Prepare( sysContext, signHandle, authHandle, nvIndex, qualifyingData, inScheme, size, offset );
    
    if( rval == TSS2_RC_SUCCESS )
    {
        rval = CommonOneCall( sysContext, cmdAuthsArray, rspAuthsArray );

        if( rval == TSS2_RC_SUCCESS )
        {
            rval = Tss2_Sys_NV_Certify_Complete( sysContext, certifyInfo, signature );
        }
    }
    
    return rval;
}

