//-*-mode:c++-*-
#ifndef SQUARE_MATRIX_MATRIX_PIVOT_H
#define SQUARE_MATRIX_MATRIX_PIVOT_H

using std::abs;

/*!	There are four parts to this pivoting algorithm.
	For A, due to storing only the lower half, there are three steps to performing a symmetric permutation:
		-#	A(k, 1:k) must be swapped with A(r, 1:k) (row-row swap).
		-#	A(k:r, k) must be swapped with A(r, k:r) (row-column swap).
		-#	A(k:r, k) must be swapped with A(k:r, r) (column-column swap).
	
	For L, since column k and r are not yet formed, there is only one step (a row permutation):
		-#	L(k, 1:k) must be swapped with L(r, 1:k) (row-row swap).
*/
template <class el_type>
inline void square_matrix<el_type>::pivot(square_matrix_swap_struct<el_type>& s, ultriangular_matrix<el_type>& L, const int& k, const int& r)
{
	//initialize temp variables
	idx_it idx_iterator; elt_it elt_iterator;
	int i, j, idx;
	
	//------------- row-row swap (1) for A -------------//
	
	//pushes column indices (which contain non-zero elements) of A(k, 1:k) onto row_r
	for (idx_it it = list[k].begin(); it != list[k].begin() + list_first[k]; it++) {
		s.row_r.push_back(*it);
		s.all_swaps.push_back(*it);
	}
	
	//pushes column indices (which contain non-zero elements) of A(r, 1:k) onto row_k
	s.row_k.assign(list[r].begin(), list[r].begin() + list_first[r]);

	//merge these two sets of indices together
	inplace_union(s.all_swaps, list[r].begin(), list[r].begin() + list_first[r]);
	//do row swaps in A (i.e. swap A(k, 1:k) with A(r, 1:k))
	for (idx_it it = s.all_swaps.begin(), end = s.all_swaps.end(); it != end; it++)
		safe_swap(m_idx[*it], k, r);
	s.all_swaps.clear();
	
	//----------------------------------------------------//
	
	
	//---------------------- (2) and (3) for A --------------------------//

	//list_first[r] should have # of nnz of A(r, 0:k-1)
	for (i = list_first[r]; i < (int) list[r].size(); i++)
	{
		j = list[r][i];
		coeffRef(r, j, idx_iterator, elt_iterator);
		if (j == k) {
			s.col_k_nnzs.push_back(r); //A(r, k) is fixed upon permutation so its index stays r
			s.row_r.push_back(k);
		} else {
			s.col_k_nnzs.push_back(j); //place A(r, j) (where k < j < r) into A(j, k)
			list[j].push_back(k);
		}
		s.col_k.push_back(*elt_iterator * sign);
		
		//delete A(r,j) from A.
		*idx_iterator = m_idx[j].back();
		*elt_iterator = m_x[j].back();
		
		m_idx[j].pop_back();
		m_x[j].pop_back();
	}

	// place A(r+1:n, r) into A(r+1:n, k), and A(r, r) into A(k, k)
	for (i = 0; i < (int)m_idx[r].size(); i++)
	{
		idx = m_idx[r][i];
		if (idx > r)
		{
			s.col_k_nnzs.push_back(idx);
			for (j = list_first[idx]; j < (int)list[idx].size(); j++)
			{
				if (list[idx][j] == r)
				{
					s.swapk.push_back(list[idx].begin() + j);
					break;
				}
			}
		}
		else if (idx == r) { 
			// place A(r, r) into A(k, k)
			s.col_k_nnzs.push_back(k);
		}
		s.col_k.push_back(m_x[r][i]);
	}

	//swap A(k:r, k) with A(r, k:r)
	for (i = 0; i < (int) m_idx[k].size(); i++)
	{
		idx = m_idx[k][i];
			
		//if idx < r, we are in (2) (row-col swap) otherwise we are in (3) (col-col swap)
		if (idx > k && idx < r)
		{
			//swap A(i, k) with A(r, i) where k < i < r.
			m_idx[idx].push_back(r);
			m_x[idx].push_back(m_x[k][i] * sign);
				
			//we also have to ensure that list is updated by popping off old entries
			//that were meant for the A(i, k)'s before they were swapped.
			ensure_invariant(idx, k, list[idx]);
			std::swap(list[idx][list_first[idx]], list[idx][list[idx].size() - 1]);
			list[idx].pop_back();
				
			//push back new elements on row_r
			s.row_r.push_back(idx);		
		}
		else if (idx > r)
		{
			//swap A(i, k) with A(i, r) where r < i.
			s.col_r_nnzs.push_back(idx);
			s.col_r.push_back(m_x[k][i]);
				
			//for each non-zero row index in the kth column, find a pointer to it in list
			//these pointers will be used to perform column swaps on list
			for (j = list_first[idx]; j < (int) list[idx].size(); j++) {
				if (list[idx][j] == k) {
					s.swapr.push_back(list[idx].begin() + j);
					break;
				}
			}
		}
		else // idx == k (A(r, k) has been deleted)
		{
			s.col_r_nnzs.push_back(r);	
			s.col_r.push_back(m_x[k][i]);
		}
	}

	//swap all A(i, k) with A(i, r) in list.
	for (typename vector<idx_it>::iterator it = s.swapk.begin(); it != s.swapk.end(); it++)
		**it = k;
	for (typename vector<idx_it>::iterator it = s.swapr.begin(); it != s.swapr.end(); it++)
		**it = r;

	//set the kth col
	m_idx[k].swap(s.col_k_nnzs);
	m_x[k].swap(s.col_k);
	
	//set the rth col
	m_idx[r].swap(s.col_r_nnzs);
	m_x[r].swap(s.col_r);
	
	//set the kth row and rth row
	list[k].swap(s.row_k);
	list[r].swap(s.row_r);

	//row swaps for first
	std::swap(list_first[k], list_first[r]);

	//----------- clear out old variables from last pivot -------------- //
	//for vectors of primitive types, clear is always constant time regardless of how many elements are in the container.
	s.clear();

	//--------end pivot A---------//
	

	//----------pivot L ----------//
	// -------------------- (1) for L ------------------------//
	
	//merge these two sets of indices together
	s.all_swaps.assign(L.list[k].begin(), L.list[k].end());
	inplace_union(s.all_swaps, L.list[r].begin(), L.list[r].end());
	//do row swaps in L (i.e. swap L(k, 1:k) with L(r, 1:k))
	for (idx_it it = s.all_swaps.begin(), end = s.all_swaps.end(); it != end; ++it)
		safe_swap(L.m_idx[*it], k, r);
	s.all_swaps.clear();

	//row swap on row non-zero indices stored in L.list
	L.list[k].swap(L.list[r]);
	//--------end pivot L---------//
}

#endif
