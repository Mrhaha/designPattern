#pragma once

#include "common_base.h"

class CTCPEntity
{
public:
	explicit CTCPEntity(EnumEntityTypes entity_type = Enum_EET_EntityTypes_Null, int32_t entity_id = 0, const std::string& str_ip = "", uint32_t ip = 0, uint16_t port = 0);

	void set_entity_type(const EnumEntityTypes entity_type) { entity_type_ = entity_type; }
	EnumEntityTypes get_entity_type()const { return entity_type_; }

	void set_entity_id(const int32_t entity_id) { entity_id_ = entity_id; }
	int32_t get_entity_id()const { return entity_id_; }

	void set_str_ip(const std::string &ip) { str_ip_ = ip; }
	const std::string& get_str_ip()const { return str_ip_; }

	void set_ip(const uint32_t& ip) { ip_ = ip; }
	uint32_t get_ip()const { return ip_; }

	void set_port(const uint16_t port) { port_ = port; }
	uint16_t get_port()const { return port_; }

private:
	EnumEntityTypes	entity_type_;
	int32_t 		entity_id_;

	std::string 	str_ip_;
	uint32_t 		ip_;
	uint16_t 		port_;
};