/*
 * dh-psi.cpp
 *
 *  Created on: Jun 28, 2022
 *      Author: tcoole
 */
#include "dh-psi.h"

void echo_hello(){
	cout<<"cPython success!"<<endl;
}
uint32_t dh_psi(role_type role, uint32_t neles, uint32_t pneles, uint32_t *elebytelens, uint8_t **elements,
				uint8_t ***result, uint32_t **resbytelens, CSocket *sock, uint32_t user_id)
{
	IppsGFpECState *pGfpec;
	IppsGFpECPoint *pGfpoint, *pGfpointPmulc, *pGfpointPmuls, *pGfpointPmulsc;
	IppsGFpState *pGF;
	IppsPRNGState *pPrng;
	IppsECCPState *pECCPS;
	IppsBigNumState *regPrivateKey;
	IppsECCPPointState *regPublicKey;
	GFpECinit(pGF, pGfpec, pGfpoint, pGfpointPmulc, pGfpointPmuls, pGfpointPmulsc);
	if (0 != sign_verify_init(pECCPS, regPrivateKey, regPublicKey, sock))
	{
		cout << "Error:sign & verify init" << endl;
		return 0;
	};
	pPrng = newPRNG();
	IppsBigNumState *randC = newRandBN(bnBitSize / 32); // 256bit = 256/32(uint32_t)
	Ipp8u **GmulA = new Ipp8u *[neles];
	Ipp8u **GmulB = new Ipp8u *[pneles];
	Ipp8u **GmulBmulA = new Ipp8u *[pneles];

	for (int i = 0; i < neles; ++i)
	{
		GmulA[i] = new Ipp8u[pointByteLen];
	}
	for (int i = 0; i < pneles; ++i)
	{
		GmulB[i] = new Ipp8u[pointByteLen];
		GmulBmulA[i] = new Ipp8u[pointByteLen];
	}
	for (size_t i = 0; i < neles; ++i)
	{

		oaid_hashpoint(elements[i], elebytelens[i], pGfpoint, pPrng, pGfpec);
		mul_randkey(pGfpoint, randC, pGfpointPmulc, pGfpec);
		ippsGFpECGetPointOctString(pGfpointPmulc, GmulA[i], pointByteLen, pGfpec);
	}

	Ipp8u *sendBuffer = new Ipp8u[neles * pointByteLen];
	Ipp8u *recvBuffer = new Ipp8u[pneles * pointByteLen];
	size_t index = 0;
	for (size_t i = 0; i < neles; ++i, index += pointByteLen)
	{
		memcpy(sendBuffer + index, GmulA[i], pointByteLen);
	}
	snd_and_rcv(sendBuffer, neles * pointByteLen, recvBuffer, pneles * pointByteLen, sock);
	if (0 != sign_and_verify(regPrivateKey, regPublicKey, user_id, sendBuffer, neles, recvBuffer, pneles, sock))
	{
		cout << "Error:sign & verify" << endl;
		return 0;
	}
	index = 0;
	for (size_t i = 0; i < pneles; ++i, index += pointByteLen)
	{
		memcpy(GmulB[i], recvBuffer + index, pointByteLen);
	}
	delete[] sendBuffer;
	delete[] recvBuffer;
	for (size_t i = 0; i < pneles; ++i)
	{
		ippsGFpECSetPointOctString(GmulB[i], pointByteLen, pGfpointPmuls, pGfpec);
		mul_randkey(pGfpointPmuls, randC, pGfpointPmulsc, pGfpec);
		ippsGFpECGetPointOctString(pGfpointPmulsc, GmulBmulA[i], pointByteLen, pGfpec);
	}
	index = 0;
	Ipp8u *sendBuffer2 = new Ipp8u[pneles * pointByteLen];
	for (size_t i = 0; i < pneles; ++i, index += pointByteLen)
	{
		memcpy(sendBuffer2 + index, GmulBmulA[i], pointByteLen);
	}
	Ipp8u *recvBuffer2 = new Ipp8u[neles * pointByteLen];
	snd_and_rcv(sendBuffer2, pneles * pointByteLen, recvBuffer2, neles * pointByteLen, sock, role);
	if (0 != sign_and_verify(regPrivateKey, regPublicKey, user_id, sendBuffer2, pneles, recvBuffer2, neles, sock, role))
	{
		cout << "Error:sign & verify" << endl;
		return 0;
	}
	uint32_t intersect_size;
	uint32_t *matches = new uint32_t[sizeof(uint32_t) * min(neles, pneles)];
	uint32_t *perm = new uint32_t[sizeof(uint32_t) * neles];
	for (size_t i = 0; i < neles; ++i)
	{
		perm[i] = i;
	}
	if (role == CLIENT)
	{
		intersect_size = find_intersection(recvBuffer2, neles, sendBuffer2, pneles, pointByteLen, perm, matches);
		create_result_from_matches_var_bitlen(result, resbytelens, elebytelens, elements, matches, intersect_size);
	}

	delete[] matches;
	delete[] perm;
	for (int i = 0; i < neles; ++i)
	{
		delete[] GmulA[i];
	}
	for (int i = 0; i < pneles; ++i)
	{
		delete[] GmulB[i];
		delete[] GmulBmulA[i];
	}
	delete[] GmulA;
	delete[] GmulB;
	delete[] GmulBmulA;
	delete[] sendBuffer2;
	delete[] recvBuffer2;
	delete (Ipp8u *)pPrng;
	delete (Ipp8u *)pGfpointPmulsc;
	delete (Ipp8u *)pGfpointPmuls;
	delete (Ipp8u *)pGfpointPmulc;
	delete (Ipp8u *)pGfpoint;

	delete (Ipp8u *)pGfpec;
	delete (Ipp8u *)pGF;
	return intersect_size;
}