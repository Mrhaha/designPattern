#include "frame_nethead.h"
int32_t frame_nethead_encode( char* src, const net_head_t* head )
{
    if (NULL == src || NULL == head)
    {
        return fail;
    }
    *(uint32_t*) src = (uint32_t) htonl(head->pkg_len);
    *(uint32_t*)(src + POSI_UIN) = (uint32_t)htonl(head->uin);
    *(uint32_t*)(src + POSI_SOCK_FD) = (uint32_t)htonl(head->sock_fd);
    *(uint32_t*)(src + POSI_SEQUENCE) = (uint32_t)htonl(head->sequence);
    *(uint16_t*)(src + POSI_MSG_ID) = (uint16_t)htons(head->msg_id);
	*(uint16_t*)(src + POSI_TRANS_TYPE) = (uint16_t)htons(head->trans_type);
    return success;
}

int32_t frame_nethead_decode( const char* src, net_head_t* head )
{
    if (NULL == src || NULL == head)
    {
        return fail;
    }
    head->pkg_len = get_pkg_len(src);
    head->uin = get_uin(src);
    head->sock_fd = get_sock_fd(src);
    head->sequence = get_sequence(src);
    head->msg_id = get_msg_id(src);
    head->trans_type = get_trans_type(src);
    return success;
}
