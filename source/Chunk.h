#pragma once

#include <vector>

const int c_MaxPacketSize = 512;

class Chunk {
public:
  Chunk();
  ~Chunk();

  // Both Get & Insert increment m_CurrentPacket
  // do not call more than once per packet
  void GetNextPacket(std::vector<char>& nextPacketcopy );
  void InsertPacket( std::vector<char>& new_packet, unsigned packetNum );
  std::vector<char> GetPacket( unsigned packetNum );
  int GetPacketId( void );
  
  bool IsSendComplete( void );
  bool IsReceiveComplete( void );

  // Chunk array
  std::vector<char> m_Array;

  // For tracking placed packets
  std::vector<bool> m_InsertedArray;

  // Self explanatory =/
  int m_TotalPackets;
  int m_ReceivedPackets;
  unsigned m_CurrentPacket;
};