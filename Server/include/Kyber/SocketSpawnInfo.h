#pragma once

namespace Kyber 
{
    struct SocketSpawnInfo
    {
        SocketSpawnInfo(bool isProxied, const char* proxyAddress, const char* proxyKey)
            : isProxied(isProxied)
            , proxyAddress(proxyAddress)
            , proxyKey(proxyKey)
        {}

        bool isProxied;
        const char* proxyAddress;
        const char* proxyKey;
        const char* serverMode;
        const char* serverLevel;
    };
}