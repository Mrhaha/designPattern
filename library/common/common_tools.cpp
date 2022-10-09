#include "common_tools.h"

#include <sys/types.h>
#include <sys/sendfile.h>
#include <dirent.h>
#include <execinfo.h>
#include <cstring>
#include <sstream>
#include <thread>
#include "common_datetime.h"


void create_pid_file(const char* pszName)
{
	assert(nullptr != pszName);
	FILE* fp = fopen(pszName, "w");
	if (nullptr == fp)
	{
		return;
	}
	const pid_t pid = getpid();
	fprintf(fp, "%d", pid);
	fclose(fp);
}

void delete_pid_file(const char* pszName)
{
	assert(nullptr != pszName);
	unlink(pszName);
}

bool create_directory(const char* pszPath)
{
	DIR* pstDir = opendir(pszPath);
	if (nullptr == pstDir)
	{
		if (mkdir(pszPath, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0)
		{
			return false;
		}
	}
	else
	{
		closedir(pstDir);
	}
	return true;
}

bool copy_file(const char* source, const char* destination)
{
	int input, output;
	if ((input = open(source, O_RDONLY)) == -1)
	{
		return false;
	}
	if ((output = open(destination, O_RDWR | O_CREAT, 0644)) == -1)
	{
		close(input);
		return false;
	}

	off_t bytesCopied = 0;
	struct stat fileinfo;
	memset(&fileinfo, 0, sizeof(fileinfo));
	fstat(input, &fileinfo);
	int result = sendfile(output, input, &bytesCopied, fileinfo.st_size);

	close(input);
	close(output);

	return result != -1;
}

void InitDaemon(int8_t nochdir, int8_t noclose)
{
	daemon(nochdir, noclose);

	//ignore signals ΠΕΊΕ
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
}

std::string CoreBacktrace(const std::string &version, int32_t iSigno)
{
    std::ostringstream os;

    //输出程序的绝对路径
    char buffer[4096] = {0};
    memset(buffer, 0, sizeof(buffer));
    readlink("/proc/self/exe", buffer, sizeof(buffer));
    os << buffer << std::endl;

    //输出程序的版本
    os << "version: " << version << std::endl;

    //输出信息的时间
    memset(buffer, 0, sizeof(buffer));
    time_t tNowTime = std::time(nullptr);
    os << "Dump Time: " << get_datetime_string(tNowTime) << std::endl;

    //线程和信号
    os << "Curr Thread: " << std::this_thread::get_id() << ", Catch Signal:" << iSigno << std::endl;

    //堆栈
    void* DumpArray[256];
    int32_t nSize = backtrace(DumpArray, 256);
    os << "Core Backtrace Depth = " << nSize << std::endl;
    if (nSize > 0)
    {
        char** symbols = backtrace_symbols(DumpArray, nSize);
        if (symbols != nullptr)
        {
            for (int32_t i = 0; i < nSize; ++i)
            {
                os << "#" << (i+1) << "\t\t" << symbols[i] << std::endl;
            }
            free(symbols);
        }
    }

    return os.str();
}

std::vector<std::string> string_split(const std::string& s, const std::string& delim)
{
	std::vector<std::string> v;

	if (!delim.empty())
	{
		auto b = s.begin();
		auto i = s.end();
		while ((i = std::search(b, s.end(), delim.begin(), delim.end())) != s.end())
		{
			if (i - b > 0) // token can't be empty string
			{
				v.emplace_back(std::string(b, i));
			}

			b = i + delim.length();
		}

		if (b != s.end())
		{
			v.emplace_back(std::string(b, s.end()));
		}
	}
	else
	{
		v.emplace_back(s);
	}
	return v;
}

static inline uint8_t toHex(const uint8_t &x)
{
    return x > 9 ? x + 55: x + 48;
}

std::string url_encode(const char* pszInput, size_t nInputLength)
{
    std::stringstream ssOutput;

    for(int32_t i = 0; i < (int32_t)nInputLength; ++i, ++pszInput)
    {
        if(isalnum((uint8_t)(*pszInput)))
        {
            ssOutput<< *pszInput;
        }
        else
        {
            if(*pszInput==' ')
            {
                ssOutput<< '+';
            }
            else
            {
                ssOutput<< '%'<< toHex( (uint8_t)(*pszInput) >> 4 )<< toHex( (uint8_t)(*pszInput) % 16 );
            }
        }
    }

    return ssOutput.str();
}

bool is_leader_card(int32_t card_id)
{
    return (card_id == 10 || card_id == 11);
}

bool is_same_card(int32_t card_id1, int32_t card_id2)
{
    return ((card_id1 == card_id2) || (is_leader_card(card_id1) && is_leader_card(card_id2)));
}
