//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// INCL++ intra-nuclear cascade model
// Pekka Kaitaniemi, CEA and Helsinki Institute of Physics
// Davide Mancusi, CEA
// Alain Boudard, CEA
// Sylvie Leray, CEA
// Joseph Cugnon, University of Liege
//
#define INCLXX_IN_GEANT4_MODE 1

#include "globals.hh"

#ifndef G4INCLConfig_hh
#define G4INCLConfig_hh 1

#include "G4INCLParticleSpecies.hh"
#include "G4INCLConfigEnums.hh"
#include "G4INCLRandomSeedVector.hh"
#include <iostream>
#include <string>
#include <sstream>
// #include <cassert>

#if defined(HAS_BOOST_PROGRAM_OPTIONS) && !defined(INCLXX_IN_GEANT4_MODE)
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <fstream>
#include <cstdlib>

namespace po = boost::program_options;
#endif

namespace G4INCL {

  /**
   * The INCL configuration object
   *
   * The Config object keeps track of various INCL physics options
   * (e.g. which Pauli blocking scheme to use, whether to use local
   * energy option or not, etc.
   */
  class Config {
  public:
    /// \brief Default constructor
    Config();

    /**
     * Constructor for INCL++ with specified target A, Z, projectile
     * type and energy. All other options are the default ones.
     */
    Config(G4int, G4int, ParticleSpecies, G4double);

    /** \brief Constructor based on command-line and config-file options.
     *
     * \param argc command-line parameters
     * \param argv command-line parameters
     * \param isFullRun is this a real calculation: true = yes; false = no, it's just a unit test
     */
    Config(G4int argc, char *argv[], G4bool isFullRun);

    /// \brief Default destructor
    ~Config();

    /// \brief Initialise the members
    void init();

    /// \brief Return a summary of the run configuration.
    std::string summary();

    /// \brief Get the verbosity.
    G4int getVerbosity() const { return verbosity; }

    /// \brief Get the run title.
    std::string const &getCalculationTitle() const { return title; }

    /// \brief Get the output file root.
    std::string const &getOutputFileRoot() const { return outputFileRoot; }

    /// \brief Get the number of shots.
    G4int getNumberOfShots() const { return nShots; }

    /// \brief Natural targets.
    G4bool isNaturalTarget() const { return naturalTarget; }

    /** \brief Get the target mass number.
     *
     * Note that A==0 means natural target. You should first check the
     * isNaturalTarget() method.
     */
    G4int getTargetA() const { return targetSpecies.theA; }

    /// \brief Get the target charge number.
    G4int getTargetZ() const { return targetSpecies.theZ; }

    /// \brief Set target mass number
    void setTargetA(G4int A) { targetSpecies.theA = A; }

    /// \brief Set target charge number
    void setTargetZ(G4int Z) { targetSpecies.theZ = Z; }

    /// \brief Get the projectile type
    ParticleType getProjectileType() const { return projectileSpecies.theType; }

    /// \brief Get the projectile species
    ParticleSpecies getProjectileSpecies() const { return projectileSpecies; }

    /// \brief Set the projectile species
    void setProjectileSpecies(ParticleSpecies const &ps) { projectileSpecies=ps; }

    /// \brief Get the projectile kinetic energy.
    G4double getProjectileKineticEnergy() const { return projectileKineticEnergy; }

    /// \brief Set the projectile kinetic energy.
    void setProjectileKineticEnergy(G4double const kinE) { projectileKineticEnergy=kinE; }

    /// \brief Get the number of the verbose event.
    G4int getVerboseEvent() const { return verboseEvent; }

    /// \brief Get the INCL version ID.
    static std::string const getVersionID();

    /// \brief Get the INCL version hash.
    static std::string const getVersionHash();

    /// \brief Get the INCL version string.
    static std::string const getVersionString() {
      std::stringstream ss;
      ss << getVersionID() << "-" << getVersionHash();
      return ss.str();
    }

    /// \brief Get the seeds for the random-number generator.
    Random::SeedVector getRandomSeeds() const {
      Random::SeedVector s;
      s.push_back(randomSeed1);
      s.push_back(randomSeed2);
      return s;
    }

    /// \brief Get the Pauli-blocking algorithm.
    PauliType getPauliType() const { return pauliType; }

    /// \brief Do we want CDPP?
    G4bool getCDPP() const { return CDPP; }

    /// \brief Do we want the pion potential?
    G4bool getPionPotential() const { return pionPotential; }

    /// \brief Get the Coulomb-distortion algorithm.
    CoulombType getCoulombType() const { return coulombType; }

    /// \brief Get the type of the potential for nucleons.
    PotentialType getPotentialType() const { return potentialType; }

    /// \brief Set the type of the potential for nucleons.
    void setPotentialType(PotentialType type) { potentialType = type; }

    /// \brief Set the type of the potential for nucleons.
    void setPionPotential(const G4bool pionPot) { pionPotential = pionPot; }

    /// \brief Get the type of local energy for N-N avatars.
    LocalEnergyType getLocalEnergyBBType() const { return localEnergyBBType; }

    /// \brief Get the type of local energy for pi-N and decay avatars.
    LocalEnergyType getLocalEnergyPiType() const { return localEnergyPiType; }

    /// \brief Set the type of local energy for N-N avatars.
    void setLocalEnergyBBType(const LocalEnergyType t) { localEnergyBBType=t; }

    /// \brief Set the type of local energy for N-N avatars.
    void setLocalEnergyPiType(const LocalEnergyType t) { localEnergyPiType=t; }

    /// \brief Get the log file name.
    std::string const &getLogFileName() const { return logFileName; }

    /// \brief Get the de-excitation model.
    DeExcitationType getDeExcitationType() const { return deExcitationType; }

    /// \brief Get the de-excitation string.
    std::string getDeExcitationString() const { return deExcitationString; }

    /// \brief Get the clustering algorithm.
    ClusterAlgorithmType getClusterAlgorithm() const { return clusterAlgorithmType; }

    /// \brief Get the maximum mass for production of clusters.
    G4int getClusterMaxMass() const { return clusterMaxMass; }

    /// \brief Set the maximum mass for production of clusters.
    void setClusterMaxMass(const G4int m){ clusterMaxMass=m; }

    /// \brief Get back-to-spectator
    G4bool getBackToSpectator() const { return backToSpectator; }

    /// \brief Whether to use real masses
    G4bool getUseRealMasses() const { return useRealMasses; }

    /// \brief Set whether to use real masses
    void setUseRealMasses(G4bool use) { useRealMasses = use; }

    std::string const &getINCLXXDataFilePath() const {
      return INCLXXDataFilePath;
    }

#ifdef INCL_DEEXCITATION_ABLAXX
    std::string const &getABLAv3pCxxDataFilePath() const {
      return ablav3pCxxDataFilePath;
    }
#endif

#ifdef INCL_DEEXCITATION_ABLA07
    std::string const &getABLA07DataFilePath() const {
      return abla07DataFilePath;
    }
#endif
#ifdef INCL_DEEXCITATION_GEMINIXX
    std::string const &getGEMINIXXDataFilePath() const {
      return geminixxDataFilePath;
    }
#endif

    G4double getImpactParameter() const { return impactParameter; }

    /// \brief Get the separation-energy type
    SeparationEnergyType getSeparationEnergyType() const { return separationEnergyType; }

    /// \brief Get the Fermi-momentum type
    FermiMomentumType getFermiMomentumType() const { return fermiMomentumType; }

    /// \brief Set the Fermi-momentum type
    void setFermiMomentumType(FermiMomentumType const f) { fermiMomentumType=f; }

    G4double getCutNN() const { return cutNN; }

#ifdef INCL_ROOT_USE
    std::string const &getROOTSelectionString() const {
      return rootSelectionString;
    }
#endif

#ifdef INCL_DEEXCITATION_FERMI_BREAKUP
    G4int getMaxMassFermiBreakUp() const {
      return maxMassFermiBreakUp;
    }
#endif

    /// \brief Get the r-p correlation coefficient
    G4double getRPCorrelationCoefficient(const ParticleType t) const {
// assert(t==Proton || t==Neutron);
      return ((t==Proton) ? rpCorrelationCoefficientProton : rpCorrelationCoefficientNeutron);
    }

    /// \brief Set the r-p correlation coefficient
    void setRPCorrelationCoefficient(const ParticleType t, const G4double corrCoeff) {
// assert(t==Proton || t==Neutron);
      if(t==Proton)
        rpCorrelationCoefficientProton=corrCoeff;
      else
        rpCorrelationCoefficientNeutron=corrCoeff;
    }

    /// \brief Set the r-p correlation coefficient
    void setRPCorrelationCoefficient(const G4double corrCoeff) {
      setRPCorrelationCoefficient(Proton,corrCoeff);
      setRPCorrelationCoefficient(Neutron,corrCoeff);
    }

    /// \brief Get the neutron-skin thickness
    G4double getNeutronSkinThickness() const { return neutronSkinThickness; }

    /// \brief Set the neutron-skin thickness
    void setNeutronSkinThickness(const G4double d) { neutronSkinThickness=d; }

    /// \brief Get the neutron-skin additional diffuseness
    G4double getNeutronSkinAdditionalDiffuseness() const { return neutronSkinAdditionalDiffuseness; }

    /// \brief Set the neutron-skin additional diffuseness
    void setNeutronSkinAdditionalDiffuseness(const G4double d) { neutronSkinAdditionalDiffuseness=d; }

    /// \brief True if we should use refraction
    G4bool getRefraction() const { return refraction; }

    /// \brief Set the refraction variable
    void setRefraction(const G4bool r) { refraction = r; }

#if defined(HAS_BOOST_PROGRAM_OPTIONS) && !defined(INCLXX_IN_GEANT4_MODE)
    /// \brief Echo the input options.
    std::string const echo() const;
#endif

  private:

#if defined(HAS_BOOST_PROGRAM_OPTIONS) && !defined(INCLXX_IN_GEANT4_MODE)
    std::string echoOptionsDescription(const po::options_description &aDesc) const;

    po::options_description runOptDesc;
    po::options_description hiddenOptDesc;
    po::options_description genericOptDesc;
    po::options_description physicsOptDesc;
    po::variables_map variablesMap;
#endif

    G4int verbosity;
    std::string inputFileName;
    std::string title;
    std::string outputFileRoot;
    std::string fileSuffix;
    std::string logFileName;

    G4int nShots;

    std::string targetString;
    ParticleSpecies targetSpecies;
    G4bool naturalTarget;

    std::string projectileString;
    ParticleSpecies projectileSpecies;
    G4double projectileKineticEnergy;

    G4int verboseEvent;

    G4int randomSeed1, randomSeed2;
    static const G4int randomSeedMin, randomSeedMax;

    std::string pauliString;
    PauliType pauliType;
    G4bool CDPP;

    std::string coulombString;
    CoulombType coulombType;

    std::string potentialString;
    PotentialType potentialType;
    G4bool pionPotential;

    std::string localEnergyBBString;
    LocalEnergyType localEnergyBBType;

    std::string localEnergyPiString;
    LocalEnergyType localEnergyPiType;

    std::string deExcitationModelList;
    std::string deExcitationOptionDescription;
    std::string deExcitationString;
    DeExcitationType deExcitationType;
#ifdef INCL_DEEXCITATION_ABLAXX
    std::string ablav3pCxxDataFilePath;
#endif
#ifdef INCL_DEEXCITATION_ABLA07
    std::string abla07DataFilePath;
#endif
#ifdef INCL_DEEXCITATION_GEMINIXX
    std::string geminixxDataFilePath;
#endif
    std::string INCLXXDataFilePath;

    std::string clusterAlgorithmString;
    ClusterAlgorithmType clusterAlgorithmType;

    G4int clusterMaxMass;

    G4bool backToSpectator;

    G4bool useRealMasses;

    G4double impactParameter;

    std::string separationEnergyString;
    SeparationEnergyType separationEnergyType;

    std::string fermiMomentumString;
    FermiMomentumType fermiMomentumType;

    G4double cutNN;

#ifdef INCL_ROOT_USE
    std::string rootSelectionString; 
#endif

#ifdef INCL_DEEXCITATION_FERMI_BREAKUP
    G4int maxMassFermiBreakUp;
#endif

    G4double rpCorrelationCoefficient;
    G4double rpCorrelationCoefficientProton;
    G4double rpCorrelationCoefficientNeutron;

    G4double neutronSkinThickness;
    G4double neutronSkinAdditionalDiffuseness;

    G4bool refraction;
  };

}

#endif
