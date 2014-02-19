function [L, D, p, S, B] = ildl(A, fill, tol, pp_tol, ordering, equil)
%ILDL   Incomplete LDL factorization of indefinite Hermitian matrices.
%Before factoring, the matrix is equilibriated (resulting in a matrix S)
%and then permuted by the Reverse Cuthill-McKee algorithm.
%
%   [L, D, p, S, B] = ILDL(A, fill_factor, tol, pp_tol, ordering, equil) attempts to 
%   factor A into the form B = P'SASP = LDL', where P is the permutation matrix generated 
%   by the permutation vector p. During factorization, Bunch-Kaufman partial
%   pivoting is used to maintain stability.
%
%   Parameters:
%       A - an n x n indefinite Hermitian matrix.
%
%       fill_factor - a parameter to control memory usage. Each column of L will
%       have at most fill_factor*nnz(A)/n non-zero elements. Default: 1.0
%
%       tol - a parameter to control accuracy. For the each column of L, 
%       elements less than norm(column(L), 1) will be dropped. Default: 0.001
%
%       pp_tol - a parameter to control aggresiveness of pivoting. 
%       Allowable ranges are [0,inf). If the parameter is >= 1, Bunch-Kaufman pivoting 
%       will be done in full. If the parameter is 0, partial pivoting will be turned off 
%       and the first non-zero pivot under the diagonal will be used. Choices close to 0
%       increase locality in pivoting (pivots closer to the diagonal are used) while
%       choices closer to 1 increase the stability of pivoting. Useful for situations
%       where you care more about preserving the structure of the matrix rather than
%       bounding the size of its elements. Default: 2.0
%
%		ordering - determines what reordering scheme is used to preorder the matrix. 
%		AMD and RCM are available. This parameter must be one of 'amd' or 'rcm'. Default: 'amd'
%
%       equil - determine if matrix is to be equilibriated (in the max
%       norm) before anything. This parameter must be one of 'none', 'bunch', or 'iter'.
%       Default: 'bunch'
%
%   Outputs:
%       L - unit lower triangular factor of P'SASP.
%
%       D - block diagonal factor (with 1x1 and 2x2 blocks) of P'SASP.
%
%       p - permutation vector of permutations done to A.
%
%       S - diagonal scaling matrix used to equilibriate A in the max-norm.
%
%       B - the scaled and permutated matrix B = P'SASP after
%       factorization.
%
%   Further documentation can be found <a href="http://www.cs.ubc.ca/~inutard/html/">here</a>.