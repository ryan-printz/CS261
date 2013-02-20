#include <Windows.h>
#include <string>
#include <Strsafe.h>

std::string getFilenameList(const char* cpath)
{
	char dirName[MAX_PATH];
	std::string path(cpath);
    path.append("*");

	size_t pathLength;
	StringCchLengthA(path.c_str(), MAX_PATH, &pathLength);
	
	if(pathLength > MAX_PATH)
	{
	printf("File path is too long.\n");
	return std::string("");
	}
	printf("Reading files names in path: %s\n", path.c_str());
	  
	StringCchCopyA(dirName, MAX_PATH, path.c_str() );
	StringCchCatA(dirName, MAX_PATH, "/*");
	
	WIN32_FIND_DATAA fileInfo;
	HANDLE findFile = FindFirstFileA(path.c_str(), &fileInfo);
	  
	if(    findFile == INVALID_HANDLE_VALUE)
	{
        printf("FindFirstFile in path %s failed\n", path.c_str());
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

            printf("Sharing file %s", fileName.c_str());

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
