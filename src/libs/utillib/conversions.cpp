/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 2017 by Great Hill Corporation.
 * All Rights Reserved
 *
 * The LICENSE at the root of this repo details your rights (if any)
 *------------------------------------------------------------------------*/
#include "biglib.h"
#include "conversions.h"

namespace qblocks {

    //------------------------------------------------------------------
    class SFBloomHex : public SFBigNumStore<unsigned char> {
    public:
        SFBloomHex(const SFUintBN& numIn);
        SFString str;
    };

#define OLD_CODE 1
    //------------------------------------------------------------------
    SFBloomHex::SFBloomHex(const SFUintBN& numIn) {

        len = 1024;
        allocate(1024);

        SFUintBN x2(numIn);
        unsigned int nDigits = 0;
        while (x2.len != 0)
        {
            SFUintBN lastDigit(x2);
            lastDigit.divide(16, x2);
            blk[nDigits] = (unsigned char)lastDigit.to_ushort();
            nDigits++;
        }
        len = nDigits;

#ifdef OLD_CODE
        char s[1024+1];
		memset(s,'\0',sizeof(s));
        for (unsigned int p=0;p<len;p++) {
            unsigned short c = blk[len-1-p];
            s[p] = ((c < 10) ? char('0'+c) : char('A'+c-10));
        }
        str = s;
#else
        str.reserve(1025);
        char *pStr = (char*)(const char*)str.c_str();
        memset(pStr,'\0',1025);
        for (unsigned int p = 0 ; p < len ; p++) {
            unsigned short c = blk[len-1-p];
            pStr[p] = ((c < 10) ? char('0'+c) : char('a'+c-10));
        }
#endif
    }

    //------------------------------------------------------------------
    SFString bloom2Bytes(const SFBloom& bl) {
        if (bl == 0)
            return "0x0";
        SFBloomHex b2(bl);
#ifdef OLD_CODE
        return ("0x" + padLeft(toLower(b2.str),512,'0'));
#else
        return ("0x" + padLeft(b2.str,512,'0'));
#endif
    }

    //-------------------------------------------------------------------------
    SFString bloom2Bits(const SFBloom& b) {
        SFString ret = bloom2Bytes(b).Substitute("0x", "");
        ret.ReplaceAll("0","0000");
        ret.ReplaceAll("1","0001");
        ret.ReplaceAll("2","0010");
        ret.ReplaceAll("3","0011");
        ret.ReplaceAll("4","0100");
        ret.ReplaceAll("5","0101");
        ret.ReplaceAll("6","0110");
        ret.ReplaceAll("7","0111");
        ret.ReplaceAll("8","1000");
        ret.ReplaceAll("9","1001");
        ret.ReplaceAll("a","1010");
        ret.ReplaceAll("b","1011");
        ret.ReplaceAll("c","1100");
        ret.ReplaceAll("d","1101");
        ret.ReplaceAll("e","1110");
        ret.ReplaceAll("f","1111");
        return ret;
    }

    //----------------------------------------------------------------------------------------------------
    timestamp_t toTimestamp(const SFString& timeIn) {
        return (timeIn.startsWith("0x") ? (timestamp_t)hex2Long(timeIn) : (timestamp_t)toLong(timeIn));
    }

    //----------------------------------------------------------------------------------------------------
    timestamp_t toTimestamp(const SFTime& timeIn) {
        SFTime  jan1970(1970, 1, 1, 0, 0, 0);
        if (timeIn < jan1970)
            return 0;

        int64_t j70 = jan1970.GetTotalSeconds();
        int64_t t   = timeIn.GetTotalSeconds();
        return (t - j70);
    }

    //----------------------------------------------------------------------------------------------------
    SFTime dateFromTimeStamp(timestamp_t tsIn) {
        time_t utc = tsIn;
        tm unused;
        struct tm *ret = gmtime_r(&utc, &unused);

        char retStr[40];
        strftime(retStr, sizeof(retStr), "%Y-%m-%d %H:%M:%S UTC", ret);

        SFString str = retStr;
        uint32_t y = toLong32u(nextTokenClear(str, '-'));
        uint32_t m = toLong32u(nextTokenClear(str, '-'));
        uint32_t d = toLong32u(nextTokenClear(str, ' '));
        uint32_t h = toLong32u(nextTokenClear(str, ':'));
        uint32_t mn = toLong32u(nextTokenClear(str, ':'));
        uint32_t s = toLong32u(nextTokenClear(str, ' '));
        return SFTime(y, m, d, h, mn, s);
    }

    /*
        Javascipt: Returns a checksummed address
        @param {String} address
        @return {String}
        exports.toChecksumAddress = function (address) {
            address = exports.stripHexPrefix(address).toLowerCase()
            const hash = exports.sha3(address).toString('hex')
            let ret = '0x'
            for (let i = 0; i < address.length; i++) {
                if (parseInt(hash[i], 16) >= 8) {
                    ret += address[i].toUpperCase()
                } else {
                    ret += address[i]
                }
            }
            return ret
        }
    */
}  // namespace qblocks
