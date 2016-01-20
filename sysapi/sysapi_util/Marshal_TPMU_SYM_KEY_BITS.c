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

void Marshal_TPMU_SYM_KEY_BITS(
	TSS2_SYS_CONTEXT *sysContext,
	TPMU_SYM_KEY_BITS *symKeyBits,
	UINT32 selector
	)
{
	if( SYS_CONTEXT->rval != TSS2_RC_SUCCESS )
		return;

	switch( selector )
	{
#ifdef TPM_ALG_AES
	case TPM_ALG_AES:
			Marshal_UINT16( SYS_CONTEXT->tpmInBuffPtr, SYS_CONTEXT->maxCommandSize, &(SYS_CONTEXT->nextData), symKeyBits->aes, &( SYS_CONTEXT->rval ) );
			break;
#endif
#ifdef TPM_ALG_SM4
	case TPM_ALG_SM4:
			Marshal_UINT16( SYS_CONTEXT->tpmInBuffPtr, SYS_CONTEXT->maxCommandSize, &(SYS_CONTEXT->nextData), symKeyBits->sm4, &( SYS_CONTEXT->rval ) );
			break;
#endif
#ifdef TPM_ALG_CAMELLIA
	case TPM_ALG_CAMELLIA:
			Marshal_UINT16( SYS_CONTEXT->tpmInBuffPtr, SYS_CONTEXT->maxCommandSize, &(SYS_CONTEXT->nextData), symKeyBits->camellia, &( SYS_CONTEXT->rval ) );
			break;
#endif
#ifdef TPM_ALG_XOR
	case TPM_ALG_XOR:
			Marshal_UINT16( SYS_CONTEXT->tpmInBuffPtr, SYS_CONTEXT->maxCommandSize, &(SYS_CONTEXT->nextData), symKeyBits->exclusiveOr, &( SYS_CONTEXT->rval ) );
			break;
#endif
#ifdef TPM_ALG_NULL
	case TPM_ALG_NULL:
					break;
#endif
	}
	return;
}
