#include <iostream>
#include "UnitTest++.h"

#include "Teuchos_RCP.hpp"
#include "Teuchos_ParameterList.hpp"

#include "Epetra_MpiComm.h"
#include "Epetra_SerialComm.h"
#include "Epetra_Vector.h"

#include "Mesh_STK.hh"
#include "MeshFactory.hh"
#include "CompositeVector.hh"
#include "TreeVector.hh"


#include "SolutionHistory.hh"

using namespace Amanzi;

SUITE(SolutionHistoryTests) {

  // data structures for testing
  struct test_data {
    Epetra_MpiComm *comm;
    Teuchos::RCP<AmanziMesh::Mesh> mesh;

    Teuchos::RCP<CompositeVector> x;
    Teuchos::RCP<TreeVector> x_tree;

    test_data() {
      comm = new Epetra_MpiComm(MPI_COMM_SELF);
      AmanziMesh::MeshFactory mesh_fact(*comm);
      mesh = mesh_fact(0.0, 0.0, 0.0, 2.0, 1.0, 1.0, 2, 1, 1);

      // non-ghosted x
      x = Teuchos::rcp(new CompositeVector(mesh, "cell", AmanziMesh::CELL, 1, false));
      x_tree = Teuchos::rcp(new TreeVector("x"));
      x_tree->set_data(x);
    }
    ~test_data() {
      delete comm;
    }
  };

  TEST_FIXTURE(test_data, SolutionHistory_1) {
    cout << "Test: SolutionHistory_1" << endl;

    // create a solution history of size three
    Amanzi::SolutionHistory SH(3, 0.0, x_tree);

    x_tree->PutScalar(1.0);
    CHECK_EQUAL(SH.history_size(),1);
    CHECK_EQUAL(SH.most_recent_time(),0.0);

    SH.record_solution(1.0,x_tree);

    CHECK_EQUAL(SH.history_size(),2);
    CHECK_EQUAL(SH.most_recent_time(),1.0);

    x_tree->PutScalar(4.0);
    SH.record_solution(2.0,x_tree);

    CHECK_EQUAL(SH.history_size(),3);
    CHECK_EQUAL(SH.most_recent_time(),2.0);

    x_tree->PutScalar(9.0);
    SH.record_solution(3.0,x_tree);

    CHECK_EQUAL(SH.history_size(),3);
    CHECK_EQUAL(SH.most_recent_time(),3.0);

    // check that the most recent vector in fact is the
    // one that contains  9.0
    Teuchos::RCP<Amanzi::TreeVector>  y_tree = Teuchos::rcp(new Amanzi::TreeVector(std::string("test vec"), *x_tree));

    SH.most_recent_solution(y_tree);

    double *norminf = new double[1];
    y_tree->NormInf(norminf); // compute the max norm of the computed difference
    CHECK_EQUAL(*norminf,9.0);

    std::vector<double> h;
    SH.time_deltas(h);

    CHECK_EQUAL(h.size(),SH.history_size()-1);
    CHECK_EQUAL(h[0], 1.0);
    CHECK_EQUAL(h[1], 2.0);

    // interpolate 1st order
    SH.interpolate_solution(2.5, y_tree, 1);

    // we should get 6.5
    y_tree->NormInf(norminf);
    CHECK_EQUAL(norminf[0],6.5);

    // interpolate 2nd order
    SH.interpolate_solution(2.5, y_tree, 2);

    // from the quadratic through (1,1),(2,4),(3,9)
    // we should get 6.25
    // x_tree->PutScalar(6.25);
    // x_tree->Update(-1.0,*y_tree,1.0);
    y_tree->NormInf(norminf);
    CHECK_EQUAL(norminf[0],6.25);

    // interpolate maximum order (should be 2nd)
    SH.interpolate_solution(2.5, y_tree);

    // from the quadratic through (1,1),(2,4),(3,9)
    // we should get 6.25
    y_tree->NormInf(norminf);
    CHECK_EQUAL(norminf[0],6.25);
  }

  TEST_FIXTURE(test_data, SolutionHistory_2) {
    cout << "Test: SolutionHistory_2" << endl;
    Teuchos::RCP<Amanzi::TreeVector> xdot_tree = Teuchos::rcp(new Amanzi::TreeVector(std::string("xdot"), *x_tree));

    x_tree->PutScalar(0.0);
    xdot_tree->PutScalar(0.0);
    // create a solution history of size three
    Amanzi::SolutionHistory SH(4, 0.0, x_tree, xdot_tree);

    x_tree->PutScalar(1.0);
    xdot_tree->PutScalar(2.0);
    SH.record_solution(1.0,x_tree,xdot_tree);

    Teuchos::RCP<Amanzi::TreeVector> y_tree = Teuchos::rcp(new Amanzi::TreeVector(std::string("y"), *x_tree));

    // interpolate 1st order
    SH.interpolate_solution(0.5, y_tree, 2);

    double *norminf = new double[1];
    y_tree->NormInf(norminf);
    CHECK_EQUAL(norminf[0],0.25);

    // interpolate maximum order (3rd in this case)
    SH.interpolate_solution(0.5, y_tree);

    y_tree->NormInf(norminf);
    CHECK_EQUAL(norminf[0],0.25);
  }
}