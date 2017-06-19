/*
  The three phase energy model is an algebraic model with dependencies.

  Generated via evaluator_generator with:
Energy for a three-phase, gas+liquid+ice evaluator.
    
  Authors: Ethan Coon (ecoon@lanl.gov)
*/

#include "Teuchos_ParameterList.hpp"
#include "dbc.hh"
#include "three_phase_energy_model.hh"

namespace Amanzi {
namespace Energy {
namespace Relations {

// Constructor from ParameterList
ThreePhaseEnergyModel::ThreePhaseEnergyModel(Teuchos::ParameterList& plist)
{
  InitializeFromPlist_(plist);
}


// Initialize parameters
void
ThreePhaseEnergyModel::InitializeFromPlist_(Teuchos::ParameterList& plist)
{

}


// main method
double
ThreePhaseEnergyModel::Energy(double phi, double sl, double nl, double ul, double si, double ni, double ui, double sg, double ng, double ug, double rho_r, double ur, double cv) const
{
  return cv*(phi*(ng*sg*ug + ni*si*ui + nl*sl*ul) + rho_r*ur*(-phi + 1));
}

double
ThreePhaseEnergyModel::DEnergyDPorosity(double phi, double sl, double nl, double ul, double si, double ni, double ui, double sg, double ng, double ug, double rho_r, double ur, double cv) const
{
  return cv*(ng*sg*ug + ni*si*ui + nl*sl*ul - rho_r*ur);
}

double
ThreePhaseEnergyModel::DEnergyDSaturationLiquid(double phi, double sl, double nl, double ul, double si, double ni, double ui, double sg, double ng, double ug, double rho_r, double ur, double cv) const
{
  return cv*nl*phi*ul;
}

double
ThreePhaseEnergyModel::DEnergyDMolarDensityLiquid(double phi, double sl, double nl, double ul, double si, double ni, double ui, double sg, double ng, double ug, double rho_r, double ur, double cv) const
{
  return cv*phi*sl*ul;
}

double
ThreePhaseEnergyModel::DEnergyDInternalEnergyLiquid(double phi, double sl, double nl, double ul, double si, double ni, double ui, double sg, double ng, double ug, double rho_r, double ur, double cv) const
{
  return cv*nl*phi*sl;
}

double
ThreePhaseEnergyModel::DEnergyDSaturationIce(double phi, double sl, double nl, double ul, double si, double ni, double ui, double sg, double ng, double ug, double rho_r, double ur, double cv) const
{
  return cv*ni*phi*ui;
}

double
ThreePhaseEnergyModel::DEnergyDMolarDensityIce(double phi, double sl, double nl, double ul, double si, double ni, double ui, double sg, double ng, double ug, double rho_r, double ur, double cv) const
{
  return cv*phi*si*ui;
}

double
ThreePhaseEnergyModel::DEnergyDInternalEnergyIce(double phi, double sl, double nl, double ul, double si, double ni, double ui, double sg, double ng, double ug, double rho_r, double ur, double cv) const
{
  return cv*ni*phi*si;
}

double
ThreePhaseEnergyModel::DEnergyDSaturationGas(double phi, double sl, double nl, double ul, double si, double ni, double ui, double sg, double ng, double ug, double rho_r, double ur, double cv) const
{
  return cv*ng*phi*ug;
}

double
ThreePhaseEnergyModel::DEnergyDMolarDensityGas(double phi, double sl, double nl, double ul, double si, double ni, double ui, double sg, double ng, double ug, double rho_r, double ur, double cv) const
{
  return cv*phi*sg*ug;
}

double
ThreePhaseEnergyModel::DEnergyDInternalEnergyGas(double phi, double sl, double nl, double ul, double si, double ni, double ui, double sg, double ng, double ug, double rho_r, double ur, double cv) const
{
  return cv*ng*phi*sg;
}

double
ThreePhaseEnergyModel::DEnergyDDensityRock(double phi, double sl, double nl, double ul, double si, double ni, double ui, double sg, double ng, double ug, double rho_r, double ur, double cv) const
{
  return cv*ur*(-phi + 1);
}

double
ThreePhaseEnergyModel::DEnergyDInternalEnergyRock(double phi, double sl, double nl, double ul, double si, double ni, double ui, double sg, double ng, double ug, double rho_r, double ur, double cv) const
{
  return cv*rho_r*(-phi + 1);
}

double
ThreePhaseEnergyModel::DEnergyDCellVolume(double phi, double sl, double nl, double ul, double si, double ni, double ui, double sg, double ng, double ug, double rho_r, double ur, double cv) const
{
  return phi*(ng*sg*ug + ni*si*ui + nl*sl*ul) + rho_r*ur*(-phi + 1);
}

} //namespace
} //namespace
} //namespace
  