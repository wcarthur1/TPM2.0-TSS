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

void Unmarshal_TPMU_CAPABILITIES(
	TSS2_SYS_CONTEXT *sysContext,
	TPMU_CAPABILITIES *capabilities,
	UINT32 selector
	)
{
	if( SYS_CONTEXT->rval != TSS2_RC_SUCCESS )
		return;

	if( capabilities == 0 )
		return;

	switch( selector )
	{
#ifdef TPM_CAP_ALGS
	case TPM_CAP_ALGS:
			Unmarshal_TPML_ALG_PROPERTY( sysContext, &capabilities->algorithms );
			break;
#endif
#ifdef TPM_CAP_HANDLES
	case TPM_CAP_HANDLES:
			Unmarshal_TPML_HANDLE( sysContext, &capabilities->handles );
			break;
#endif
#ifdef TPM_CAP_COMMANDS
	case TPM_CAP_COMMANDS:
			Unmarshal_TPML_CCA( sysContext, &capabilities->command );
			break;
#endif
#ifdef TPM_CAP_PP_COMMANDS
	case TPM_CAP_PP_COMMANDS:
			Unmarshal_TPML_CC( sysContext, &capabilities->ppCommands );
			break;
#endif
#ifdef TPM_CAP_AUDIT_COMMANDS
	case TPM_CAP_AUDIT_COMMANDS:
			Unmarshal_TPML_CC( sysContext, &capabilities->auditCommands );
			break;
#endif
#ifdef TPM_CAP_PCRS
	case TPM_CAP_PCRS:
			Unmarshal_TPML_PCR_SELECTION( sysContext, &capabilities->assignedPCR );
			break;
#endif
#ifdef TPM_CAP_TPM_PROPERTIES
	case TPM_CAP_TPM_PROPERTIES:
			Unmarshal_TPML_TAGGED_TPM_PROPERTY( sysContext, &capabilities->tpmProperties );
			break;
#endif
#ifdef TPM_CAP_PCR_PROPERTIES
	case TPM_CAP_PCR_PROPERTIES:
			Unmarshal_TPML_TAGGED_PCR_PROPERTY( sysContext, &capabilities->pcrProperties );
			break;
#endif
#ifdef TPM_CAP_ECC_CURVES
	case TPM_CAP_ECC_CURVES:
			Unmarshal_TPML_ECC_CURVE( sysContext, &capabilities->eccCurves );
			break;
#endif
	}
	return;
}
