#include "common_platform.h"
#include "common_base.h"


void create_compile_file(const char* pszName, const char* git_sha1, const char* build_date, const char* build_time)
{
	assert(nullptr != pszName && nullptr != git_sha1 && nullptr != build_date && nullptr != build_time);
	FILE* fp = fopen(pszName, "w");
	if (nullptr == fp)
	{
		return;
	}
	fprintf(fp, "build git %s in %s %s", git_sha1, build_date, build_time);
	fclose(fp);
}

void delete_compile_file(const char* pszName)
{
	assert(nullptr != pszName);
	unlink(pszName);
}