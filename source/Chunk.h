#pragma once

#include <vector>



const int c_MaxPacketSize = 512;

class Chunk
{
public:
  Chunk();
  ~Chunk();

  // Both Get & Insert increment m_CurrentPacket
  // do not call more than once per packet
  std::vector<char> GetNextPacket( void );
  void InsertPacket( std::vector<char>& new_packet );
  int GetPacketId( void );

  bool IsComplete( void );

  // Chunk Array
  std::vector<char> m_Array;

  // Self explanatory =/
  int m_TotalPackets;
  unsigned m_CurrentPacket;
};