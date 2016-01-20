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

TPM_RC Tss2_Sys_ZGen_2Phase_Prepare(
    TSS2_SYS_CONTEXT *sysContext,
    TPMI_DH_OBJECT	keyA,
    TPM2B_ECC_POINT	*inQsB,
    TPM2B_ECC_POINT	*inQeB,
    TPMI_ECC_KEY_EXCHANGE	inScheme,
    UINT16	counter
    )
{
    if( sysContext == NULL )
    {
        return( TSS2_SYS_RC_BAD_REFERENCE );
    }

     

    CommonPreparePrologue( sysContext, TPM_CC_ZGen_2Phase );

    Marshal_UINT32( SYS_CONTEXT->tpmInBuffPtr, SYS_CONTEXT->maxCommandSize, &(SYS_CONTEXT->nextData), keyA, &(SYS_CONTEXT->rval) );

    if( inQsB == 0 )
	{
		SYS_CONTEXT->decryptNull = 1;
	}
            
    Marshal_TPM2B_ECC_POINT( sysContext, inQsB );

    Marshal_TPM2B_ECC_POINT( sysContext, inQeB );

    Marshal_UINT16( SYS_CONTEXT->tpmInBuffPtr, SYS_CONTEXT->maxCommandSize, &(SYS_CONTEXT->nextData), inScheme, &(SYS_CONTEXT->rval) );

    Marshal_UINT16( SYS_CONTEXT->tpmInBuffPtr, SYS_CONTEXT->maxCommandSize, &(SYS_CONTEXT->nextData), counter, &(SYS_CONTEXT->rval) );

    SYS_CONTEXT->decryptAllowed = 1;
    SYS_CONTEXT->encryptAllowed = 1;
    SYS_CONTEXT->authAllowed = 1;

    CommonPrepareEpilogue( sysContext );

    return SYS_CONTEXT->rval;
}

TPM_RC Tss2_Sys_ZGen_2Phase_Complete(
    TSS2_SYS_CONTEXT *sysContext,
    TPM2B_ECC_POINT	*outZ1,
    TPM2B_ECC_POINT	*outZ2
    )
{
    if( sysContext == NULL )
    {
        return( TSS2_SYS_RC_BAD_REFERENCE );
    }

    CommonComplete( sysContext );

    Unmarshal_TPM2B_ECC_POINT( sysContext, outZ1 );

    Unmarshal_TPM2B_ECC_POINT( sysContext, outZ2 );

    return SYS_CONTEXT->rval;
}

TPM_RC Tss2_Sys_ZGen_2Phase(
    TSS2_SYS_CONTEXT *sysContext,
    TPMI_DH_OBJECT	keyA,
    TSS2_SYS_CMD_AUTHS const *cmdAuthsArray,
    TPM2B_ECC_POINT	*inQsB,
    TPM2B_ECC_POINT	*inQeB,
    TPMI_ECC_KEY_EXCHANGE	inScheme,
    UINT16	counter,
    TPM2B_ECC_POINT	*outZ1,
    TPM2B_ECC_POINT	*outZ2,
    TSS2_SYS_RSP_AUTHS *rspAuthsArray
    )
{
    TSS2_RC     rval = TPM_RC_SUCCESS;

     

    rval = Tss2_Sys_ZGen_2Phase_Prepare( sysContext, keyA, inQsB, inQeB, inScheme, counter );
    
    if( rval == TSS2_RC_SUCCESS )
    {
        rval = CommonOneCall( sysContext, cmdAuthsArray, rspAuthsArray );

        if( rval == TSS2_RC_SUCCESS )
        {
            rval = Tss2_Sys_ZGen_2Phase_Complete( sysContext, outZ1, outZ2 );
        }
    }
    
    return rval;
}

