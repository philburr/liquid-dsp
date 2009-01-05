//
// FEC, etc.
//

#ifndef __LIQUID_FEC_H__
#define __LIQUID_FEC_H__

#include <stdbool.h>

#define FEC_CONCAT(prefix,name) prefix ## name

#define LIQUID_FEC_MANGLE_CRC32(name)       FEC_CONCAT(fec_crc32,name)
#define LIQUID_FEC_MANGLE_CHECKSUM32(name)  FEC_CONCAT(fec_checksum32,name)

// Macro: crc/checksum
//  CRC : name-mangling macro
#define LIQUID_CRC_DEFINE_API(CRC) \
typedef struct CRC(_s) * CRC(); \
CRC() CRC(_create)(); \
void CRC(_destroy)(CRC() _crc); \
void CRC(_print)(CRC() _crc); \
void CRC(_generate_key)(CRC() _crc, unsigned char * _msg, unsigned int _msg_len); \
bool CRC(_validate_key)(CRC() _crc, unsigned char * _msg, unsigned int _msg_len);

LIQUID_CRC_DEFINE_API(LIQUID_FEC_MANGLE_CRC32)
LIQUID_CRC_DEFINE_API(LIQUID_FEC_MANGLE_CHECKSUM32)

//
// checksum
//
unsigned char checksum_generate_key(unsigned char *_data, unsigned int _n);
bool checksum_validate_message(unsigned char *_data, unsigned int _n, unsigned char _key);

//
// CRC
//
//void crc_generate_key(unsigned char *_msg, unsigned int _msg_len, unsigned char *_key, unsigned int _key_len);
unsigned int crc32_generate_key(unsigned char *_data, unsigned int _n);
bool crc32_validate_message(unsigned char *_data, unsigned int _n, unsigned int _key);

//
// 1/2-rate (8,4) Hamming code
//
void fec_hamming84_encode(unsigned char *_msg_dec, unsigned int _msg_len, unsigned char *_msg_enc);
unsigned int
     fec_hamming84_decode(unsigned char *_msg_enc, unsigned int _msg_len, unsigned char *_msg_dec);

void fec_hamming74_encode(unsigned char *_msg_dec, unsigned int _msg_len, unsigned char *_msg_enc);
unsigned int
     fec_hamming74_decode(unsigned char *_msg_enc, unsigned int _msg_len, unsigned char *_msg_dec);

#define LIQUID_FEC_MANGLE_REP3(name)        FEC_CONCAT(fec_rep3,name)
#define LIQUID_FEC_MANGLE_HAMMING74(name)   FEC_CONCAT(fec_hamming74,name)

// Macro: fec codec
//  FEC :   name-mangling macro
//
#define LIQUID_FEC_CODEC_DEFINE_API(FEC) \
typedef struct FEC(_s) * FEC(); \
unsigned int FEC(_get_enc_msg_length)(unsigned int _msg_len); \
float FEC(_get_rate)(); \
FEC() FEC(_create)(unsigned int _enc_len, void* _opts); \
void FEC(_destroy)(FEC() _fec); \
void FEC(_print)(FEC() _fec); \
void FEC(_encode)(FEC() _fec, unsigned char * _msg_dec, unsigned char * _msg_enc); \
void FEC(_decode)(FEC() _fec, unsigned char * _msg_enc, unsigned char * _msg_dec);

LIQUID_FEC_CODEC_DEFINE_API(LIQUID_FEC_MANGLE_REP3)
//LIQUID_FEC_CODEC_DEFINE_API(LIQUID_FEC_MANGLE_HAMMING74)

#endif // __LIQUID_FEC_H__

