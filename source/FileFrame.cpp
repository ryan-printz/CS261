
#include "FileFrame.h"
#include <iostream>

FileFrame::FileFrame() : m_CurrentChunk(0) , m_TotalChunks(0), m_Ready(false)
{
}

void FileFrame::LoadChunk(std::string& filename )
{
    //open file, seek to end to find file length
    FILE* loader = fopen(filename.c_str(), "rb");
    
    int filesize = getFileLength(filename);

    int chunkSize = 0;
    if(filesize < c_MaxChunkSize)
        chunkSize  = filesize; 
    else
        chunkSize  = c_MaxChunkSize;

    m_TotalChunks = (filesize/chunkSize) + (filesize%chunkSize == 0 ? 0 : 1);

    int packetSize = 0;
    if (chunkSize < c_MaxPacketSize)
        packetSize = chunkSize;
    else
        packetSize = c_MaxPacketSize;
        
    m_Chunk.m_TotalPackets = chunkSize/packetSize + (chunkSize % packetSize == 0 ? 0 : 1);


    fseek(loader, m_CurrentChunk*chunkSize, SEEK_SET);
   
    char* readBuf = new char[chunkSize];
    int bytes_read = fread((void*)readBuf, sizeof(char), chunkSize, loader);

    m_Chunk.m_Array.clear();
    for(int i = 0; i < bytes_read; ++i )
    {
        m_Chunk.m_Array.push_back(readBuf[i]); 
    }
    fclose(loader);
    ++m_CurrentChunk;
	this->m_Chunk.m_CurrentPacket = -1;
}

void FileFrame::WriteChunk(std::string & filename )
{
    //write entire chunk to end of buffer
    FILE* writeFile = fopen(filename.c_str(), "ab");
    
    if( ! writeFile){
        std::cout << "bad filename! write chunk failed." << std::endl;
        return;}

    for(int i = 0; i < m_Chunk.m_Array.size(); ++i)
    {
        putc(m_Chunk.m_Array[i], writeFile);  
    } 
    fclose(writeFile);
    m_CurrentChunk++;
}

int FileFrame::getFileLength( std::string &filename )
{
    FILE* loader = fopen(filename.c_str(), "rb");
    fseek(loader, 0, SEEK_END);
    return ftell(loader);
}

bool FileFrame::IsFinalChunk( void )
{
    return (m_CurrentChunk == m_TotalChunks); 
}

int FileFrame::GetChunkId( void )
{
    return m_CurrentChunk;
}
