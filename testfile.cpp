#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

// this is just a test file of eigen's capabilities
int main() {

    MatrixXd m(2,2);
    m(0,0) = 3;
    m(1,0) = 2.5;
    m(0,1) = -1;
    m(1,1) = m(1,0) + m(0,1);
    cout << m << endl;

    cout << endl;
    
    MatrixXf A = MatrixXf::Random(3,3);
    A = (A + MatrixXf::Constant(3,3,1.2)) * 50;
    cout << "A =" << endl << A << endl;
    VectorXf v(3);
    v << 1, 2, 3;
    cout << "A * v =" << endl << A * v << endl;
    
    cout << endl;
    
    Matrix3f B;
    Vector3f b;
    B << 1,2,3,  4,5,6,  7,8,10;
    b << 3, 3, 4;
    cout << "Here is the matrix B:\n" << B << endl;
    cout << "Here is the vector b:\n" << b << endl;
    Vector3f x = B.colPivHouseholderQr().solve(b);
    cout << "The solution is:\n" << x << endl;

    Matrix2f C;
    C << 0, 2, -2, 0;
    cout << "Here is the matrix C:\n" << C << endl;
    EigenSolver<Matrix2f> eigensolver(C);
    if (eigensolver.info() != Success) abort();
    cout << "The eigenvalues of C are:\n" << eigensolver.eigenvalues() << endl;
    cout << "Here's a matrix whose columns are eigenvectors of C \n"
         << "corresponding to these eigenvalues:\n"
         << eigensolver.eigenvectors() << endl;
          
    return 0;
}
