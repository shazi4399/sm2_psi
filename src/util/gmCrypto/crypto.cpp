/*
 * crypto.cpp
 *
 *  Created on: Jun 28, 2022
 *      Author: tcoole
 */

#include "crypto.h"
void oaid_hashpoint(Ipp8u *oaid, uint32_t len, IppsGFpECPoint *pGfpoint, void *pCtx, IppsGFpECState *pGfpEC)
{
	int pScratchBuffersize;
	ippsGFpECScratchBufferSize(1, pGfpEC, &pScratchBuffersize);
	Ipp8u *sPointHashBuffer = new Ipp8u[pScratchBuffersize];

	Ipp32u hdr = 0x78903473;
	while (0 != ippsGFpECSetPointHash_rmf(hdr, oaid, len, pGfpoint, pGfpEC, ippsHashMethod_SM3(), sPointHashBuffer))
	{
		hdr += 0xf387;
	};

	delete[] sPointHashBuffer;
}
void mul_randkey(const IppsGFpECPoint *pGfpSrc, BigNumber pRrd, IppsGFpECPoint *pGfpDst, IppsGFpECState *pGfpEC)
{
	int pScratchBuffersize;
	ippsGFpECScratchBufferSize(1, pGfpEC, &pScratchBuffersize);
	Ipp8u *pScratchBuffer = new Ipp8u[pScratchBuffersize];
	ippsGFpECMulPoint(pGfpSrc, pRrd, pGfpDst, pGfpEC, pScratchBuffer);
	delete[] pScratchBuffer;
}
#define SAFE_FREE(ptr, size)      \
	do                            \
	{                             \
		if (NULL != (ptr))        \
		{                         \
			memset(ptr, 0, size); \
			free(ptr);            \
			(ptr) = NULL;         \
		}                         \
	} while (0);

/* Define EC over GF(p) context for SM2 */
static IppsECCPState *new_ECC_sm2(void)
{
	int ctxSize = 0;
	IppsECCPState *pSM2 = NULL;
	IppStatus status = ippStsNoErr;

	// Get the size of ECC context for SM2
	status = ippsECCPGetSize(256, &ctxSize);
	if (status != ippStsNoErr)
	{
		printf("Error: fail to get size of ECCP\n");
		return NULL;
	}

	// Allocate the ECC context for SM2
	pSM2 = (IppsECCPState *)(malloc(ctxSize));
	if (pSM2 == NULL)
	{
		printf("Error: fail to allocate memory for ECCP\n");
		return NULL;
	}

	// Initialize the ECC context for SM2
	status = ippsECCPInit(256, pSM2);
	if (status != ippStsNoErr)
	{
		printf("Error: fail to initialize ECCP\n");
		SAFE_FREE(pSM2, ctxSize);
		return NULL;
	}

	// Set up a recommended set of domain parameters for ECC context for SM2
	status = ippsECCPSetStd(ippECPstdSM2, pSM2);
	if (status != ippStsNoErr)
	{
		printf("Error: fail to set up recommended set of domain parameters for ECCP\n");
		SAFE_FREE(pSM2, ctxSize);
		return NULL;
	}

	return pSM2;
}

/* Define EC over GF(p) Point context */
static IppsECCPPointState *new_ECC_Point(void)
{
	int ctxSize = 0;
	IppsECCPPointState *pPoint = NULL;
	IppStatus status = ippStsNoErr;

	status = ippsECCPPointGetSize(256, &ctxSize);
	if (status != ippStsNoErr)
	{
		printf("Error: fail to get size of ECCPPoint\n");
		return NULL;
	}

	pPoint = (IppsECCPPointState *)(malloc(ctxSize));
	if (pPoint == NULL)
	{
		printf("Error: fail to allocate memory for ECCPPoint\n");
		return NULL;
	}

	status = ippsECCPPointInit(256, pPoint);
	if (status != ippStsNoErr)
	{
		printf("Error: fail to initialize ECCPPoint\n");
		SAFE_FREE(pPoint, ctxSize);
		return NULL;
	}

	return pPoint;
}

/* Define Big Number context */
static IppsBigNumState *new_BN(int len, const unsigned int *pData)
{
	int ctxSize = 0;
	IppsBigNumState *pBN = NULL;
	IppStatus status = ippStsNoErr;

	status = ippsBigNumGetSize(len, &ctxSize);
	if (status != ippStsNoErr)
	{
		printf("Error: fail to get size of BigNum\n");
		return NULL;
	}
	pBN = (IppsBigNumState *)(malloc(ctxSize));
	if (pBN == NULL)
	{
		printf("Error: fail to allocate memory for BigNum\n");
		return NULL;
	}

	status = ippsBigNumInit(len, pBN);
	if (status != ippStsNoErr)
	{
		printf("Error: fail to initialize BigNum\n");
		SAFE_FREE(pBN, ctxSize);
		return NULL;
	}

	if (pData)
		ippsSet_BN(IppsBigNumPOS, len, pData, pBN);

	return pBN;
}

/* Calculate ZA = H256(ENTLA || IDA || a || b || xG || yG || xA || yA) */
static int hash_digest_z(const IppsHashMethod *hash_method, const Ipp8u *id, const int id_len, const IppsBigNumState *pubX, const IppsBigNumState *pubY, unsigned char *z_digest)
{
	int ctx_size = 0;
	IppsHashState_rmf *hash_handle = NULL;
	IppStatus ipp_ret = ippStsNoErr;
	int ret = 0;

	int id_bit_len = id_len * 8;
	unsigned char entl[2] = {0};
	entl[0] = (id_bit_len & 0xff00) >> 8;
	entl[1] = id_bit_len & 0xff;
	unsigned char a[32] = {
		0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc};
	unsigned char b[32] = {
		0x28, 0xe9, 0xfa, 0x9e, 0x9d, 0x9f, 0x5e, 0x34,
		0x4d, 0x5a, 0x9e, 0x4b, 0xcf, 0x65, 0x09, 0xa7,
		0xf3, 0x97, 0x89, 0xf5, 0x15, 0xab, 0x8f, 0x92,
		0xdd, 0xbc, 0xbd, 0x41, 0x4d, 0x94, 0x0e, 0x93};
	unsigned char xG[32] = {
		0x32, 0xc4, 0xae, 0x2c, 0x1f, 0x19, 0x81, 0x19,
		0x5f, 0x99, 0x04, 0x46, 0x6a, 0x39, 0xc9, 0x94,
		0x8f, 0xe3, 0x0b, 0xbf, 0xf2, 0x66, 0x0b, 0xe1,
		0x71, 0x5a, 0x45, 0x89, 0x33, 0x4c, 0x74, 0xc7};
	unsigned char yG[32] = {
		0xbc, 0x37, 0x36, 0xa2, 0xf4, 0xf6, 0x77, 0x9c,
		0x59, 0xbd, 0xce, 0xe3, 0x6b, 0x69, 0x21, 0x53,
		0xd0, 0xa9, 0x87, 0x7c, 0xc6, 0x2a, 0x47, 0x40,
		0x02, 0xdf, 0x32, 0xe5, 0x21, 0x39, 0xf0, 0xa0};
	unsigned char xA[32] = {0};
	unsigned char yA[32] = {0};

	do
	{
		ipp_ret = ippsGetOctString_BN(xA, 32, pubX);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to Convert BN value pubX into octet string xA\n");
			ret = -1;
			break;
		}
		ipp_ret = ippsGetOctString_BN(yA, 32, pubY);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to Convert BN value pubY into octet string yA\n");
			ret = -2;
			break;
		}

		ipp_ret = ippsHashGetSize_rmf(&ctx_size);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to get size of ippsHashGetSize_rmf\n");
			ret = -3;
			break;
		}

		hash_handle = (IppsHashState_rmf *)(malloc(ctx_size));
		if (!hash_handle)
		{
			printf("Error: fail to allocate memory for ippsHashGetSize_rmf\n");
			ret = -4;
			break;
		}

		// Set Hash 256 handler:
		// SM3 - ippsHashMethod_SM3()
		// SHA256 - ippsHashMethod_SHA256_TT()
		ipp_ret = ippsHashInit_rmf(hash_handle, hash_method);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to set hash 256 handler\n");
			ret = -5;
			break;
		}

		// ZA = H256(ENTLA || IDA || a || b || xG || yG || xA || yA)
		ipp_ret = ippsHashUpdate_rmf(entl, sizeof(entl), hash_handle);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to update hash value of ENTLA\n");
			ret = -6;
			break;
		}
		ipp_ret = ippsHashUpdate_rmf((unsigned char *)id, id_len, hash_handle);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to update hash value of IDA\n");
			ret = -7;
			break;
		}
		ipp_ret = ippsHashUpdate_rmf(a, sizeof(a), hash_handle);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to update hash value of a\n");
			ret = -8;
			break;
		}
		ipp_ret = ippsHashUpdate_rmf(b, sizeof(b), hash_handle);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to update hash value of b\n");
			ret = -9;
			break;
		}
		ipp_ret = ippsHashUpdate_rmf(xG, sizeof(xG), hash_handle);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to update hash value of xG\n");
			ret = -10;
			break;
		}
		ipp_ret = ippsHashUpdate_rmf(yG, sizeof(yG), hash_handle);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to update hash value of yG\n");
			ret = -11;
			break;
		}
		ipp_ret = ippsHashUpdate_rmf(xA, sizeof(xA), hash_handle);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to update hash value of xA\n");
			ret = -12;
			break;
		}
		ipp_ret = ippsHashUpdate_rmf(yA, sizeof(yA), hash_handle);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to update hash value of yA\n");
			ret = -13;
			break;
		}
		ipp_ret = ippsHashFinal_rmf(z_digest, hash_handle);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to complete message digesting and return digest\n");
			ret = -14;
			break;
		}
	} while (0);

	SAFE_FREE(hash_handle, sizeof(hash_handle));

	return ret;
}

/* Calculate ZA = H256(Z||M) */
static int hash_digest_with_preprocess(const IppsHashMethod *hash_method, const Ipp8u *msg, const int msg_len, const Ipp8u *id, const int id_len, const IppsBigNumState *pubX, const IppsBigNumState *pubY, unsigned char *digest)
{
	int ctx_size = 0;
	IppsHashState_rmf *hash_handle = NULL;
	IppStatus ipp_ret = ippStsNoErr;
	int ret = 0;
	unsigned char z_digest[32] = {0};

	do
	{
		ret = hash_digest_z(hash_method, id, id_len, pubX, pubY, z_digest);
		if (ret != 0)
		{
			printf("Error: fail to complete SM3 digest of leading data Z\n");
			return -1;
			break;
		}

		ipp_ret = ippsHashGetSize_rmf(&ctx_size);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to get size of IppsHashState_rmf\n");
			ret = -2;
			break;
		}

		hash_handle = (IppsHashState_rmf *)(malloc(ctx_size));
		if (!hash_handle)
		{
			printf("Error: fail to allocate memory for IppsHashState_rmf\n");
			ret = -3;
			break;
		}

		// Set Hash 256 handler:
		// SM3 - ippsHashMethod_SM3()
		// SHA256 - ippsHashMethod_SHA256_TT()
		ipp_ret = ippsHashInit_rmf(hash_handle, hash_method);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to initialize IppsHashState_rmf\n");
			ret = -4;
			break;
		}

		// ZA = H256(Z||M)
		ipp_ret = ippsHashUpdate_rmf(z_digest, sizeof(z_digest), hash_handle);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to update hash value of Z\n");
			ret = -5;
			break;
		}
		ipp_ret = ippsHashUpdate_rmf(msg, msg_len, hash_handle);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to update hash value of M\n");
			ret = -6;
			break;
		}
		ipp_ret = ippsHashFinal_rmf(digest, hash_handle);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to complete message digesting and return digest\n");
			ret = -7;
			break;
		}
	} while (0);

	SAFE_FREE(hash_handle, sizeof(hash_handle));

	return ret;
}

/* SM2 sign */
static int sm2_do_sign(const IppsBigNumState *regPrivateKey, const IppsHashMethod *hash_method, Ipp8u *id, int id_len, Ipp8u *msg, const int msg_len, IppsBigNumState *signX, IppsBigNumState *signY)
{
	IppsECCPState *pECCPS = NULL;
	IppsPRNGState *pPRNGS = NULL;
	IppsBigNumState *ephPrivateKey = NULL;
	IppsECCPPointState *regPublicKey = NULL, *ephPublicKey = NULL;
	IppsBigNumState *pMsg = NULL;
	IppsBigNumState *pX = NULL, *pY = NULL;
	IppStatus ipp_ret = ippStsNoErr;
	int ret = 0;
	unsigned char hash[32] = {0};

	do
	{
		// 1. Create ECC context for SM2
		pECCPS = new_ECC_sm2();
		if (pECCPS == NULL)
		{
			printf("Error: fail to create pECCPS\n");
			ret = -1;
			break;
		}

		// 2. Create ephemeral private key and public key, regular public key
		ephPrivateKey = new_BN(ordSize, nullptr);
		if (ephPrivateKey == NULL)
		{
			printf("Error: fail to create ephemeral private key\n");
			ret = -2;
			break;
		}
		ephPublicKey = new_ECC_Point();
		if (ephPublicKey == NULL)
		{
			printf("Error: fail to create ephemeral public key\n");
			ret = -3;
			break;
		}
		regPublicKey = new_ECC_Point();
		if (regPublicKey == NULL)
		{
			printf("Error: fail to create regular public key\n");
			ret = -4;
			break;
		}
		ipp_ret = ippsECCPPublicKey(regPrivateKey, regPublicKey, pECCPS);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to calculate regular public key\n");
			ret = -5;
			break;
		}

		// 3. Generate ephemeral key pairs
		pPRNGS = newPRNG();
		if (pPRNGS == NULL)
		{
			printf("Error: fail to create pPRNGS\n");
			ret = -6;
			break;
		}

		ipp_ret = ippsECCPGenKeyPair(ephPrivateKey, ephPublicKey, pECCPS, ippsPRNGen, pPRNGS);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to generate ephemeral key pairs\n");
			ret = -7;
			break;
		}

		// 4. Create pX and pY
		pX = new_BN(ordSize, nullptr);
		if (pX == NULL)
		{
			printf("Error: fail to create pX\n");
			ret = -8;
			break;
		}
		pY = new_BN(ordSize, nullptr);
		if (pY == NULL)
		{
			printf("Error: fail to create pY\n");
			ret = -9;
			break;
		}
		ipp_ret = ippsECCPGetPoint(pX, pY, regPublicKey, pECCPS);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to convert internal presentation EC point into regular affine coordinates EC point\n");
			ret = -10;
			break;
		}

		// 5. Do user message digest
		ret = hash_digest_with_preprocess(hash_method, msg, msg_len, id, id_len, pX, pY, hash);
		if (ret != 0)
		{
			printf("Error: fail to do hash digest with preprocess\n");
			ret = -11;
			break;
		}
		pMsg = new_BN(ordSize, nullptr);
		if (pMsg == NULL)
		{
			printf("Error: fail to create BN\n");
			ret = -12;
			break;
		}
		ipp_ret = ippsSetOctString_BN(hash, sizeof(hash), pMsg);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to convert octet string into BN value\n");
			ret = -13;
			break;
		}

		// 6. Sign using ECC context for SM2
		ipp_ret = ippsECCPSetKeyPair(ephPrivateKey, ephPublicKey, ippFalse, pECCPS);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to set ephemeral key pairs\n");
			ret = -14;
			break;
		}
		ipp_ret = ippsECCPSignSM2(pMsg, regPrivateKey, ephPrivateKey, signX, signY, pECCPS);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to compute signature\n");
			ret = -15;
			break;
		}
	} while (0);

	// 7. Final, remove secret and release resources
	SAFE_FREE(pY, sizeof(pY));
	SAFE_FREE(pX, sizeof(pX));
	SAFE_FREE(pMsg, sizeof(pMsg));
	SAFE_FREE(regPublicKey, sizeof(regPublicKey));
	SAFE_FREE(ephPublicKey, sizeof(ephPublicKey));
	SAFE_FREE(ephPrivateKey, sizeof(ephPrivateKey));
	SAFE_FREE(pPRNGS, sizeof(pPRNGS));
	SAFE_FREE(pECCPS, sizeof(pECCPS));

	return ret;
}

/* SM2 verify */
static int sm2_do_verify(const IppsECCPPointState *regPublicKey, const IppsHashMethod *hash_method, Ipp8u *id, const int id_len, Ipp8u *msg, const int msg_len, IppsBigNumState *signX, IppsBigNumState *signY)
{
	IppsECCPState *pECCPS = NULL;
	IppsBigNumState *pMsg = NULL;
	IppsBigNumState *pX = NULL, *pY = NULL;
	IppStatus ipp_ret = ippStsNoErr;
	IppECResult eccResult = ippECValid;
	int ret = 0;
	unsigned char hash[32] = {0};

	do
	{
		// 1. Create ECC context for SM2
		pECCPS = new_ECC_sm2();
		if (pECCPS == NULL)
		{
			printf("Error: fail to create pECCPS\n");
			ret = -1;
			break;
		}

		// 2. Create pX and pY
		pX = new_BN(ordSize, nullptr);
		if (pX == NULL)
		{
			printf("Error: fail to create pX\n");
			ret = -2;
			break;
		}
		pY = new_BN(ordSize, nullptr);
		if (pY == NULL)
		{
			printf("Error: fail to create pY\n");
			ret = -3;
			break;
		}
		ipp_ret = ippsECCPGetPoint(pX, pY, regPublicKey, pECCPS);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to convert internal presentation EC point into regular affine coordinates EC point\n");
			ret = -4;
			break;
		}

		// 3. Do user message digest
		ret = hash_digest_with_preprocess(hash_method, msg, msg_len, id, id_len, pX, pY, hash);
		if (ret != 0)
		{
			printf("Error: fail to do hash digest with preprocess\n");
			ret = -5;
			break;
		}
		pMsg = new_BN(ordSize, nullptr);
		if (pMsg == NULL)
		{
			printf("Error: fail to create BN\n");
			ret = -6;
			break;
		}
		ipp_ret = ippsSetOctString_BN(hash, sizeof(hash), pMsg);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to convert octet string into BN value\n");
			ret = -7;
			break;
		}

		// 4. Verify using ECC context for SM2
		ipp_ret = ippsECCPSetKeyPair(NULL, regPublicKey, ippTrue, pECCPS);
		if (ipp_ret != ippStsNoErr)
		{
			printf("Error: fail to set regular public key\n");
			ret = -8;
			break;
		}
		ipp_ret = ippsECCPVerifySM2(pMsg, regPublicKey, signX, signY, &eccResult, pECCPS);
		if ((ipp_ret != ippStsNoErr) || (eccResult != ippECValid))
		{
			printf("Error: fail to verify signature\n");
			ret = -9;
			break;
		}
	} while (0);

	// 5. Final, remove secret and release resources
	SAFE_FREE(pY, sizeof(pY));
	SAFE_FREE(pX, sizeof(pX));
	SAFE_FREE(pMsg, sizeof(pMsg));
	SAFE_FREE(pECCPS, sizeof(pECCPS));

	return ret;
}
int sign_and_verify(const IppsBigNumState *regPrivateKey, const IppsECCPPointState *regPublicKey,
					uint32_t user_id, Ipp8u *sendBuffer, int neles, Ipp8u *recvBuffer, int pneles, CSocket *sock)
{
	IppsBigNumState *signX = NULL, *signY = NULL;
	Ipp8u *pSignX = new Ipp8u[halfPointByteLen];
	Ipp8u *pSignY = new Ipp8u[halfPointByteLen];
	Ipp8u user_id_str[idLen];
	user_id_str[idLen - 1] = '\0';
	memcpy(user_id_str, &user_id, idLen - 1);
	// 4. Create signX and signY
	signX = new_BN(ordSize, nullptr);
	if (signX == NULL)
	{
		printf("Error: fail to create signX\n");
		return -1;
	}
	signY = new_BN(ordSize, 0);
	if (signY == NULL)
	{
		printf("Error: fail to create signY\n");
		return -1;
	}
	int ret = sm2_do_sign(regPrivateKey, ippsHashMethod_SM3(), user_id_str, 4, sendBuffer, neles * 64, signX, signY);

	if (ret != 0)
	{
		printf("Error: fail to sign\n");
		return -2;
	}
	Ipp8u *ipp8uSignX = IppBNStoIpp8u(signX);
	Ipp8u *ipp8uSignY = IppBNStoIpp8u(signY);
	snd_and_rcv(ipp8uSignX, halfPointByteLen, pSignX, halfPointByteLen, sock);
	snd_and_rcv(ipp8uSignY, halfPointByteLen, pSignY, halfPointByteLen, sock);

	IppsBigNumState *pBBSSignX = new_BN(ordSize, nullptr);
	IppsBigNumState *pBBSSignY = new_BN(ordSize, nullptr);
	ippsSetOctString_BN(pSignX, halfPointByteLen, pBBSSignX);
	ippsSetOctString_BN(pSignY, halfPointByteLen, pBBSSignY);

	Ipp8u puserid[idLen];
	puserid[idLen - 1] = '\0';
	snd_and_rcv(user_id_str, idLen - 1, puserid, idLen - 1, sock);

	ret = sm2_do_verify(regPublicKey, ippsHashMethod_SM3(), puserid, idLen - 1, recvBuffer, pneles * pointByteLen, pBBSSignX, pBBSSignY);
	if (ret != 0)
	{
		printf("Error: fail to verify\n");
		return -3;
	}
	delete[] pBBSSignX;
	delete[] pBBSSignY;
	delete[] pSignX;
	delete[] pSignY;
	delete[] ipp8uSignX;
	delete[] ipp8uSignY;
	return 0;
}
int sign_and_verify(const IppsBigNumState *regPrivateKey, const IppsECCPPointState *regPublicKey,
					uint32_t user_id, Ipp8u *sendBuffer, int neles, Ipp8u *recvBuffer, int pneles, CSocket *sock, role_type role)
{
	IppsBigNumState *signX = NULL, *signY = NULL;
	Ipp8u *pSignX = new Ipp8u[halfPointByteLen];
	Ipp8u *pSignY = new Ipp8u[halfPointByteLen];
	Ipp8u user_id_str[idLen];
	user_id_str[idLen - 1] = '\0';
	memcpy(user_id_str, &user_id, idLen - 1);
	// 4. Create signX and signY
	signX = new_BN(ordSize, nullptr);
	if (signX == NULL)
	{
		printf("Error: fail to create signX\n");
		return -1;
	}
	signY = new_BN(ordSize, 0);
	if (signY == NULL)
	{
		printf("Error: fail to create signY\n");
		return -1;
	}
	int ret = sm2_do_sign(regPrivateKey, ippsHashMethod_SM3(), user_id_str, 4, sendBuffer, neles * pointByteLen, signX, signY);

	if (ret != 0)
	{
		printf("Error: fail to sign\n");
		return -2;
	}
	Ipp8u *ipp8uSignX = IppBNStoIpp8u(signX);
	Ipp8u *ipp8uSignY = IppBNStoIpp8u(signY);
	snd_and_rcv(ipp8uSignX, halfPointByteLen, pSignX, halfPointByteLen, sock, role);
	snd_and_rcv(ipp8uSignY, halfPointByteLen, pSignY, halfPointByteLen, sock, role);
	Ipp8u puserid[idLen];
	puserid[idLen - 1] = '\0';
	snd_and_rcv(user_id_str, idLen - 1, puserid, idLen - 1, sock, role);
	if (role == CLIENT)
	{
		IppsBigNumState *pBBSSignX = new_BN(ordSize, nullptr);
		IppsBigNumState *pBBSSignY = new_BN(ordSize, nullptr);
		ippsSetOctString_BN(pSignX, halfPointByteLen, pBBSSignX);
		ippsSetOctString_BN(pSignY, halfPointByteLen, pBBSSignY);

		ret = sm2_do_verify(regPublicKey, ippsHashMethod_SM3(), puserid, idLen - 1, recvBuffer, pneles * pointByteLen, pBBSSignX, pBBSSignY);
		if (ret != 0)
		{
			printf("Error: fail to verify\n");
			return -3;
		}
		delete[] pBBSSignX;
		delete[] pBBSSignY;
	}
	delete[] pSignX;
	delete[] pSignY;
	delete[] ipp8uSignX;
	delete[] ipp8uSignY;
	return 0;
}
int sign_verify_init(IppsECCPState *&pECCPS, IppsBigNumState *&regPrivateKey, IppsECCPPointState *&regPublicKey, CSocket *sock)
{
	/*
	sign and verify init
	*/

	IppStatus ipp_ret = ippStsNoErr;
	int ret = 0;

	// 1. Create ECC context for SM2
	pECCPS = new_ECC_sm2();
	if (pECCPS == NULL)
	{
		printf("Error: fail to create ecc context for sm2\n");
		return -1;
	}

	// 2. Create regular private key and public key
	regPrivateKey = newRandBN(8); // 256bits

	if (regPrivateKey == NULL)
	{
		printf("Error: fail to create regular private key\n");
		return -2;
	}
	regPublicKey = new_ECC_Point();
	if (regPublicKey == NULL)
	{
		printf("Error: fail to create regular public key\n");
		return -1;
	}

	// 3. Create regular private and public key pairs
	ipp_ret = ippsECCPPublicKey(regPrivateKey, regPublicKey, pECCPS);
	if (ipp_ret != ippStsNoErr)
	{
		printf("Error: fail to calculate regular public key\n");
		return -3;
	}
	IppsBigNumState *pX = new_BN(ordSize, nullptr);
	IppsBigNumState *pY = new_BN(ordSize, nullptr);
	ippsECCPGetPoint(pX, pY, regPublicKey, pECCPS);
	Ipp8u *pukSendBuffer = new Ipp8u[pointByteLen];
	Ipp8u *pukRecvBuffer = new Ipp8u[pointByteLen];
	Ipp8u *px = IppBNStoIpp8u(pX);
	Ipp8u *py = IppBNStoIpp8u(pY);
	memcpy(pukSendBuffer, px, halfPointByteLen);
	memcpy(pukSendBuffer + halfPointByteLen, py, halfPointByteLen);
	delete[] px;
	delete[] py;

	snd_and_rcv(pukSendBuffer, pointByteLen, pukRecvBuffer, pointByteLen, sock);
	delete[] pukSendBuffer;

	Ipp8u *pkx = new Ipp8u[halfPointByteLen];
	Ipp8u *pky = new Ipp8u[halfPointByteLen];
	memcpy(pkx, pukRecvBuffer, halfPointByteLen);
	memcpy(pky, pukRecvBuffer + halfPointByteLen, halfPointByteLen);
	delete[] pukRecvBuffer;

	ippsSetOctString_BN(pkx, halfPointByteLen, pX);
	ippsSetOctString_BN(pky, halfPointByteLen, pY);

	ippsECCPSetPoint(pX, pY, regPublicKey, pECCPS);
	delete[] pkx;
	delete[] pky;
	delete (Ipp8u *)pX;
	delete (Ipp8u *)pY;
	return 0;
}
void GFpECinit(IppsGFpState *&pGF, IppsGFpECState *&pGfpec, IppsGFpECPoint *&pGfpoint,
			   IppsGFpECPoint *&pGfpointPmulc, IppsGFpECPoint *&pGfpointPmuls, IppsGFpECPoint *&pGfpointPmulsc)
{
	int gfpSize = 0;
	int fbSize = ecPar;
	ippsGFpGetSize(fbSize, &gfpSize);
	pGF = (IppsGFpState *)new Ipp8u[gfpSize];
	IppsBigNumState *P = new_BN(ordSize, nullptr);
	ippsSetOctString_BN(mp, halfPointByteLen, P);
	ippsGFpInit(P, 256, ippsGFpMethod_p256sm2(), pGF); // or ippsGFpMethod_p256r1()

	int elemSize = 0;
	ippsGFpElementGetSize(pGF, &elemSize);
	IppsGFpElement *pGfpElemA = (IppsGFpElement *)(new Ipp8u[elemSize]);
	ippsGFpElementInit(nullptr, 0, pGfpElemA, pGF);
	ippsGFpSetElementOctString(a, pointByteLen / 2, pGfpElemA, pGF);

	IppsGFpElement *pGfpElemB = (IppsGFpElement *)(new Ipp8u[elemSize]);
	ippsGFpElementInit(nullptr, 0, pGfpElemB, pGF);
	ippsGFpSetElementOctString(b, pointByteLen / 2, pGfpElemB, pGF);

	int pecSize = 0;
	ippsGFpECGetSize(pGF, &pecSize);
	pGfpec = (IppsGFpECState *)(new Ipp8u[pecSize]);
	ippsGFpECInit(pGF, pGfpElemA, pGfpElemB, pGfpec);

	// msg base point
	int pecpSize = 0;
	ippsGFpECPointGetSize(pGfpec, &pecpSize);
	pGfpoint = (IppsGFpECPoint *)(new Ipp8u[pecpSize]);
	ippsGFpECPointInit(nullptr, nullptr, pGfpoint, pGfpec);

	// pa base point
	pGfpointPmulc = (IppsGFpECPoint *)(new Ipp8u[pecpSize]);
	ippsGFpECPointInit(nullptr, nullptr, pGfpointPmulc, pGfpec);

	// ps base point
	pGfpointPmuls = (IppsGFpECPoint *)(new Ipp8u[pecpSize]);
	ippsGFpECPointInit(nullptr, nullptr, pGfpointPmuls, pGfpec);

	// psc base point
	pGfpointPmulsc = (IppsGFpECPoint *)(new Ipp8u[pecpSize]);
	ippsGFpECPointInit(nullptr, nullptr, pGfpointPmulsc, pGfpec);

	delete (Ipp8u *)pGfpElemB;
	delete (Ipp8u *)pGfpElemA;
	delete (Ipp8u *)P;
}