#include <assert.h>
#include <unistd.h>

#include "tc_uuid.h"

namespace taf
{

char myPid[2];

class PidInitializer
{
public:
    
    PidInitializer()
    {
        pid_t p = getpid();
        myPid[0] = (p >> 8) & 0x7F;
        myPid[1] = p & 0xFF;
    }
};

PidInitializer pidInitializer;

inline void halfByteToHex(unsigned char hb, char*& hexBuffer)
{
    if(hb < 10)
    {
        *hexBuffer++ = '0' + hb;
    }
    else
    {
        *hexBuffer++ = 'A' + (hb - 10);
    } 
}

inline void bytesToHex(unsigned char* bytes, size_t len, char*& hexBuffer)
{
    for(size_t i = 0; i < len; i++)
    {
        halfByteToHex((bytes[i] & 0xF0) >> 4, hexBuffer);
        halfByteToHex((bytes[i] & 0x0F), hexBuffer);
    }
}

std::string generateUUID()
{
    struct UUID
    {
        unsigned char timeLow[4];
        unsigned char timeMid[2];
        unsigned char timeHighAndVersion[2];
        unsigned char clockSeqHiAndReserved;
        unsigned char clockSeqLow;
        unsigned char node[6];
    };
    UUID uuid;

    assert(sizeof(UUID) == 16);

    // 
    // Get a random sequence of bytes. Instead of using 122 random
    // bits that could be duplicated (because of a bug with some Linux
    // kernels and potentially other Unix platforms -- see comment in
    // Random.cpp), we replace the last 15 bits of all "random"
    // Randoms by the last 15 bits of the process id.
    //
    char* buffer = reinterpret_cast<char*>(&uuid);
    generateRandom(buffer, static_cast<int>(sizeof(UUID)));

    //
    // Adjust the bits that say "version 4" UUID
    //
    uuid.timeHighAndVersion[0] &= 0x0F;
    uuid.timeHighAndVersion[0] |= (4 << 4);
    uuid.clockSeqHiAndReserved &= 0x3F;
    uuid.clockSeqHiAndReserved |= 0x80;

    //
    // Replace the end of the node by myPid (15 bits) 
    //
    uuid.node[4] = (uuid.node[4] & 0x80) | myPid[0];
    uuid.node[5] = myPid[1];

    //
    // Convert to a UUID string
    //
    char uuidString[16 * 2 + 4 + 1]; // 16 bytes, 4 '-' and a final '\0'
    char* uuidIndex = uuidString;
    bytesToHex(uuid.timeLow, sizeof(uuid.timeLow), uuidIndex);
//    *uuidIndex++ = '-';
    bytesToHex(uuid.timeMid, sizeof(uuid.timeMid), uuidIndex);
//    *uuidIndex++ = '-';
    bytesToHex(uuid.timeHighAndVersion, sizeof(uuid.timeHighAndVersion), uuidIndex);
//    *uuidIndex++ = '-';
    bytesToHex(&uuid.clockSeqHiAndReserved, sizeof(uuid.clockSeqHiAndReserved), uuidIndex);
    bytesToHex(&uuid.clockSeqLow, sizeof(uuid.clockSeqLow), uuidIndex);
//    *uuidIndex++ = '-';
    bytesToHex(uuid.node, sizeof(uuid.node), uuidIndex);
    *uuidIndex = '\0';

    return uuidString;
}

}
