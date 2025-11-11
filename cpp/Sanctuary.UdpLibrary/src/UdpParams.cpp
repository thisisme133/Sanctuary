#include <Sanctuary/UdpLibrary/UdpParams.h>

namespace Sanctuary::UdpLibrary {

UdpParams::UdpParams(ManagerRole role) {
    // Default values are already initialized via member initializers in the header.
    // Override specific values based on the role.

    switch (role) {
        case ManagerRole::InternalServer:
            outgoingBufferSize = 4 * 1024 * 1024;
            incomingBufferSize = 4 * 1024 * 1024;
            crcBytes = 2;
            icmpErrorRetryPeriod = 500;
            maxRawPacketSize = 1460;
            hashTableSize = 10000;
            keepAliveDelay = 30000;
            noDataTimeout = 90000;
            maxConnections = 2000;
            pooledPacketMax = 20000;
            pooledPacketInitial = 1000;
            allowPortRemapping = false;
            reliable[0].maxInstandingPackets = 1000;
            reliable[0].maxOutstandingBytes = 1024 * 1024;
            reliable[0].maxOutstandingPackets = 1000;
            reliable[0].congestionWindowMinimum = 4 * 1024;
            reliable[0].congestionWindowMaximum = 16 * 1024;
            reliable[0].resendDelayAdjust = 150;
            break;

        case ManagerRole::InternalClient:
            outgoingBufferSize = 1024 * 1024;
            incomingBufferSize = 1024 * 1024;
            crcBytes = 2;
            icmpErrorRetryPeriod = 500;
            maxRawPacketSize = 1460;
            hashTableSize = 10;
            keepAliveDelay = 30000;
            noDataTimeout = 90000;
            maxConnections = 2;
            pooledPacketMax = 2000;
            pooledPacketInitial = 100;
            allowPortRemapping = false;
            reliable[0].maxInstandingPackets = 1000;
            reliable[0].maxOutstandingBytes = 1024 * 1024;
            reliable[0].maxOutstandingPackets = 1000;
            reliable[0].congestionWindowMinimum = 4 * 1024;
            reliable[0].congestionWindowMaximum = 16 * 1024;
            reliable[0].resendDelayAdjust = 150;
            break;

        case ManagerRole::ExternalServer:
            outgoingBufferSize = 2 * 1024 * 1024;
            incomingBufferSize = 2 * 1024 * 1024;
            crcBytes = 2;
            icmpErrorRetryPeriod = 2500;
            hashTableSize = 10000;
            keepAliveDelay = 30000;
            noDataTimeout = 90000;
            maxConnections = 2000;
            pooledPacketMax = 20000;
            pooledPacketInitial = 1000;
            break;

        case ManagerRole::ExternalClient:
            crcBytes = 2;
            icmpErrorRetryPeriod = 2500;
            hashTableSize = 10;
            keepAliveDelay = 30000;
            noDataTimeout = 90000;
            maxConnections = 2;
            pooledPacketMax = 2000;
            pooledPacketInitial = 10;
            break;

        case ManagerRole::Lfn:
            outgoingBufferSize = 16 * 1024 * 1024;
            incomingBufferSize = 16 * 1024 * 1024;
            crcBytes = 2;
            icmpErrorRetryPeriod = 1500;
            maxRawPacketSize = 1460;
            keepAliveDelay = 30000;
            noDataTimeout = 90000;
            maxConnections = 2;
            pooledPacketMax = 50000;
            pooledPacketInitial = 5000;
            allowPortRemapping = false;
            incomingLogicalPacketMax = 200 * 1024 * 1024;
            callbackEventPoolMax = 50000;
            reliable[0].maxInstandingPackets = 32000;
            reliable[0].maxOutstandingBytes = 50 * 1024 * 1024;
            reliable[0].maxOutstandingPackets = 32000;
            reliable[0].congestionWindowMinimum = 300000;
            reliable[0].congestionWindowMaximum = 300000;
            reliable[0].toleranceLossCount = 100;
            break;

        case ManagerRole::Default:
            // Use default member initializers from header
            break;
    }
}

} // namespace Sanctuary::UdpLibrary
