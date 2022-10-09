#pragma once

#include "common_base.h"



class CShmMemory;

class CCodeQueue
{
public:

	enum
	{
		invalid_offset		= -1,	//无效的偏移量
		reserved_length	= 8,	//预留的空间长度
	};

	/** 队列所用的共享内存区 */
	static CShmMemory* m_pstSharedMemory;

	/** 设置队列所在的共享内存区 */
	static void set_sharedmemory(CShmMemory* pMem);

	/** 得到队列所占用的内存大小*/
	static size_t size(int32_t buffersize);

protected:
	//数据区总长度，单位: Byte。
	int32_t m_iSize;
	//可用数据起始位置
	int32_t m_iHead;
	//可用数据结束位置
	int32_t m_iTail;
	//数据区的偏移位置
	int32_t m_iCodeOffset;

public:
	CCodeQueue(int32_t size);
	~CCodeQueue();

	int32_t initialize(int32_t size);

	int32_t resume(int32_t size);

	void* operator new( size_t size ) noexcept;

	void  operator delete( void *pbuffer );

	/*
	* @method:    append 从尾部追加消息buffer。该函数只是改变m_iTail，即使是在多线程环境下也不需要加/解锁操作，
	*				因为，对于四字节的int32_t的读写操作是原子操作
	* @fullname:  CCodeQueue::append
	* @access:    public 
	* @param: const char * code
	* @param: int32_t size
	* @return:   int32_t。 
	* - 0: 成功
	* - >0 : 失败
	* @qualifier: 
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t append(const char* code, int32_t size);

	
	/*
	* @method:    pop 从队列头部取一条消息
	* @fullname:  CCodeQueue::pop
	* @access:    public 
	* @param[in]: char * dst
	* @param[in,out]: short & outlength。传入dst的长度，传出实际消息code的长度
	* @return:   int32_t
	* - 0: 成功. outlength返回实际code的长度
	* - >0 : 失败， dst,outlength的值无意义
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t pop(char* dst, int32_t& outlength);

	
	/*
	* @method:    full 判断队列是否满
	* @fullname:  CCodeQueue::full
	* @access:    public 
	* @param: void
	* @return:   bool
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	bool full(void);


	/*
	* @method:    empty 判读队列是否为空
	* @fullname:  CCodeQueue::empty
	* @access:    public 
	* @param: void
	* @return:   bool
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	bool empty(void);

protected:
	/** 返回数据区偏移 */
	int32_t code_offset(void)const;

	int32_t set_boundary(int32_t head, int32_t tail);

	int32_t get_boundary(int32_t& head, int32_t& tail)const;

	char* get_code_buffer(void)const;

public:
	/*
	* @method:    get_free_size
	* @fullname:  CCodeQueue::get_free_size
	* @access:    protected 
	* @param: void
	* @return:   int32_t
	* - 返回可用空间的大小。如果没有可用空间，返回值为0。也就是说该函数的
	*	返回值总是>=0的。
	* @qualifier: const
	* @note	保证队列buffer的长度不能为0,否则结果为定义.
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t get_free_size()const;

	int32_t get_code_size()const;

	
	

private:
	//forbidden copy constructor
	CCodeQueue(const CCodeQueue& init);
	CCodeQueue& operator =(const CCodeQueue&);
};



