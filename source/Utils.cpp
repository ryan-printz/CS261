#include <Windows.h>
#include <string>
#include <Strsafe.h>

std::string getFilenameList(const char* path)
{
	char dirName[MAX_PATH];
	
	size_t pathLength;
	StringCchLengthA(path, MAX_PATH, &pathLength);
	
	if(pathLength > MAX_PATH)
	{
	printf("File path is too long.\n");
	return std::string("");
	}
	printf("Reading files names in path: %s\n", path);
	  
	StringCchCopyA(dirName, MAX_PATH, path );
	StringCchCatA(dirName, MAX_PATH, "/*");
	
	WIN32_FIND_DATAA fileInfo;
	HANDLE findFile = FindFirstFileA(path, &fileInfo);
	  
	if(    findFile == INVALID_HANDLE_VALUE)
	{
	printf("FindFirstFile in path %s failed\n", path);
	return std::string("");
	}
	std::string fileNameList;
	std::string fileName;
	do 
	{
        if(!(fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            fileName = fileInfo.cFileName;
            fileName += "\n";

            if(fileNameList.find(fileName) == std::string::npos)
                fileNameList  +=  fileName;
        }
	} while (FindNextFileA(findFile, &fileInfo));
	return fileNameList;
}
	
std::string getword(std::string& message)
{
	std::string word;
	size_t endword = message.find_first_of(" \n");
	
	if(	 endword == std::string::npos)
	{
	return message;
	}
	word =   message.substr(0, endword);
	message.erase (0, endword+1);
	
	return word;
}
