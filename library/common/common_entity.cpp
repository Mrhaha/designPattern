#include "common_entity.h"

CTCPEntity::CTCPEntity(EnumEntityTypes entity_type, int32_t entity_id, const std::string& str_ip, uint32_t ip, uint16_t port) :
	entity_type_(entity_type),
	entity_id_(entity_id),
	str_ip_(str_ip),
	ip_(ip),
	port_(port)
{
	
}
