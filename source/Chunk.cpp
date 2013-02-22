#include "Chunk.h"

Chunk::Chunk()
    : m_CurrentPacket(-1)
    , m_TotalPackets(0)
    , m_InsertedArray(false)
{
}

Chunk::~Chunk()
{}

std::vector<char> Chunk::GetNextPacket( void )
{
    ++m_CurrentPacket;
    int packetSize = 0;

    if ( (m_Array.size() - (m_CurrentPacket*c_MaxPacketSize)) < c_MaxPacketSize )
        packetSize = m_Array.size() - (m_CurrentPacket*c_MaxPacketSize);

    else
        packetSize = c_MaxPacketSize;

    std::vector<char> nextPacketcopy;

    for(int packetIt = 0; packetIt < packetSize; ++packetIt)
    {
        nextPacketcopy.push_back(m_Array[packetIt + (m_CurrentPacket*c_MaxPacketSize)]);  
    }

    return nextPacketcopy;
}

std::vector<char> Chunk::GetPacket( unsigned packetNum ) {
    int packetSize = 0;

    if ( (m_Array.size() - (packetNum*c_MaxPacketSize)) < c_MaxPacketSize )
        packetSize = m_Array.size();

    else
        packetSize = c_MaxPacketSize;

    std::vector<char> nextPacketcopy;

    for(int packetIt = 0; packetIt < packetSize; ++packetIt)
    {
        nextPacketcopy.push_back(m_Array[packetIt + (packetNum*c_MaxPacketSize)]);  
    }

    return nextPacketcopy;
}

void Chunk::InsertPacket( std::vector<char>& new_packet, unsigned packetNum )
{
    unsigned offset = packetNum * c_MaxPacketSize;

    for (unsigned i = 0; i < new_packet.size(); ++i) {
        m_Array[i+offset] = new_packet[i];
    }

    m_InsertedArray[packetNum] = true;
}

bool Chunk::IsComplete( void )
{
    return (m_CurrentPacket == m_TotalPackets-1);
}

int Chunk::GetPacketId( void )
{
    return m_CurrentPacket;
}

