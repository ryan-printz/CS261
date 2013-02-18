
#include "FileFrame.h"
#include <iostream>

FileFrame::FileFrame() : m_CurrentChunk(-1) , m_TotalChunks(0)
{
}

void FileFrame::LoadChunk(std::string& filename )
{
//open file, seek to end to find file length
FILE* loader = fopen(filename.c_str(), "rb");
    
int filesize = getFileLength(filename);
// int filesize = getFileLength(filename);

int chunkSize = 0;
//if size of file is less than size of chunk
if(filesize < c_MaxChunkSize)
    chunkSize  = filesize; //only use size of this file
else
    chunkSize  = c_MaxChunkSize;

m_TotalChunks = (filesize/chunkSize) + (filesize%chunkSize != 0);
m_Chunk.m_TotalPackets = chunkSize/c_MaxPacketSize + (chunkSize % c_MaxPacketSize != 0);


fseek(loader, m_CurrentChunk*chunkSize, SEEK_SET);
   
char* readBuf = new char[chunkSize];
int bytes_read = fread((void*)readBuf, sizeof(char), chunkSize, loader);

m_Chunk.m_Array.clear();
for(int i = 0; i < bytes_read; ++i )
{
m_Chunk.m_Array.push_back(readBuf[i]); 
}
fclose(loader);




m_CurrentChunk++;
}

void FileFrame::WriteChunk(std::string filename )
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

bool FileFrame::IsComplete( void )
{
return (m_CurrentChunk == m_TotalChunks); 
}

int FileFrame::GetChunkId( void )
{
return m_CurrentChunk;
}
