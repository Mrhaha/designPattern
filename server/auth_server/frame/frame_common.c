#include "frame_common.h"
int32_t frame_common_file_exists(const char* file)
{
    if (NULL == file)
    {
        return fail;
    }
    struct stat st;
    return (0 == stat(file, &st)) ? success : fail;
}

int32_t frame_common_create_pid_file( const char* file )
{
    char name[100] = {0};
    if (0 >= strlen(file))
    {
        snprintf(name, sizeof(name), "server.pid");
    }
    else
    {
        snprintf(name, sizeof(name), "%s", file);
    }
    FILE* fp = fopen(name, "w");
    if (NULL == fp)
    {
        return fail;
    }
    fprintf(fp, "%d", getpid());
    fclose(fp);
    return success;
}

int32_t frame_common_delete_pid_file( const char* file )
{
    char sz[100] = {0};
    if (0 >= strlen(sz))
    {
        snprintf(sz, sizeof(sz), "server.pid");
    }
    else
    {
        snprintf(sz, sizeof(sz), "%s", file);
    }
    if (-1 == unlink(sz))
    {
        return fail;
    }
    return success;
}
int32_t frame_common_decode_int8(char** in, uint8_t* val)
{
	if (NULL == *in || NULL == in || NULL == val)
	{
		return 0;
	}
	*val = **in;
	(*in)++;
	return (int32_t)sizeof(uint8_t);
}
int32_t frame_common_encode_int8(char** out, uint8_t val)
{
	if (NULL == *out || NULL == out)
	{
		return 0;
	}
	**out = (char) val;
	(*out)++;
	return (int32_t)sizeof(uint8_t);
}
int32_t frame_common_decode_int16( char** in, uint16_t* val )
{
    if (NULL == *in || NULL == in || NULL == val)
    {
        return 0;
    }
    *val = (uint16_t) ntohs((uint16_t)*(uint16_t*)(*in));
    *in += sizeof(uint16_t);
    return (int32_t)sizeof(uint16_t);
}

int32_t frame_common_encode_int16( char** out, uint16_t val )
{
    if (NULL == out || NULL == *out)
    {
        return 0;
    }
    *(uint16_t*)(*out) = htons(val);
    *out += sizeof(uint16_t);

    return (int32_t) sizeof(uint16_t);
}

int32_t frame_common_decode_int32( char** in, uint32_t* val )
{
    if (NULL == *in || NULL == in || NULL == val)
    {
        return 0;
    }
    *val = (uint32_t) ntohl((uint32_t)*(uint32_t*)(*in));
    *in += sizeof(uint32_t);
    return (int32_t)sizeof(uint32_t);
}

int32_t frame_common_encode_int32( char** out, uint32_t val )
{
    if (NULL == out || NULL == *out)
    {
        return 0;
    }
    *(uint32_t*)(*out) = htonl(val);
    *out += sizeof(uint32_t);

    return (int32_t) sizeof(uint32_t);
}

int32_t frame_common_decode_string( char** in, char* val, int16_t max )
{
    if (NULL == in || NULL == *in || NULL == val || 0 >= max)
    {
        return 0;
    }

    int16_t str_len = 0;
    int16_t real_len = 0;

    int32_t tmp_len = frame_common_decode_int16(in, (uint16_t*)&str_len);

    if (str_len <= 0)
    {
        val[0] = '\0';
        return tmp_len;
    }

    real_len = str_len;

    if ( str_len > max)
    {
        real_len = max;
    }

    snprintf(val, real_len, "%s", *in);

    *in += str_len; //NOTE! 不能只移动real_length大小
    val[real_len-1] = '\0';

    return (int32_t)(str_len + tmp_len);
}

int32_t frame_common_encode_string( char** out, char* src, int16_t max )
{
    if (NULL == out || NULL == *out || NULL == src || 0 >= max)
    {
        return 0;
    }
    int16_t str_len = (int16_t) strlen(src);
    int32_t encode_len;
    if (0 != str_len)
    {
        str_len += 1;
    }
    if (str_len > max)
    {
        str_len = max;
    }
    encode_len = frame_common_encode_int16(out, str_len);
    if (0 == encode_len)
    {
        return encode_len;
    }
    snprintf(*out, str_len, "%s", src);
    (*out)[str_len - 1] = '\0';

    *out += str_len;
    return (encode_len + str_len);
}
int32_t frame_common_decode_memory( char** in, char* val, int32_t size )
{
    if (NULL == in || NULL == *in || NULL == val || 0 >= size)
    {
        return 0;
    }
    memcpy((void*)val, (const void*)*in, size);
    *in += size;

    return size;
}
int32_t frame_common_encode_memory(char** out, char* val, int32_t size)
{
    if (NULL == out || NULL == *out || NULL == val || 0 >= size)
    {
        return 0;
    }
    memcpy((void*)*out, (const void*)val, size);
    *out += size;

    return size;
}
int32_t frame_common_root_path(char* src, int32_t len)
{
    if (NULL == src || 0 >= len)
    {
        return fail;
    }
    if (NULL == getcwd(src, len))
    {
        fprintf(stderr, "getcwd fail\n");
        return fail;
    }
    return success;
}
int32_t frame_common_flock( char* str )
{
    if (NULL == str)
    {
        return fail;
    }
    int32_t len;
    int32_t fd;
    char src[MAX_ROOT_PATH_LEN] = {0};
    char f = str[0];
    if ('.' == f)
    {
        if (fail == frame_common_root_path(src, MAX_ROOT_PATH_LEN))
        {
            fprintf(stderr, "get current working dir error\n");
            return fail;
        }
        else
        {
            len = strlen(src);
            snprintf(&src[len], MAX_ROOT_PATH_LEN-len, "/%s", basename(str));
        }
    }
    else
    {
        snprintf(src, MAX_ROOT_PATH_LEN, "%s", str);
    }
    fd = open(src, O_RDONLY);
    if (0 > fd)
    {
        fprintf(stderr, "open %s file fail\n", src);
        return fail;
    }
    //给文件加上一个排他锁，这样就能保证只会有一个service启动了。
    if (0 > flock(fd, LOCK_EX | LOCK_NB))
    {
        fprintf(stderr, "%s is running\n", src);
        return fail;
    }
    return success;
}

