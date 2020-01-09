#pragma once

namespace Memory
{
	/**
	 * Simple pointer wrapper that doesn't allow the deletion of the underlying pointer.
	 */
	template <typename T>
	class relay_ptr
	{
	public:
		/**
		 * Constructor with pointer
		 *
		 * @param object The pointer to be stored
		 */
		relay_ptr(T* aptr)
			: ptr(aptr)
		{ }

		/**
		 * Empty constructor
		 */
		relay_ptr()
			: ptr(nullptr)
		{ }

		/**
		 * Copy constructor
		 *
		 * @param other Another relay object
		 */
		relay_ptr(const relay_ptr<T> &relay)
		{
			ptr = relay.ptr;
		}

#ifdef _MEMORY_
		/**
		 * Create relay_ptr from unique_ptr
		 *
		 * @param aptr std::unique_ptr with T value
		 */
		relay_ptr(std::unique_ptr<T>& aptr)
			: ptr(aptr.get())
		{ }

		/**
		 * Create relay_ptr from shared_ptr
		 *
		 * @param aptr std::shared_ptr with T value
		 */
		relay_ptr(std::shared_ptr<T>& aptr)
			: ptr(aptr.get())
		{ }
#endif

		/**
		 * Destructor
		 */
		~relay_ptr()
		{

		}

		/**
		 * Set relay_ptr value to different ptr, the value of the original ptr is not changed
		 *
		 * @param other New value for this relay object
		 * @return This, but with new values
		 */
		relay_ptr<T>& operator=(const relay_ptr<T>& rhs)
		{
			ptr = rhs.ptr;
			return *this;
		}

		/**
		 * Set relay_ptr value to different ptr, the value of the original ptr is not changed
		 *
		 * @param other New value for this relay object
		 * @return This, but with new values
		 */
		relay_ptr<T>& operator=(T* rhs)
		{
			ptr = rhs;
			return *this;
		}

		/**
		 * Compares two relay pointers of type T by checking
		 * if the underlying pointers point to the same memory
		 *
		 * @param other The other relay pointer to compare to
		 * @return True if both relays point to the same memory
		 */
		bool operator==(const relay_ptr<T>& other) const
		{
			return ptr == other.ptr;
		}

		/**
		 * Checks if the underlying pointer is valid
		 *
		 * @return True if the pointer is not pointing to nullptr
		 */
		bool valid() const
		{
			return ptr != nullptr;
		}

		/**
		 * Member access operator
		 *
		 * @return Underlying object const pointer
		 */
		T* operator->() const
		{
			return ptr;
		}
	private:
		T* ptr;
	};
}