#ifdef _MSC_VER

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif	

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#endif //_MSC_VER


#include <iostream>
#include <cassert>
#include "MemoryPool.h"

int main()
{
#ifdef _MSC_VER
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif //_MSC_VER

	Smokin::MemoryPool memory_pool;
	
	{

	assert(memory_pool.blockSize() == Smokin::memory_pool_default_block_size);
	assert(memory_pool.numberOfBlocks() == memory_pool.numberOfFreeBlocks());
	assert(memory_pool.numberOfUsedBlocks() == 0);
	
	Smokin::MemoryBlockAutoHandler<int> block_0(memory_pool);
	assert(memory_pool.numberOfUsedBlocks() == 1);

	{
		Smokin::MemoryBlockAutoHandler<int> block_1(memory_pool);
		assert(memory_pool.numberOfUsedBlocks() == 2);

		*block_1.data() = 256;

		assert(*block_1.data() == 256);
	}

	assert(memory_pool.numberOfUsedBlocks() == 1);

	Smokin::MemoryPool::DataTypePointer block_2 = memory_pool.requestRawMemoryBlock();

	*block_2 = 23;
	assert(*block_2 == 23);

	memory_pool.releaseRawMemoryBlock(block_2);

	}

	unsigned int pool_size = 200;
	unsigned int block_size = 10;
	unsigned int number_of_blocks = (pool_size / block_size);

	memory_pool.reallocPool(pool_size, block_size);
	assert(memory_pool.blockSize() == block_size);
	assert(memory_pool.numberOfBlocks() == number_of_blocks);
	assert(memory_pool.numberOfUsedBlocks() == 0);

	for (unsigned int i = 0; i != number_of_blocks; ++i)
	{
		memory_pool.requestRawMemoryBlock();
	}

	try 
	{
		memory_pool.requestRawMemoryBlock(); 	// the next statement will throw an MemoryFullException
		assert(false);
	}
	catch (Smokin::MemoryFullException exception)
	{

	}

	block_size = 20;
	number_of_blocks = (pool_size / block_size);
	memory_pool.reblockPool(block_size);
	assert(memory_pool.blockSize() == block_size);
	assert(memory_pool.numberOfBlocks() == number_of_blocks);
	assert(memory_pool.numberOfUsedBlocks() == 0);


	std::cout << "Done." << std::endl;
	std::cin.get();

	return 0;
}
