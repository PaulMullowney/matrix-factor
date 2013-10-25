%   Tests the factorization generated by matrix-factor mex file.
% 
%   No parameters are needed. Currently this script only tests
%   the following matrices:
%       aug3dcqp.mtx
%       bloweya.mtx
%       bratu3d.mtx
%       tuma1.mtx
%       tuma2.mtx
%       1138_bus.mtx
%
%   Outputs for each test case:
%       The fill factor (nnz(L+D+L')/nnz(A))
%       The number of iterations for GMRES to converge.

display('==============starting tests================');
warning off;

all_mats = { 'aug3dcqp'; 'bloweya'; 'bratu3d'; ...
            'tuma1'; 'tuma2'; '1138_bus'; 'Lshape_matrices4'; 'lund_b'};
        
opts.fill_factor = 2.2;
opts.tol = 0.001;
opts.pp_tol = 0.65;
for i = 1:length(all_mats)
    mat_name = all_mats{i};
    fprintf('Now testing %s:\n', mat_name);
    base = '';
    file = strcat(base, mat_name, '.mtx');
    A = mmread(file);
    
    opts.equil = 'n';
    opts.shift_factor = eigs(A,[],1,'lm');
    [l d p S B] = shifted_ildl(A, opts);
   
    C = S*A*S;
    B = C(p,p);
    fprintf('The relative residual is %d.\n', norm(B - l*d*l', 1)/norm(B, 1));
    fprintf('The fill factor is %.3f.\n', nnz(l+d+l')/nnz(B));
    fprintf('The largest elem. of L is %.3f.\n', full(max(max(abs(l)))));
	fprintf('A has %i nnz.\n', nnz(A));
    fprintf('The condition number is %d.\n', condest(B));

    %spy(A(p,p)); figure; spy(abs(l*d*l') > 1e-8);
    %spy(B); figure; spy(A(p,p));
    
    e = ones(size(B,1),1);
    %[y, flag, relres, iter, resvec] = ...
        %gmres(B,S^(-1)*e,min(60,size(B,1)),1e-8,3,l*d, l');
        bicgstab(B,S^(-1)*e,1e-8,min(1000,size(B,1)),l*d, l');
    
    %semilogy(1:length(resvec), resvec, 'r-');

    %xlabel('iteration');
    %ylabel('relative residual');
    fprintf('\n');
end

warning on;
display('================ending tests================');
