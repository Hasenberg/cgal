// Copyright (c) 2014  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
//
// Author(s) : Jocelyn Meyron and Quentin Mérigot
//

#ifndef CGAL_EIGEN_VCM_TRAITS_H
#define CGAL_EIGEN_VCM_TRAITS_H

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

#include <CGAL/array.h>

namespace CGAL {

/// A model of the concept `VCMTraits` using \ref thirdpartyEigen.
/// \cgalModels `VCMTraits`

template <typename FT, unsigned int degree = 3>
class Eigen_vcm_traits{

  typedef Eigen::Matrix<FT, degree, degree> Matrix;
  typedef Eigen::Matrix<FT, degree, 1> Vector;
  
  // Construct the covariance matrix
  static Matrix
  construct_covariance_matrix
  (const cpp11::array<FT, (degree * (degree+1) / 2)>& cov)  {
    Matrix m;

    for (std::size_t i = 0; i < degree; ++ i)
      for (std::size_t j = i; j < degree; ++ j)
	{
	  m(i,j) = static_cast<float>(cov[(degree * i) + j - ((i * (i+1)) / 2)]);
	  if (i != j)
	    m(j,i) = m(i,j);
	}

    return m;
  }

  // Diagonalize a selfadjoint matrix
  static bool
  diagonalize_selfadjoint_matrix (Matrix& m,
				  Matrix& eigenvectors,
                                  Vector& eigenvalues) {
      Eigen::SelfAdjointEigenSolver<Matrix> eigensolver(m);

      if (eigensolver.info() != Eigen::Success) {
          return false;
      }

      eigenvalues = eigensolver.eigenvalues();
      eigenvectors = eigensolver.eigenvectors();

      return true;
  }

public:
  static bool
  diagonalize_selfadjoint_covariance_matrix(
    const cpp11::array<FT, (degree * (degree+1) / 2)>& cov,
    cpp11::array<FT, degree>& eigenvalues)
  {
    Matrix m = construct_covariance_matrix(cov);

    // Diagonalizing the matrix
    Vector eigenvalues_;
    Matrix eigenvectors_;
    bool res = diagonalize_selfadjoint_matrix(m, eigenvectors_, eigenvalues_);

    if (res)
    {
      for (std::size_t i = 0; i < degree; ++ i)
	eigenvalues[i] = static_cast<FT>(eigenvalues_[i]);
    }

    return res;
  }

  static bool
  diagonalize_selfadjoint_covariance_matrix(
    const cpp11::array<FT, (degree * (degree+1) / 2)>& cov,
    cpp11::array<FT, degree>& eigenvalues,
    cpp11::array<FT, degree * degree>& eigenvectors)
  {
    Matrix m = construct_covariance_matrix(cov);

    // Diagonalizing the matrix
    Vector eigenvalues_;
    Matrix eigenvectors_;
    bool res = diagonalize_selfadjoint_matrix(m, eigenvectors_, eigenvalues_);

    if (res)
    {
      for (std::size_t i = 0; i < degree; ++ i)
	{
	  eigenvalues[i] = static_cast<FT>(eigenvalues_[i]);

	  for (std::size_t j = 0; j < degree; ++ j)
	    eigenvectors[degree*i + j]=static_cast<FT>(eigenvectors_(j,i));
	}
    }

    return res;
  }

  // Extract the eigenvector associated to the largest eigenvalue
  static bool
  extract_largest_eigenvector_of_covariance_matrix (
    const cpp11::array<FT, (degree * (degree+1) / 2)>& cov,
    cpp11::array<FT,degree> &normal)
  {
      // Construct covariance matrix
      Matrix m = construct_covariance_matrix(cov);

      // Diagonalizing the matrix
      Vector eigenvalues;
      Matrix eigenvectors;
      if (! diagonalize_selfadjoint_matrix(m, eigenvectors, eigenvalues)) {
          return false;
      }

      // Eigenvalues are already sorted by increasing order
      for (unsigned int i = 0; i < degree; ++ i)
	normal[i] = static_cast<FT> (eigenvectors(i,0));

      return true;
  }
};

} // namespace CGAL

#endif // CGAL_EIGEN_VCM_TRAITS_H
