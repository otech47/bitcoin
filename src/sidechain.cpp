// Copyright (c) 2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "sidechain.h"

#include "clientversion.h"
#include "core_io.h"
#include "hash.h"
#include "streams.h"
#include "utilmoneystr.h"

#include <sstream>

const uint32_t nType = 1;
const uint32_t nVersion = 1;

uint256 SidechainObj::GetHash(void) const
{
    uint256 ret;
    if (sidechainop == 'W')
        ret = SerializeHash(*(SidechainWT *) this);
    else
    if (sidechainop == 'J')
        ret = SerializeHash(*(SidechainWTJoin *) this);
    else
    if (sidechainop == 'D')
        ret = SerializeHash(*(SidechainDeposit *) this);

    return ret;
}

CScript SidechainObj::GetScript(void) const
{
    CDataStream ds (SER_DISK, CLIENT_VERSION);
    if (sidechainop == 'W')
        ((SidechainWT *) this)->Serialize(ds);
    else
    if (sidechainop == 'J')
        ((SidechainWTJoin *) this)->Serialize(ds);
    else
    if (sidechainop == 'D')
        ((SidechainDeposit *) this)->Serialize(ds);

    CScript script;
    script << std::vector<unsigned char>(ds.begin(), ds.end()) << OP_SIDECHAIN;
    return script;
}

SidechainObj *SidechainObjCtr(const CScript &script)
{
    CScript::const_iterator pc = script.begin();
    std::vector<unsigned char> vch;
    opcodetype opcode;

    if (!script.GetOp(pc, opcode, vch))
        return NULL;
    if (vch.size() == 0)
        return NULL;
    const char *vch0 = (const char *) &vch.begin()[0];
    CDataStream ds(vch0, vch0+vch.size(), SER_DISK, CLIENT_VERSION);

    if (*vch0 == 'W') {
        SidechainWT *obj = new SidechainWT;
        obj->Unserialize(ds);
        return obj;
    }
    else
    if (*vch0 == 'J') {
        SidechainWTJoin *obj = new SidechainWTJoin;
        obj->Unserialize(ds);
        return obj;
    }
    else
    if (*vch0 == 'D') {
        SidechainDeposit *obj = new SidechainDeposit;
        obj->Unserialize(ds);
        return obj;
    }
    return NULL;
}

std::string SidechainObj::ToString(void) const
{
    std::stringstream str;
    str << "sidechainop=" << sidechainop << std::endl;
    str << "nHeight=" << nHeight << std::endl;
    str << "txid=" << txid.GetHex() << std::endl;
    return str.str();
}

std::string SidechainWT::ToString() const
{
    std::stringstream str;
    str << "sidechainop=" << sidechainop << std::endl;
    str << "nHeight=" << nHeight << std::endl;
    str << "txid=" << txid.GetHex() << std::endl;
    return str.str();
}

std::string SidechainWTJoin::ToString() const
{
    std::stringstream str;
    str << "sidechainop=" << sidechainop << std::endl;
    str << "nHeight=" << nHeight << std::endl;
    str << "txid=" << txid.GetHex() << std::endl;
    return str.str();
}

std::string SidechainDeposit::ToString() const
{
    std::stringstream str;
    str << "sidechainop=" << sidechainop << std::endl;
    str << "nSidechain=" << (unsigned int)nSidechain << std::endl;
    str << "txid=" << txid.GetHex() << std::endl;
    str << "payout=" << FormatMoney(amtUserPayout) << std::endl;
    str << "dtxid=" << dtx.GetHash().ToString() << std::endl;
    return str.str();
}
