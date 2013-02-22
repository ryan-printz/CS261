#pragma once

#include <vector>

const int c_MaxPacketSize = 256;

class Chunk {
public:
  Chunk();
  ~Chunk();

  // Both Get & Insert increment m_CurrentPacket
  // do not call more than once per packet
  std::vector<char> GetNextPacket( void );
  void InsertPacket( std::vector<char>& new_packet, unsigned packetNum );
  std::vector<char> GetPacket( unsigned packetNum );
  int GetPacketId( void );
  

  bool IsComplete( void );

  // Chunk array
  std::vector<char> m_Array;

  // For tracking placed packets
  std::vector<bool> m_InsertedArray;

  // Self explanatory =/
  int m_TotalPackets;
  unsigned m_CurrentPacket;
};