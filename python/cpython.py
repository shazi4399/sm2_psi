import ctypes
lib = ctypes.CDLL("../libpsi.so")
lib._Z10echo_hellov()
'''
                 w _ITM_deregisterTMCloneTable
                 w _ITM_registerTMCloneTable
                 U _Unwind_Resume
0000000000009490 T _Z10deletePRNGP7_cpPRNG
0000000000005cf0 T _Z10echo_hellov
000000000000c390 T _Z11mul_randkeyPK13_cpGFpECPoint9BigNumberPS_P8_cpGFpEC
00000000000094b0 T _Z11newPrimeGeni
00000000000076c0 T _Z11print_usageNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEP11parsing_ctxj
000000000000ae70 T _Z11strlen_safePKcm
0000000000009cd0 T _Z13IppBNStoIpp8uPK9_cpBigNum
00000000000079d0 T _Z13parse_optionsPiPPPcP11parsing_ctxj
0000000000009510 T _Z14deletePrimeGenP8_cpPrime
000000000000c2e0 T _Z14oaid_hashpointPhjP13_cpGFpECPointPvP8_cpGFpEC
000000000000c420 T _Z15sign_and_verifyPK9_cpBigNumPK13_cpGFpECPointjPhiS5_iP7CSocket
000000000000c690 T _Z15sign_and_verifyPK9_cpBigNumPK13_cpGFpECPointjPhiS5_iP7CSocket9role_type
000000000000c930 T _Z16sign_verify_initRP8_cpGFpECRP9_cpBigNumRP13_cpGFpECPointP7CSocket
0000000000008fd0 T _Z3BitRKSt6vectorIjSaIjEEi
00000000000091f0 T _Z5newBNiPKj
00000000000095c0 T _Z5printPhi
0000000000005f10 T _Z6dh_psi9role_typejjPjPPhPS2_PS0_P7CSocketj
00000000000073d0 T _Z6listenPKctP7CSocketj
0000000000009530 T _Z6newDLPii
0000000000009290 T _Z6rand32Pji
0000000000007180 T _Z7connectPKctR7CSocket
0000000000009370 T _Z7newPRNGi
0000000000009270 T _Z8deleteBNP9_cpBigNum
000000000000cb70 T _Z9GFpECinitRP6_cpGFpRP8_cpGFpECRP13_cpGFpECPointS7_S7_S7_
000000000000a510 T _Z9Ipp8u2strB5cxx11PKhi
00000000000095a0 T _Z9deleteDLPP6_cpDLP
0000000000009310 T _Z9newRandBNi
00000000000080a0 T _ZN9BigNumber3OneEv
0000000000007ff0 T _ZN9BigNumber3SetEPKji13IppsBigNumSGN
0000000000008130 T _ZN9BigNumber3TwoEv
00000000000096f0 T _ZN9BigNumber3tBNB5cxx11EPKc
0000000000008010 T _ZN9BigNumber4ZeroEv
0000000000007da0 T _ZN9BigNumber6createEPKji13IppsBigNumSGN
000000000000a0f0 T _ZN9BigNumber7toIpp8uEv
0000000000007ed0 T _ZN9BigNumberC1EPK9_cpBigNum
00000000000088d0 T _ZN9BigNumberC1EPKc
0000000000007f50 T _ZN9BigNumberC1EPKji13IppsBigNumSGN
0000000000007f70 T _ZN9BigNumberC1ERKS_
0000000000007e60 T _ZN9BigNumberC1Ei
0000000000007e30 T _ZN9BigNumberC1Ej
0000000000007ed0 T _ZN9BigNumberC2EPK9_cpBigNum
00000000000088d0 T _ZN9BigNumberC2EPKc
0000000000007f50 T _ZN9BigNumberC2EPKji13IppsBigNumSGN
0000000000007f70 T _ZN9BigNumberC2ERKS_
0000000000007e60 T _ZN9BigNumberC2Ei
0000000000007e30 T _ZN9BigNumberC2Ej
0000000000007d80 T _ZN9BigNumberD0Ev
0000000000007d50 T _ZN9BigNumberD1Ev
0000000000007d50 T _ZN9BigNumberD2Ev
00000000000081c0 T _ZN9BigNumberaSERKS_
0000000000008610 T _ZN9BigNumberdVERKS_
0000000000008310 T _ZN9BigNumbermIERKS_
00000000000083e0 T _ZN9BigNumbermLERKS_
00000000000084a0 T _ZN9BigNumbermLEj
0000000000008240 T _ZN9BigNumberpLERKS_
0000000000008580 T _ZN9BigNumberrMERKS_
0000000000008e10 T _ZNK9BigNumber10InverseAddERKS_
0000000000008be0 T _ZNK9BigNumber10InverseMulERKS_
000000000000aac0 T _ZNK9BigNumber3LSBEv
000000000000abb0 T _ZNK9BigNumber3MSBEv
0000000000008f80 T _ZNK9BigNumber5IsOddEv
0000000000008c20 T _ZNK9BigNumber6ModAddERKS_S1_
0000000000008ca0 T _ZNK9BigNumber6ModMulERKS_S1_
0000000000008eb0 T _ZNK9BigNumber6ModSubERKS_S1_
0000000000008b90 T _ZNK9BigNumber6ModuloERKS_
0000000000008d20 T _ZNK9BigNumber7compareERKS_
0000000000008ff0 T _ZNK9BigNumber7num2hexERNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
000000000000aa10 T _ZNK9BigNumber7num2vecERSt6vectorIjSaIjEE
                 U _ZNKSt5ctypeIcE13_M_widen_initEv
0000000000007060 W _ZNKSt5ctypeIcE8do_widenEc
                 U _ZNSo3putEc
                 U _ZNSo5flushEv
                 U _ZNSo9_M_insertImEERSoT_
                 U _ZNSolsEi
                 U _ZNSt6localeC1Ev
                 U _ZNSt6localeD1Ev
000000000000ad40 W _ZNSt6vectorIjSaIjEE17_M_realloc_insertIJRKjEEEvN9__gnu_cxx17__normal_iteratorIPjS1_EEDpOT_
                 U _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_replaceEmmPKcm
                 U _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE14_M_replace_auxEmmmc
                 U _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERmm
000000000000acf0 W _ZNSt7__cxx1115basic_stringbufIcSt11char_traitsIcESaIcEED0Ev
000000000000acb0 W _ZNSt7__cxx1115basic_stringbufIcSt11char_traitsIcESaIcEED1Ev
000000000000acb0 W _ZNSt7__cxx1115basic_stringbufIcSt11char_traitsIcESaIcEED2Ev
                 U _ZNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEED1Ev
                 U _ZNSt8ios_base4InitC1Ev
                 U _ZNSt8ios_base4InitD1Ev
                 U _ZNSt8ios_baseC2Ev
                 U _ZNSt8ios_baseD2Ev
                 U _ZNSt9basic_iosIcSt11char_traitsIcEE4initEPSt15basic_streambufIcS1_E
                 U _ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate
0000000000007070 W _ZSt13__adjust_heapIPjljN9__gnu_cxx5__ops15_Iter_less_iterEEvT_T0_S5_T1_T2_
                 U _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
                 U _ZSt16__throw_bad_castv
                 U _ZSt19__throw_logic_errorPKc
                 U _ZSt4cerr
                 U _ZSt4cout
                 U _ZStrsIcSt11char_traitsIcESaIcEERSt13basic_istreamIT_T0_ES7_RNSt7__cxx1112basic_stringIS4_S5_T1_EE
0000000000010d30 V _ZTI9BigNumber
000000000000d340 V _ZTS9BigNumber
                 U _ZTTNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE
0000000000010d40 V _ZTV9BigNumber
                 U _ZTVN10__cxxabiv117__class_type_infoE
                 U _ZTVNSt7__cxx1115basic_stringbufIcSt11char_traitsIcESaIcEEE
                 U _ZTVNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEE
                 U _ZTVSt15basic_streambufIcSt11char_traitsIcEE
                 U _ZTVSt9basic_iosIcSt11char_traitsIcEE
                 U _ZdaPv
                 U _ZdlPv
                 U _ZdlPvm
0000000000008ac0 T _ZdvRK9BigNumberS1_
0000000000008df0 T _ZeqRK9BigNumberS1_
0000000000008dd0 T _ZgtRK9BigNumberS1_
0000000000009120 T _ZlsRSoRK9BigNumber
0000000000008db0 T _ZltRK9BigNumberS1_
0000000000008750 T _ZmiRK9BigNumberS1_
00000000000087d0 T _ZmlRK9BigNumberS1_
0000000000008850 T _ZmlRK9BigNumberj
                 U _Znam
0000000000008f60 T _ZneRK9BigNumberS1_
                 U _Znwm
00000000000086d0 T _ZplRK9BigNumberS1_
0000000000008b40 T _ZrmRK9BigNumberS1_
                 U __assert_fail
0000000000011541 B __bss_start
                 U __cxa_atexit
                 w __cxa_finalize
                 U __cxa_guard_abort
                 U __cxa_guard_acquire
                 U __cxa_guard_release
                 U __cxa_throw_bad_array_new_length
                 U __errno_location
                 w __gmon_start__
                 U __gxx_personality_v0
                 U __stack_chk_fail
0000000000011541 D _edata
00000000000115c8 B _end
000000000000cd94 T _fini
0000000000005000 T _init
                 U accept
                 U bind
                 U close
                 U connect
                 U exit
                 U free
                 U g_hash_table_insert
                 U g_hash_table_lookup_extended
                 U g_hash_table_new_full
                 U g_int64_equal
                 U g_int64_hash
                 U gethostbyname
                 U inet_addr
                 U ippsAdd_BN
                 U ippsBigNumGetSize
                 U ippsBigNumInit
                 U ippsCmpZero_BN
                 U ippsDLPGetSize
                 U ippsDLPInit
                 U ippsDiv_BN
                 U ippsECCPGenKeyPair
                 U ippsECCPGetPoint
                 U ippsECCPGetSize
                 U ippsECCPInit
                 U ippsECCPPointGetSize
                 U ippsECCPPointInit
                 U ippsECCPPublicKey
                 U ippsECCPSetKeyPair
                 U ippsECCPSetPoint
                 U ippsECCPSetStd
                 U ippsECCPSignSM2
                 U ippsECCPVerifySM2
                 U ippsGFpECGetPointOctString
                 U ippsGFpECGetSize
                 U ippsGFpECInit
                 U ippsGFpECMulPoint
                 U ippsGFpECPointGetSize
                 U ippsGFpECPointInit
                 U ippsGFpECScratchBufferSize
                 U ippsGFpECSetPointHash_rmf
                 U ippsGFpECSetPointOctString
                 U ippsGFpElementGetSize
                 U ippsGFpElementInit
                 U ippsGFpGetSize
                 U ippsGFpInit
                 U ippsGFpMethod_p256sm2
                 U ippsGFpSetElementOctString
                 U ippsGetOctString_BN
                 U ippsGetSize_BN
                 U ippsGet_BN
                 U ippsHashFinal_rmf
                 U ippsHashGetSize_rmf
                 U ippsHashInit_rmf
                 U ippsHashMethod_SM3
                 U ippsHashUpdate_rmf
                 U ippsModInv_BN
                 U ippsMod_BN
                 U ippsMul_BN
                 U ippsPRNGGetSize
                 U ippsPRNGInit
                 U ippsPRNGSetAugment
                 U ippsPRNGSetSeed
                 U ippsPRNGen
                 U ippsPrimeGetSize
                 U ippsPrimeInit
                 U ippsRef_BN
                 U ippsSetOctString_BN
                 U ippsSet_BN
                 U ippsSub_BN
                 U listen
                 U malloc
                 U memcpy
                 U memmove
                 U perror
                 U pthread_create
                 U pthread_join
                 U puts
                 U rand
                 U recv
                 U send
                 U setsockopt
                 U shutdown
                 U socket
                 U srand
                 U strcspn
                 U strlen
                 U strtod
                 U strtol
                 U usleep
'''