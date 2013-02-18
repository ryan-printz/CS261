#include "Chunk.h"

Chunk::Chunk()
    : m_CurrentPacket(-1)
    , m_TotalPackets(0)
{

}

Chunk::~Chunk()
{

}

std::vector<char> Chunk::GetNextPacket( void )
{
    int packetSize = 0;

    if ( m_Array.size() < c_MaxPacketSize )
        packetSize = m_Array.size();

    else
        packetSize = c_MaxPacketSize;

    std::vector<char> nextPacketcopy;

    for(int packetIt = 0 ;packetIt < packetSize; ++packetIt)
    {
        nextPacketcopy.push_back(m_Array[packetIt]);  
    }
    m_CurrentPacket++;

    return nextPacketcopy;
}

void Chunk::InsertPacket( std::vector<char>& new_packet )
{
    for each(auto newP in new_packet)
        m_Array.push_back(newP);
    m_CurrentPacket++;
}

bool Chunk::IsComplete( void )
{
    return (m_CurrentPacket == m_TotalPackets-1);
}

int Chunk::GetPacketId( void )
{
    return m_CurrentPacket;
}

