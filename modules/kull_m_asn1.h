/*
	sekcee_extractor
	
	
*/
#pragma once
#include "globals.h"
#include <Winldap.h>
#include <Winber.h>
#include "kull_m_string.h"

#define DIRTY_ASN1_ID_BOOLEAN			0x01
#define DIRTY_ASN1_ID_INTEGER			0x02
#define DIRTY_ASN1_ID_BIT_STRING		0x03
#define DIRTY_ASN1_ID_OCTET_STRING		0x04
#define DIRTY_ASN1_ID_NULL				0x05
#define DIRTY_ASN1_ID_OBJECT_IDENTIFIER	0x06
#define DIRTY_ASN1_ID_GENERAL_STRING	0x1b
#define DIRTY_ASN1_ID_GENERALIZED_TIME	0x18
#define DIRTY_ASN1_ID_SEQUENCE			0x30

#define DIRTY_ASN1_MASK_APPLICATION		0x60
#define DIRTY_ASN1_MASK_CONTEXT			0xa0

#define MAKE_APP_TAG(AppId)		((ber_tag_t) (DIRTY_ASN1_MASK_APPLICATION | AppId))
#define MAKE_CTX_TAG(CtxId)		((ber_tag_t) (DIRTY_ASN1_MASK_CONTEXT | CtxId))

void kull_m_asn1_BitStringFromULONG(BerElement * pBer, ULONG data);
void kull_m_asn1_GenTime(BerElement * pBer, PFILETIME localtime);
void kull_m_asn1_GenString(BerElement * pBer, PCUNICODE_STRING String);