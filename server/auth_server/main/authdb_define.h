#pragma once

#include "frame_common.h"
enum authdb_result_id
{
    result_id_success           = 0,
    result_id_invalid_ses_key   = 1,    //错误的session_key
    result_id_not_obj           = 2,    //hashtable查找出错
    result_id_system            = 3,    //系统错误
    result_id_invalid_group_id  = 4,    //错误的group id

    result_id_max
};
//msg id
#define MSG_SQL_EXECUTE         0x0000       //执行sql语句

#define MSG_REQUEST_TEST        0x0100      //测试代码
#define MSG_REQUEST_AUTH        0x0101      //请求认证
#define MSG_REQUEST_AUTH_RENEW  0x0102      //请求换key
#define MSG_REQ_MAKE_SIGN       0x0103      //生成signature
