/*
 * crypto.h
 *
 *  Created on: Jun 28, 2022
 *      Author: tcoole
 */

#include "../typedefs.h"
#include "../connection.h"
#include "../helpers.h"
#include <ippcp.h> /* ipp library */
#include "../bigNum/bignum.h"
#include "../bigNum/common.h"
#include "../bigNum/utils.h"
// sm2:
const Ipp8u a[] = "\xFF\xFF\xFF\xFE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFC";
const Ipp8u b[] = "\x28\xE9\xFA\x9E\x9D\x9F\x5E\x34\x4D\x5A\x9E\x4B\xCF\x65\x09\xA7\xF3\x97\x89\xF5\x15\xAB\x8F\x92\xDD\xBC\xBD\x41\x4D\x94\x0E\x93";
const Ipp8u mp[] = "\xFF\xFF\xFF\xFE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
// 256r1:
// const Ipp8u a[] = "\xFF\xFF\xFF\xFF\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFC";
// const Ipp8u b[] = "\x5A\xC6\x35\xD8\xAA\x3A\x93\xE7\xB3\xEB\xBD\x55\x76\x98\x86\xBC\x65\x1D\x06\xB0\xCC\x53\xB0\xF6\x3B\xCE\x3C\x3E\x27\xD2\x60\x4B";
// mp "0xFFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF";//
// crypto.cpp:ippsGFpInit(P, 256, ippsGFpMethod_p256sm2(), pGF);//or ippsGFpMethod_p256r1()
const uint32_t bnBitSize = 256;
const uint32_t ecPar = 256;							 // global size param(bit)
const uint32_t pointByteLen = ecPar / 8 * 2;		 // point (x,y) size of ec(byte)
const uint32_t halfPointByteLen = ecPar / 8 * 2 / 2; // half-point (x)or(y) size of ec(byte)
const uint32_t idLen = 5;
const unsigned int order[] = {0x39D54123, 0x53BBF409, 0x21C6052B, 0x7203DF6B, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE};
const int ordSize = sizeof(order) / sizeof(unsigned int);

void oaid_hashpoint(Ipp8u *oaid, uint32_t len, IppsGFpECPoint *pGfpoint, void *pCtx, IppsGFpECState *pGfpEC);
void mul_randkey(const IppsGFpECPoint *pGfpSrc, BigNumber pRrd, IppsGFpECPoint *pGfpDst, IppsGFpECState *pGfpEC);
static IppsECCPState *new_ECC_sm2(void);
static IppsECCPPointState *new_ECC_Point(void);
static int hash_digest_z(const IppsHashMethod *hash_method, const Ipp8u *id, const int id_len, const IppsBigNumState *pubX, const IppsBigNumState *pubY, unsigned char *z_digest);
static int hash_digest_with_preprocess(const IppsHashMethod *hash_method, const Ipp8u *msg, const int msg_len, const Ipp8u *id, const int id_len, const IppsBigNumState *pubX, const IppsBigNumState *pubY, unsigned char *digest);
static int sm2_do_sign(const IppsBigNumState *regPrivateKey, const IppsHashMethod *hash_method, Ipp8u *id, int id_len, Ipp8u *msg, const int msg_len, IppsBigNumState *signX, IppsBigNumState *signY);
static int sm2_do_verify(const IppsECCPPointState *regPublicKey, const IppsHashMethod *hash_method, Ipp8u *id, const int id_len, Ipp8u *msg, const int msg_len, IppsBigNumState *signX, IppsBigNumState *signY);
int sign_and_verify(const IppsBigNumState *regPrivateKey, const IppsECCPPointState *regPublicKey,
					uint32_t user_id, Ipp8u *sendBuffer, int neles, Ipp8u *recvBuffer, int pneles, CSocket *sock);
int sign_and_verify(const IppsBigNumState *regPrivateKey, const IppsECCPPointState *regPublicKey,
					uint32_t user_id, Ipp8u *sendBuffer, int neles, Ipp8u *recvBuffer, int pneles, CSocket *sock, role_type role);
int sign_verify_init(IppsECCPState *&pECCPS, IppsBigNumState *&regPrivateKey, IppsECCPPointState *&regPublicKey, CSocket *sock);
void GFpECinit(IppsGFpState *&pGF, IppsGFpECState *&pGfpec, IppsGFpECPoint *&pGfpoint, IppsGFpECPoint *&pGfpointPmulc, IppsGFpECPoint *&pGfpointPmuls, IppsGFpECPoint *&pGfpointPmulsc);