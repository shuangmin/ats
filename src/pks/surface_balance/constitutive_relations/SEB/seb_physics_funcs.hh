/*
  Functions for calculating the snow / surface energy balance.
*/

#ifndef SURFACEBALANCE_SEB_PHYSICS_FUNCS_HH_
#define SURFACEBALANCE_SEB_PHYSICS_FUNCS_HH_

#include <cmath>
#include <string>

#include "VerboseObject.hh"
#include "seb_physics_defs.hh"

namespace Amanzi {
namespace SurfaceBalance {
namespace SEBPhysics {

// Main SEB functions

//
// Calculates incoming shortwave and longwave radiation incident on surface
// ------------------------------------------------------------------------------------------
std::pair<double,double>
IncomingRadiation(const MetData& met, double albedo);

//
// Determine the albedo of snow as a function of density.
// ------------------------------------------------------------------------------------------
double CalcAlbedoSnow(double density_snow);

//
// Determine the surface roughness
// ------------------------------------------------------------------------------------------
double CalcRoughnessFactor(double snow_height, double Z_rough_bare, double Z_rough_snow);

//
// Calculate longwave from air temp and relative humidity
// ------------------------------------------------------------------------------------------
double CalcIncomingLongwave(double air_temp, double relative_humidity, double c_stephan_boltzmann);

//
// Calculates outgoing longwave radiation
// ------------------------------------------------------------------------------------------
double OutgoingRadiation(double temp, double emissivity, double c_stephan_boltzmann);

//
// Wind speed term D_he
// ------------------------------------------------------------------------------------------
double WindFactor(double Us, double Z_Us, double Z_rough, double c_von_Karman);

// 
// Stability of convective overturning term Zeta AKA Sqig
// ------------------------------------------------------------------------------------------
double StabilityFunction(double air_temp, double skin_temp, double Us,
                         double Z_Us, double c_gravity);


// 
// Partial pressure of water vapor in air, saturated.
// After Dingman D-7 (Bolton, 1980).
// In [kPa]
// ------------------------------------------------------------------------------------------
double SaturatedVaporPressure(double temp);

// 
// Partial pressure of water vapor in air.
// In [kPa]
// ------------------------------------------------------------------------------------------
double VaporPressureAir(double air_temp, double relative_humidity);

// 
// Partial pressure of water vapor in gaseous phase, in the soil.
// After Ho & Webb 2006
// In [kPa]
// ------------------------------------------------------------------------------------------
double VaporPressureGround(const GroundProperties& surf, const ModelParams& params);


// 
// Diffusion of vapor pressure limiter on evaporation.
// After Sakagucki and Zeng 2009 eqaution (10)
// ------------------------------------------------------------------------------------------
double EvaporativeResistanceGround(const GroundProperties& surf,
        const MetData& met,
        const ModelParams& params, 
        double vapor_pressure_air, double vapor_pressure_ground);

    
// 
// Basic sensible heat.
// ------------------------------------------------------------------------------------------
double SensibleHeat(double resistance_coef,
                    double density_air,
                    double Cp_air,
                    double air_temp,
                    double skin_temp);

// 
// Basic latent heat.
// ------------------------------------------------------------------------------------------
double LatentHeat(double resistance_coef,
                  double density_air,  /// this should be w?
                  double latent_heat_fusion,
                  double vapor_pressure_air,
                  double vapor_pressure_skin,
                  double Apa);

// 
// Heat conducted to ground via simple diffusion model between snow and skin surface.
// ------------------------------------------------------------------------------------------
double ConductedHeatIfSnow(double ground_temp,
                           const SnowProperties& snow);

// 
// Update the energy balance, solving for the amount of heat available to melt snow.
//
// NOTE, this should not be used directly -- instead it is called within the loop solving for
// snow temperature.
// ------------------------------------------------------------------------------------------
void UpdateEnergyBalanceWithSnow(const GroundProperties& surf,
        const SnowProperties& snow,
        const MetData& met,
        const ModelParams& params,
        EnergyBalance& eb);

// 
// Determine the snow temperature by solving for energy balance, i.e. the snow
// temp at equilibrium.  Assumes no melting (and therefore T_snow calculated
// can be greater than 0 C.
// ------------------------------------------------------------------------------------------
double DetermineSnowTemperature(const GroundProperties& surf,
        const MetData& met,
        const ModelParams& params,
        SnowProperties& snow,
        EnergyBalance& eb,
        std::string method="toms");


// 
// Update the energy balance, solving for the amount of heat conducted to the ground.
//
// NOTE, this CAN be used directly.
// ------------------------------------------------------------------------------------------
void UpdateEnergyBalanceWithoutSnow(const GroundProperties& surf,
        const MetData& met,
        const ModelParams& params,
        EnergyBalance& eb);


// 
// Given an energy balance, determine the resulting mass changes between
// precip, evaporation, melt, etc.
// ------------------------------------------------------------------------------------------
MassBalance UpdateMassBalance(const GroundProperties& surf, const SnowProperties& snow_old,
        const MetData& met, const ModelParams& params, const EnergyBalance eb,
        SnowProperties& snow_new, double dt);

// 
// Main driver -- calculate the energy and mass balances.
// ------------------------------------------------------------------------------------------
std::tuple<SnowProperties, EnergyBalance, MassBalance>
CalculateSurfaceBalance(double dt,
                        const GroundProperties& surf,
                        const SnowProperties& snow_old,
                        const MetData& met,
                        const ModelParams& params,
                        bool debug, const Teuchos::RCP<VerboseObject>& vo);



// Calculation of a snow temperature requires a root-finding operation, for
// which we use a functor.
class SnowTemperatureFunctor_ {
 public:

  explicit SnowTemperatureFunctor_(
      GroundProperties const * const surf,
      SnowProperties * const snow,
      MetData const * const met,
      ModelParams const * const params,
      EnergyBalance * const eb) 
      : surf_(surf),
        params_(params),
        snow_(snow),
        met_(met),
        eb_(eb) {}

  double operator()(double temp) {
    snow_->temp = temp;
    UpdateEnergyBalanceWithSnow(*surf_, *snow_, *met_, *params_, *eb_);
    return eb_->fQm;
  }

 private:
  GroundProperties const * const surf_;
  ModelParams const * const params_;
  MetData const * const met_;

  SnowProperties * const snow_;
  EnergyBalance * const eb_;
};


// Convergence criteria for root-finding
struct Tol_ {
  Tol_(double eps) : eps_(eps) {}
  bool operator()(const double& a, const double& b) const {
    return std::abs(a - b) <= eps_;
  }
  double eps_;
};

} // namespace
} // namespace
} // namespace

#endif
