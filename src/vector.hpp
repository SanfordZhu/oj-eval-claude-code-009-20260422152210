#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <algorithm>

namespace sjtu
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector
{
public:
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */
	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */
	class const_iterator;
	class iterator
	{
	// The following code is written for the C++ type_traits library.
	// Type traits is a C++ feature for describing certain properties of a type.
	// For instance, for an iterator, iterator::value_type is the type that the
	// iterator points to.
	// STL algorithms and containers may use these type_traits (e.g. the following
	// typedef) to work properly. In particular, without the following code,
	// @code{std::sort(iter, iter1);} would not compile.
	// See these websites for more information:
	// https://en.cppreference.com/w/cpp/header/type_traits
	// About value_type: https://blog.csdn.net/u014299153/article/details/72419713
	// About iterator_category: https://en.cppreference.com/w/cpp/iterator
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	private:
		T* ptr;
		vector* vec;

	public:
		iterator() : ptr(nullptr), vec(nullptr) {}
		iterator(T* p, vector* v) : ptr(p), vec(v) {}

		/**
		 * return a new iterator which pointer n-next elements
		 * as well as operator-
		 */
		iterator operator+(const int &n) const
		{
			return iterator(ptr + n, vec);
		}
		iterator operator-(const int &n) const
		{
			return iterator(ptr - n, vec);
		}
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const
		{
			if (vec != rhs.vec) throw invalid_iterator();
			return ptr - rhs.ptr;
		}
		iterator& operator+=(const int &n)
		{
			ptr += n;
			return *this;
		}
		iterator& operator-=(const int &n)
		{
			ptr -= n;
			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator tmp = *this;
			++ptr;
			return tmp;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
			++ptr;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator tmp = *this;
			--ptr;
			return tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
			--ptr;
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const{
			return *ptr;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory address).
		 */
		bool operator==(const iterator &rhs) const {
			return ptr == rhs.ptr;
		}
		bool operator==(const const_iterator &rhs) const;
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return ptr != rhs.ptr;
		}
		bool operator!=(const const_iterator &rhs) const;

		friend class const_iterator;
		friend class vector;
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */
	class const_iterator
	{
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	private:
		const T* ptr;
		const vector* vec;

	public:
		const_iterator() : ptr(nullptr), vec(nullptr) {}
		const_iterator(const T* p, const vector* v) : ptr(p), vec(v) {}
		const_iterator(const iterator &it) : ptr(it.ptr), vec(it.vec) {}

		const_iterator operator+(const int &n) const {
			return const_iterator(ptr + n, vec);
		}
		const_iterator operator-(const int &n) const {
			return const_iterator(ptr - n, vec);
		}
		int operator-(const const_iterator &rhs) const {
			if (vec != rhs.vec) throw invalid_iterator();
			return ptr - rhs.ptr;
		}
		const_iterator& operator+=(const int &n) {
			ptr += n;
			return *this;
		}
		const_iterator& operator-=(const int &n) {
			ptr -= n;
			return *this;
		}
		const_iterator operator++(int) {
			const_iterator tmp = *this;
			++ptr;
			return tmp;
		}
		const_iterator& operator++() {
			++ptr;
			return *this;
		}
		const_iterator operator--(int) {
			const_iterator tmp = *this;
			--ptr;
			return tmp;
		}
		const_iterator& operator--() {
			--ptr;
			return *this;
		}
		const T& operator*() const {
			return *ptr;
		}
		bool operator==(const const_iterator &rhs) const {
			return ptr == rhs.ptr;
		}
		bool operator!=(const const_iterator &rhs) const {
			return ptr != rhs.ptr;
		}

		friend class iterator;
		friend class vector;
	};

private:
	T* data;
	size_t vec_size;
	size_t vec_capacity;

	void reallocate(size_t new_capacity) {
		T* new_data = static_cast<T*>(operator new[](new_capacity * sizeof(T)));
		size_t i = 0;
		try {
			for (; i < vec_size; ++i) {
				new (new_data + i) T(data[i]);
			}
		} catch (...) {
			for (size_t j = 0; j < i; ++j) {
				new_data[j].~T();
			}
			operator delete[](new_data);
			throw;
		}

		for (size_t j = 0; j < vec_size; ++j) {
			data[j].~T();
		}
		operator delete[](data);

		data = new_data;
		vec_capacity = new_capacity;
	}

public:
/**
 * TODO Constructs
 * At least two: default constructor, copy constructor
 */
	vector() : data(nullptr), vec_size(0), vec_capacity(0) {}

	vector(const vector &other) : data(nullptr), vec_size(0), vec_capacity(0) {
		if (other.vec_size > 0) {
			data = static_cast<T*>(operator new[](other.vec_capacity * sizeof(T)));
			vec_capacity = other.vec_capacity;
			size_t i = 0;
			try {
				for (; i < other.vec_size; ++i) {
					new (data + i) T(other.data[i]);
				}
			} catch (...) {
				for (size_t j = 0; j < i; ++j) {
					data[j].~T();
				}
				operator delete[](data);
				throw;
			}
			vec_size = other.vec_size;
		}
	}
/**
 * TODO Destructor
 */
	~vector() {
		for (size_t i = 0; i < vec_size; ++i) {
			data[i].~T();
		}
		operator delete[](data);
	}
/**
 * TODO Assignment operator
 */
	vector &operator=(const vector &other) {
		if (this != &other) {
			vector tmp(other);
			swap(tmp);
		}
		return *this;
	}

	void swap(vector &other) {
		std::swap(data, other.data);
		std::swap(vec_size, other.vec_size);
		std::swap(vec_capacity, other.vec_capacity);
	}
/**
 * assigns specified element with bounds checking
 * throw index_out_of_bound if pos is not in [0, size)
 */
	T & at(const size_t &pos) {
		if (pos >= vec_size) throw index_out_of_bound();
		return data[pos];
	}
	const T & at(const size_t &pos) const {
		if (pos >= vec_size) throw index_out_of_bound();
		return data[pos];
	}
/**
 * assigns specified element with bounds checking
 * throw index_out_of_bound if pos is not in [0, size)
 * !!! Pay attentions
 *   In STL this operator does not check the boundary but I want you to do.
 */
	T & operator[](const size_t &pos) {
		if (pos >= vec_size) throw index_out_of_bound();
		return data[pos];
	}
	const T & operator[](const size_t &pos) const {
		if (pos >= vec_size) throw index_out_of_bound();
		return data[pos];
	}
/**
 * access the first element.
 * throw container_is_empty if size == 0
 */
	const T & front() const {
		if (vec_size == 0) throw container_is_empty();
		return data[0];
	}
/**
 * access the last element.
 * throw container_is_empty if size == 0
 */
	const T & back() const {
		if (vec_size == 0) throw container_is_empty();
		return data[vec_size - 1];
	}
/**
 * returns an iterator to the beginning.
 */
	iterator begin() {
		return iterator(data, this);
	}
	const_iterator begin() const {
		return const_iterator(data, this);
	}
	const_iterator cbegin() const {
		return const_iterator(data, this);
	}
/**
 * returns an iterator to the end.
 */
	iterator end() {
		return iterator(data + vec_size, this);
	}
	const_iterator end() const {
		return const_iterator(data + vec_size, this);
	}
	const_iterator cend() const {
		return const_iterator(data + vec_size, this);
	}
/**
 * checks whether the container is empty
 */
	bool empty() const {
		return vec_size == 0;
	}
/**
 * returns the number of elements
 */
	size_t size() const {
		return vec_size;
	}
/**
 * clears the contents
 */
	void clear() {
		for (size_t i = 0; i < vec_size; ++i) {
			data[i].~T();
		}
		vec_size = 0;
	}
/**
 * inserts value before pos
 * returns an iterator pointing to the inserted value.
 */
	iterator insert(iterator pos, const T &value) {
		size_t index = pos.ptr - data;
		if (vec_size == vec_capacity) {
			reallocate(vec_capacity == 0 ? 1 : vec_capacity * 2);
		}

		// Shift elements to the right
		for (size_t i = vec_size; i > index; --i) {
			new (data + i) T(data[i - 1]);
			data[i - 1].~T();
		}
		// Insert the new element
		new (data + index) T(value);
		++vec_size;

		return iterator(data + index, this);
	}
/**
 * inserts value at index ind.
 * after inserting, this->at(ind) == value
 * returns an iterator pointing to the inserted value.
 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
 */
	iterator insert(const size_t &ind, const T &value) {
		if (ind > vec_size) throw index_out_of_bound();
		return insert(begin() + ind, value);
	}
/**
 * removes the element at pos.
 * return an iterator pointing to the following element.
 * If the iterator pos refers the last element, the end() iterator is returned.
 */
	iterator erase(iterator pos) {
		size_t index = pos.ptr - data;
		for (size_t i = index; i < vec_size - 1; ++i) {
			data[i] = data[i + 1];
		}
		data[vec_size - 1].~T();
		--vec_size;
		return iterator(data + index, this);
	}
/**
 * removes the element with index ind.
 * return an iterator pointing to the following element.
 * throw index_out_of_bound if ind >= size
 */
	iterator erase(const size_t &ind) {
		if (ind >= vec_size) throw index_out_of_bound();
		return erase(begin() + ind);
	}
/**
 * adds an element to the end.
	 */
	void push_back(const T &value) {
		if (vec_size == vec_capacity) {
			reallocate(vec_capacity == 0 ? 1 : vec_capacity * 2);
		}
		new (data + vec_size) T(value);
		++vec_size;
	}
/**
 * remove the last element from the end.
 * throw container_is_empty if size() == 0
	 */
	void pop_back() {
		if (vec_size == 0) throw container_is_empty();
		data[vec_size - 1].~T();
		--vec_size;
	}
};


template<typename T>
bool vector<T>::iterator::operator==(const const_iterator &rhs) const {
	return ptr == rhs.ptr;
}

template<typename T>
bool vector<T>::iterator::operator!=(const const_iterator &rhs) const {
	return ptr != rhs.ptr;
}

}

#endif