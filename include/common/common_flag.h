#pragma once

#include "common_base.h"

class CFlag
{
public:

	enum
	{
		all_bits = (uint32_t)0xffffffff,
	};

	CFlag(uint32_t flag);
	CFlag();
	~CFlag();

	void initialize(uint32_t flag);

	uint32_t flag();

	void unset(uint32_t bit);

	void set(uint32_t bit);

	bool is_setted(uint32_t bit);

	uint32_t get_allbit();

protected:
	uint32_t m_uiFlag;
private:
};




