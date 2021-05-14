#pragma once

#include <iostream>
#include <tuple>

#include "../assert.h"
#include "container_traits.h"

// асимптотика(для меня)
// 
// MORELINE => намного дольше LINE
// LINE => O(row_sz * col_sz) => требуется пройти по всей матрице
// ONE => O(1) => происходят какие-то мизерные вычисления

#define verify_operator(T1, oop, T2) typename = decltype(std::declval<T1&>() oop std::declval<const T2&>())

template<typename T>
class matrix {
	size_t row_sz, col_sz;
	T* data = nullptr;

	// "корректно" => значит, что можно не волноваться о том, где и как ее вызвать

	// корректно копирует матрицу
	// LINE
	void copy_matrix(const matrix& source) {
		if (this != &source) {
			resize(source.row_sz, source.col_sz);

			// Это нельзя заменить на memcpy, потому что T может быть не простым типом (vector, string...)
			// TODO можно делать проверку на обычный тип (int, double, long long...), для которых использовать свой memcpy

			size_t n = row_sz * col_sz;
			for (size_t i = 0; i < n; i++) {
				data[i] = source.data[i];
			}
		}
	}

	// корректно перемещает матрицу
	// ONE
	void move_matrix(matrix& source) {
		if (this != &source) {
			delete[] data;

			row_sz = source.row_sz;
			col_sz = source.col_sz;
			data = source.data;

			source.data = nullptr;
			source.row_sz = source.col_sz = 0;
		}
	}

public:

	//=========================//
	//==STANDART CONSTRUCTORS==//
	//=========================//

	matrix() { // ONE
		row_sz = col_sz = 0;
	}
	matrix(const matrix& source) { // LINE
		copy_matrix(source);
	}
	matrix(matrix&& source) noexcept { // ONE
		move_matrix(source);
	}
	~matrix() { // ONE
		delete[] data;
		// можно еще обнулить все, хотя зачем?
	}

	matrix& operator = (const matrix& source) { // LINE
		copy_matrix(source);
		return *this;
	}
	matrix& operator = (matrix&& source) noexcept { // ONE
		move_matrix(source);
		return *this;
	}

	//=========================//
	//===OTHERS CONSTRUCTORS===//
	//=========================//

	matrix(size_t row_size, size_t col_size) { // LINE
		resize(row_size, col_size);
	}

	template<typename container_t, 
		typename = 
		decltype(is_container<container_t>::value, is_container<typename container_t::value_type>::value)>
	matrix(const container_t& container2d) { // LINE

		resize(container2d.size(), !container2d.empty() ? container2d.begin()->size() : 0);

		size_t i = 0;
		for (const auto& row_container : container2d) {
			ASSERT(col_sz == row_container.size(), "this is not a matrix");

			for (const auto& col_container : row_container) {
				data[i] = col_container;
				i++;
			}
		}
	}

	//=========================//
	//===CONTAINER FUNCTIONS===//
	//=========================//

	void resize(size_t row_size, size_t col_size, T fill_value = T()) { // LINE
		if (data) {
			delete[] data;
		}

		row_sz = row_size;
		col_sz = col_size;
		data = new T[row_sz * col_sz];

		size_t n = row_sz * col_sz;
		for (size_t i = 0; i < n; i++) {
			data[i] = fill_value;
		}
	}
	void clear() { // ONE
		delete[] data;
		data = nullptr;
		row_sz = col_sz = 0;
	}

	size_t row_size() const { // ONE
		return row_sz;
	}
	size_t col_size() const { // ONE
		return col_sz;
	}
	std::tuple<size_t, size_t> shape() const { // ONE
		return std::make_pair(row_sz, col_sz);
	}

	T* operator [](size_t row) { // ONE
		return data + row * col_sz;
	}
	const T* operator [](size_t row) const { // ONE
		return data + row * col_sz;
	}

	//=========================//
	//=====MATRIX FUNCTIONS====//
	//=========================//

	// проходит по всем элементам матрицы и применяет fn
	// fn должна возвращать void
	template<typename func_t>
	void aggregate(func_t&& fn) const { // LINE
		size_t n = row_sz * col_sz;
		for (size_t i = 0; i < n; i++) {
			fn(data[i]);
		}
	}
	// проходит по всем элементам матрицы и применяет fn
	// fn должна возвращать void
	template<typename func_t>
	void aggregate(func_t&& fn) { // LINE
		size_t n = row_sz * col_sz;
		for (size_t i = 0; i < n; i++) {
			fn(data[i]);
		}
	}

	// применяет fn для данной матрицы. второй переменной будет элемент из m
	template<typename func_t>
	void aggregate(const matrix& m, func_t&& fn) const { // LINE
		ASSERT(shape() == m.shape(), "wrong matrix shape");

		size_t n = row_sz * col_sz;
		for (size_t i = 0; i < n; i++) {
			fn(data[i], m.data[i]);
		}
	}
	// применяет fn для данной матрицы. второй переменной будет элемент из m
	template<typename func_t>
	void aggregate(const matrix& m, func_t&& fn) { // LINE
		ASSERT(shape() == m.shape(), "wrong matrix shape");

		size_t n = row_sz * col_sz;
		for (size_t i = 0; i < n; i++) {
			fn(data[i], m.data[i]);
		}
	}

	template<verify_operator(T, +=, T)>
	T sum() const {
		T res = T();
		aggregate([&res](const T& value) {
			res += value;
		});
		return res;
	}
	template<verify_operator(T, +=, T)>
	T mean() const {
		return sum() / (row_sz * col_sz);
	}

	// вернет матрицу, в которой [i][j] => [j][i]
	matrix transpose() {
		matrix result(col_sz, row_sz);
		for (size_t row = 0; row < row_sz; row++) {
			for (size_t col = 0; col < col_sz; col++) {
				result[col][row] = (*this)[row][col];
			}
		}
		return result;
	}

	//=========================//
	//=====MATRIX OPERATORS====//
	//=========================//

	template<verify_operator(T, +=, T)>
	matrix& operator += (const matrix& add) { // LINE
		aggregate(add, [](T& value, const T& add) {
			value += add;
		});
		return *this;
	}
	template<verify_operator(T, +=, T)>
	matrix operator + (const matrix& add) const { // LINE
		return matrix(*this) += add;
	}

	template<verify_operator(T, -=, T)>
	matrix& operator -= (const matrix& sub) { // LINE
		aggregate(sub, [](T& value, const T& sub) {
			value -= sub;
		});
		return *this;
	}
	template<verify_operator(T, -=, T)>
	matrix operator - (const matrix& sub) const { // LINE
		return matrix(*this) -= sub;
	}

	template<verify_operator(T, *=, T)>
	matrix& operator *= (const matrix& mult) { // MORELINE
		return *this = *this * mult;
	}
	template<verify_operator(T, *=, T)>
	matrix operator * (const matrix& mult) const { // MORELINE
		ASSERT(col_sz == mult.row_sz, "wrong matrix shape");
		
		matrix result(row_sz, mult.col_sz);

		for (size_t i = 0; i < row_sz; i++) {
			for (size_t j = 0; j < mult.col_sz; j++) {
				for (size_t k = 0; k < col_sz; k++) {
					result[i][j] += (*this)[i][k] * mult[k][j];
				}
			}
		}
		return result;
	}
};

template<typename T, verify_operator(std::ostream, <<, T)>
std::ostream& operator << (std::ostream& output, const matrix<T>& matrix) {
	output << matrix.row_size() << " " << matrix.col_size() << "\n";

	for (size_t i = 0; i < matrix.row_size(); i++) {
		for (size_t j = 0; j < matrix.col_size(); j++) {
			output << matrix[i][j] << " ";
		}
		output << "\n";
	}
	return output;
}

// result_ij = fn(A_ij, B_ij)
template<typename T, typename func_t>
matrix<T> aggregate(const matrix<T>& A, const matrix<T>& B, func_t&& fn) {
    ASSERT(A.shape() == B.shape(), "incorrect matrix shape");

    matrix<T> result(A.row_size(), A.col_size());
    for (size_t i = 0; i < A.row_size(); i++) {
        for (size_t j = 0; j < A.col_size(); j++) {
            result[i][j] = fn(A[i][j], B[i][j]);
        }
    }
    return result;
}

#undef verify_operator
