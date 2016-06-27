/*
	Smokin::MemoryPool - Memory Pool.
	Copyright (C) 2007  Rui Varela - rui.filipe.varela@gmail.com

	This file is part of Smokin::MemoryPool.

	Smokin::MemoryPool is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2.1 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __SMOKIN_MEMORY_POOL__
#define __SMOKIN_MEMORY_POOL__

#include <malloc.h>
#include <sstream>
#include <vector>
#include <cassert>

#include "..\Exception\BaseExceptions.h"

#define SMOKIN_MEMORY_POOL_USE_MALLOC
//#define SMOKIN_MEMORY_POOL_DEBUG_IS_VALID_ENABLED

namespace Smokin
{
	unsigned int const memory_pool_default_pool_size = 1024 * 10;
	unsigned int const memory_pool_default_block_size = 1024;

	class MemoryPool
	{
	public:
		typedef unsigned int SizeType;
		typedef unsigned char DataType;
		typedef DataType* DataTypePointer;
		static unsigned int const data_type_size = sizeof(DataType);

		//
		//	pool_size : total memory in bytes for the pool
		//  block_size : size for each block to be used
		//
		MemoryPool(SizeType const pool_size = memory_pool_default_pool_size, 
				   SizeType const block_size = memory_pool_default_block_size);

		virtual ~MemoryPool();

		// 
		// Changes the total memory for the pool, and then reblocks
		//
		virtual void reallocPool(SizeType const pool_size, SizeType const block_size);

		//
		// Realloc and not reblock
		//
		virtual void reallocPool(SizeType const pool_size);

		//
		// Rebuilds the memory paging for this memory pool
		//
		virtual void reblockPool(SizeType const block_size);

		//
		// Requests a memory block, no auto deletion.
		//
		virtual DataTypePointer requestRawMemoryBlock();

		//
		//	Releases a memory block
		//
		virtual void releaseRawMemoryBlock(DataTypePointer data);
		virtual void releaseAllMemoryBlocks();

		SizeType numberOfBlocks() const;
		SizeType numberOfUsedBlocks() const;
		SizeType numberOfFreeBlocks() const;
		SizeType blockSize() const;

		SizeType numberOfReblocks() const;
	private:
		bool isValid() const;
		void freePool();

		SizeType pool_size_;
		SizeType block_size_;
		SizeType number_of_blocks_;

		unsigned int number_of_reblocks_;

		DataTypePointer data_;

		std::vector<DataTypePointer> free_memory_;
	};

	template <typename T>
	class MemoryBlockAutoHandler
	{
	public:
		typedef T DataType;
		typedef DataType* DataTypePointer;

		MemoryBlockAutoHandler(MemoryPool & memory_pool)
			: memory_pool_(memory_pool)
		{
			data_ = MemoryBlockAutoHandler::DataTypePointer( memory_pool_.requestRawMemoryBlock() );
		}

		~MemoryBlockAutoHandler()
		{
			memory_pool_.releaseRawMemoryBlock( MemoryPool::DataTypePointer(data_) );
		}

		DataTypePointer const data() const { return data_; }
	private :
		MemoryPool& memory_pool_;
		DataTypePointer data_;
	};

	//
	// Implementation
	//

	inline MemoryPool::MemoryPool(MemoryPool::SizeType const pool_size, MemoryPool::SizeType const block_size)
		:pool_size_(0), block_size_(0), number_of_blocks_(0), number_of_reblocks_(0), data_(0)
	{
		reallocPool(pool_size, block_size);
		
		assert(isValid());
	}

	inline MemoryPool::~MemoryPool()
	{
		assert(isValid());

		freePool();
	}

	inline void MemoryPool::reallocPool(MemoryPool::SizeType const pool_size, MemoryPool::SizeType const block_size)
	{
		assert(isValid());

		reallocPool(pool_size);
		reblockPool(block_size);
		
		assert(isValid());
	}

	inline void MemoryPool::reallocPool(MemoryPool::SizeType const pool_size)
	{
		assert(isValid());

		freePool();

#ifdef SMOKIN_MEMORY_POOL_USE_MALLOC

		data_ = (DataTypePointer) malloc(pool_size); // data type dependent??

		if (!data_)
		{
			std::stringstream stringstream;
			stringstream << "Memory pool unable to allocate " << pool_size << " bytes.";
			throw AllocationException(10001, stringstream.str());
		}
#else
		try {
			data_ = (DataTypePointer) new DataType[pool_size];
		} catch (std::bad_alloc exception)
		{
			std::stringstream stringstream;
			stringstream << "Memory pool unable to allocate " << pool_size << " bytes. " <<
				<< "bad_alloc message : " << exception.what();

			throw AllocationException(10001, stringstream.str());
		}

#endif //SMOKIN_MEMORY_POOL_USE_MALLOC

		pool_size_ = pool_size;

		assert(isValid());
	}

	inline void MemoryPool::reblockPool(MemoryPool::SizeType const block_size)
	{	
		assert(isValid());

		number_of_reblocks_++;

		block_size_ = block_size;
		number_of_blocks_ = pool_size_ / block_size;

		free_memory_.clear();
		free_memory_.reserve(number_of_blocks_);

		releaseAllMemoryBlocks();

		assert(isValid());
	}

	inline void MemoryPool::freePool()
	{
		assert(isValid());

		if (data_)
		{

#ifdef SMOKIN_MEMORY_POOL_USE_MALLOC
			free(data_);
#else
			delete[](data_);
#endif //SMOKIN_MEMORY_POOL_USE_MALLOC

			data_ = 0;
		}

		pool_size_ = 0;
		block_size_ = 0;
		number_of_blocks_ = 0;
		free_memory_.clear();

		assert(isValid());
	}

	inline MemoryPool::DataTypePointer MemoryPool::requestRawMemoryBlock()
	{
		assert(isValid());
		
		if ( free_memory_.empty() )
		{
			throw MemoryFullException(10002, "Memory pool is full.");
		}

		DataTypePointer data = free_memory_.back();
		free_memory_.pop_back();

		assert(isValid());
		return data;
	}

	inline void MemoryPool::releaseRawMemoryBlock(DataTypePointer data)
	{
		assert(isValid());

		free_memory_.push_back(data);
		
		assert(isValid());
	}

	inline void MemoryPool::releaseAllMemoryBlocks()
	{
		assert(isValid());

		free_memory_.clear();

		DataTypePointer data_iterator = data_;

		for (SizeType i = 0; i != number_of_blocks_; i++)
		{
			free_memory_.push_back(data_iterator);
			data_iterator += (block_size_ / data_type_size);
		}

		assert(isValid());
	}

	inline MemoryPool::SizeType MemoryPool::numberOfBlocks() const
	{
		return number_of_blocks_;
	}

	inline MemoryPool::SizeType MemoryPool::numberOfUsedBlocks() const
	{
		return numberOfBlocks() - numberOfFreeBlocks();
	}

	inline MemoryPool::SizeType MemoryPool::numberOfFreeBlocks() const
	{
		return MemoryPool::SizeType( free_memory_.size() );
	}

	inline MemoryPool::SizeType MemoryPool::blockSize() const
	{
		return block_size_;
	}

	inline MemoryPool::SizeType MemoryPool::numberOfReblocks() const
	{
		return number_of_reblocks_;
	}

#ifdef SMOKIN_MEMORY_POOL_DEBUG_IS_VALID_ENABLED

	inline bool MemoryPool::isValid() const
	{
		bool is_valid = true;

		is_valid &= (block_size_ <= pool_size_);

		// test if every free block is really part of this pool
		for (std::vector<DataTypePointer>::size_type free_block = 0; 
			free_block != free_memory_.size(); 
			++free_block)
		{
			bool block_found = false;

			DataTypePointer data_iterator = data_;
			for (SizeType i = 0; i != number_of_blocks_; i++)
			{
				if (data_iterator == free_memory_[free_block])
				{
					block_found = true;
				}
				data_iterator += (block_size_ / data_type_size);
			}

			is_valid &= block_found;
		}

		// test if the same block has been released multiple times
		DataTypePointer data_iterator = data_;
		for (SizeType i = 0; i != number_of_blocks_; i++)
		{
			int counter = 0;

			for (std::vector<DataTypePointer>::size_type free_block = 0; 
				free_block != free_memory_.size(); 
				++free_block)
			{
				if (data_iterator == free_memory_[free_block])
				{
					counter++;
				}
			}
			data_iterator += (block_size_ / data_type_size);

			is_valid &= (counter < 2);
		}

		return is_valid;
	}

#else

	inline bool MemoryPool::isValid() const
	{
		return true;
	}

#endif //  SMOKIN_MEMORY_POOL_DEBUG_IS_VALID_ENABLED



};

#endif __SMOKIN_MEMORY_POOL__

