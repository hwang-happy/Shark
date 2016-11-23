/*!
 * 
 *
 * \brief       -
 *
 * \author      O. Krause
 * \date        2012
 *
 *
 * \par Copyright 1995-2017 Shark Development Team
 * 
 * <BR><HR>
 * This file is part of Shark.
 * <http://shark-ml.org/>
 * 
 * Shark is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Shark is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MatAERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with Shark.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef SHARK_LINALG_BLAS_KERNELS_DEFAULT_GEMatAV_HPP
#define SHARK_LINALG_BLAS_KERNELS_DEFAULT_GEMatAV_HPP

#include "../../expression_types.hpp"
#include <boost/mpl/bool.hpp>

namespace shark {namespace blas {namespace bindings {
	
//row major can be further reduced to inner_prod()
template<class ResultV, class MatA, class V>
void gemv_impl(
	matrix_expression<MatA, cpu_tag> const& A,
	vector_expression<V, cpu_tag> const& x,
	vector_expression<ResultV, cpu_tag>& result, 
	typename ResultV::value_type alpha,
	row_major
) {
	typedef typename ResultV::value_type value_type;
	for(std::size_t i = 0; i != A().size1();++i){
		value_type value = inner_prod(row(A,i),x);
		if(value != value_type())//handling of sparse results.
			result()(i) += alpha* value;
	}
}

//column major is implemented by computing a linear combination of matrix-rows 
template<class ResultV, class MatA, class V>
void gemv_impl(
	matrix_expression<MatA, cpu_tag> const& A,
	vector_expression<V, cpu_tag> const& x,
	vector_expression<ResultV, cpu_tag>& result,
	typename ResultV::value_type alpha,
	column_major
) {
	typedef typename V::const_iterator iterator;
	typedef typename ResultV::value_type value_type;
	iterator end = x().end();
	for(iterator it = x().begin(); it != end; ++it) {
		value_type multiplier = alpha * (*it);
		//fixme: for sparse result vectors, this might hurt.
		noalias(result)+= multiplier * column(A(), it.index());
	}
}

//unknown orientation is dispatched to row_major
template<class ResultV, class MatA, class V>
void gemv_impl(
	matrix_expression<MatA, cpu_tag> const& A,
	vector_expression<V, cpu_tag> const& x,
	vector_expression<ResultV, cpu_tag>& result,
	typename ResultV::value_type alpha,
	unknown_orientation
) {
	gemv_impl(A,x,result,alpha,row_major());
}

// result += alpha * A * x
template<class ResultV, class MatA, class V>
void gemv(
	matrix_expression<MatA, cpu_tag> const& A,
        vector_expression<V, cpu_tag> const& x,
        vector_expression<ResultV, cpu_tag>& result, 
	typename ResultV::value_type alpha,
	boost::mpl::false_
) {
	typedef typename MatA::orientation orientation;

	gemv_impl(A, x, result, alpha, orientation());
}

}}}
#endif
