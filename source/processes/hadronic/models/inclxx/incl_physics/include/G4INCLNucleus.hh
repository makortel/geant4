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

/*
 * G4INCLNucleus.hh
 *
 *  \date Jun 5, 2009
 * \author Pekka Kaitaniemi
 */

#ifndef G4INCLNUCLEUS_HH_
#define G4INCLNUCLEUS_HH_

#include <list>
#include <string>

#include "G4INCLParticle.hh"
#include "G4INCLEventInfo.hh"
#include "G4INCLCluster.hh"
#include "G4INCLFinalState.hh"
#include "G4INCLStore.hh"
#include "G4INCLGlobals.hh"
#include "G4INCLParticleTable.hh"
#include "G4INCLConfig.hh"
#include "G4INCLConfigEnums.hh"
#include "G4INCLCluster.hh"
#include "G4INCLProjectileRemnant.hh"

namespace G4INCL {

  class Nucleus : public Cluster {
  public:
    Nucleus(G4int mass, G4int charge, Config const * const conf, const G4double universeRadius=-1.);
    virtual ~Nucleus();

    /// \brief Dummy copy constructor to silence Coverity warning
    Nucleus(const Nucleus &rhs);

    /// \brief Dummy assignment operator to silence Coverity warning
    Nucleus &operator=(const Nucleus &rhs);

    /**
     * Call the Cluster method to generate the initial distribution of
     * particles. At the beginning all particles are assigned as spectators.
     */
    void initializeParticles();

    /// \brief Insert a new particle (e.g. a projectile) in the nucleus.
    void insertParticle(Particle *p) {
      theZ += p->getZ();
      theA += p->getA();
      theStore->particleHasEntered(p);
      if(p->isNucleon()) {
        theNpInitial += Math::heaviside(ParticleTable::getIsospin(p->getType()));
        theNnInitial += Math::heaviside(-ParticleTable::getIsospin(p->getType()));
      }
      if(!p->isTargetSpectator()) theStore->getBook().incrementCascading();
    };

    /**
     * Apply reaction final state information to the nucleus.
     */
    void applyFinalState(FinalState *);

    G4int getInitialA() const { return theInitialA; };
    G4int getInitialZ() const { return theInitialZ; };

    /**
     * Get the list of particles that were created by the last applied final state
     */
    ParticleList const &getCreatedParticles() const { return justCreated; }

    /**
     * Get the list of particles that were updated by the last applied final state
     */
    ParticleList const &getUpdatedParticles() const { return toBeUpdated; }

    /// \brief Get the delta that could not decay
    Particle *getBlockedDelta() const { return blockedDelta; }

    /**
     * Propagate the particles one time step.
     *
     * @param step length of the time step
     */
    void propagateParticles(G4double step);

    G4int getNumberOfEnteringProtons() const { return theNpInitial; };
    G4int getNumberOfEnteringNeutrons() const { return theNnInitial; };

    /** \brief Outgoing - incoming separation energies.
     *
     * Used by CDPP.
     */
    G4double computeSeparationEnergyBalance() const {
      G4double S = 0.0;
      ParticleList const &outgoing = theStore->getOutgoingParticles();
      for(ParticleIter i=outgoing.begin(), e=outgoing.end(); i!=e; ++i) {
        const ParticleType t = (*i)->getType();
        switch(t) {
          case Proton:
          case Neutron:
          case DeltaPlusPlus:
          case DeltaPlus:
          case DeltaZero:
          case DeltaMinus:
            S += thePotential->getSeparationEnergy(*i);
            break;
          case Composite:
            S += (*i)->getZ() * thePotential->getSeparationEnergy(Proton)
              + ((*i)->getA() - (*i)->getZ()) * thePotential->getSeparationEnergy(Neutron);
            break;
          case PiPlus:
            S += thePotential->getSeparationEnergy(Proton) - thePotential->getSeparationEnergy(Neutron);
            break;
          case PiMinus:
            S += thePotential->getSeparationEnergy(Neutron) - thePotential->getSeparationEnergy(Proton);
            break;
          default:
            break;
        }
      }

      S -= theNpInitial * thePotential->getSeparationEnergy(Proton);
      S -= theNnInitial * thePotential->getSeparationEnergy(Neutron);
      return S;
    }

    /** \brief Force the decay of outgoing deltas.
     *
     * \return true if any delta was forced to decay.
     */
    G4bool decayOutgoingDeltas();

    /** \brief Force the decay of deltas inside the nucleus.
     *
     * \return true if any delta was forced to decay.
     */
    G4bool decayInsideDeltas();

    /** \brief Force the decay of unstable outgoing clusters.
     *
     * \return true if any cluster was forced to decay.
     */
    G4bool decayOutgoingClusters();

    /** \brief Force the phase-space decay of the Nucleus.
     *
     * Only applied if Z==0 or Z==A.
     *
     * \return true if the nucleus was forced to decay.
     */
    G4bool decayMe();

    /// \brief Force emission of all pions inside the nucleus.
    void emitInsidePions();

    /** \brief Compute the recoil momentum and spin of the nucleus. */
    void computeRecoilKinematics();

    /** \brief Compute the current center-of-mass position.
     *
     * \return the center-of-mass position vector [fm].
     */
    ThreeVector computeCenterOfMass() const;

    /** \brief Compute the current total energy.
     *
     * \return the total energy [MeV]
     */
    G4double computeTotalEnergy() const;

    /** \brief Compute the current excitation energy.
     *
     * \return the excitation energy [MeV]
     */
    G4double computeExcitationEnergy() const;

    /** \brief Set the incoming angular-momentum vector. */
    void setIncomingAngularMomentum(const ThreeVector &j) {
      incomingAngularMomentum = j;
    }

    /** \brief Get the incoming angular-momentum vector. */
    const ThreeVector &getIncomingAngularMomentum() const { return incomingAngularMomentum; }

    /** \brief Set the incoming momentum vector. */
    void setIncomingMomentum(const ThreeVector &p) {
      incomingMomentum = p;
    }

    /** \brief Get the incoming momentum vector. */
    const ThreeVector &getIncomingMomentum() const {
      return incomingMomentum;
    }

    /** \brief Set the initial energy. */
    void setInitialEnergy(const G4double e) { initialEnergy = e; }

    /** \brief Get the initial energy. */
    G4double getInitialEnergy() const { return initialEnergy; }

    /** \brief Get the excitation energy of the nucleus.
     *
     * Method computeRecoilKinematics() should be called first.
     */
    G4double getExcitationEnergy() const { return theExcitationEnergy; }

    ///\brief Returns true if the nucleus contains any deltas.
    inline G4bool containsDeltas() {
      ParticleList const &inside = theStore->getParticles();
      for(ParticleIter i=inside.begin(), e=inside.end(); i!=e; ++i)
        if((*i)->isDelta()) return true;
      return false;
    }

    /**
     * Print the nucleus info
     */
    std::string print();

    Store* getStore() const {return theStore; };
    void setStore(Store *s) {
      delete theStore;
      theStore = s;
    };

    G4double getInitialInternalEnergy() const { return initialInternalEnergy; };

    /** \brief Is the event transparent?
     *
     * To be called at the end of the cascade.
     **/
    G4bool isEventTransparent() const;

    /** \brief Does the nucleus give a cascade remnant?
     *
     * To be called after computeRecoilKinematics().
     **/
    G4bool hasRemnant() const { return remnant; }

    /**
     * Fill the event info which contains INCL output data
     */
    void fillEventInfo(EventInfo *eventInfo);

    G4bool getTryCompoundNucleus() { return tryCN; }

    /// \brief Return the charge number of the projectile
    G4int getProjectileChargeNumber() const { return projectileZ; }

    /// \brief Return the mass number of the projectile
    G4int getProjectileMassNumber() const { return projectileA; }

    /// \brief Set the charge number of the projectile
    void setProjectileChargeNumber(G4int n) { projectileZ=n; }

    /// \brief Set the mass number of the projectile
    void setProjectileMassNumber(G4int n) { projectileA=n; }

    /// \brief Get the transmission barrier
    G4double getTransmissionBarrier(Particle const * const p) {
      const G4double theTransmissionRadius = theDensity->getTransmissionRadius(p);
      const G4double theParticleZ = p->getZ();
      return PhysicalConstants::eSquared*(theZ-theParticleZ)*theParticleZ/theTransmissionRadius;
    }

    /// \brief Struct for conservation laws
    struct ConservationBalance {
      ThreeVector momentum;
      G4double energy;
      G4int Z, A;
    };

    /// \brief Compute charge, mass, energy and momentum balance
    ConservationBalance getConservationBalance(EventInfo const &theEventInfo, const G4bool afterRecoil) const;

    /// \brief Adjust the kinematics for complete-fusion events
    void useFusionKinematics();

    /** \brief Get the maximum allowed radius for a given particle.
     *
     * Calls the NuclearDensity::getMaxRFromP() method for nucleons and deltas,
     * and the NuclearDensity::getTrasmissionRadius() method for pions.
     *
     * \param particle pointer to a particle
     * \return surface radius
     */
    G4double getSurfaceRadius(Particle const * const particle) const {
      if(particle->isPion())
        // Temporarily set RPION = RMAX
        return getUniverseRadius();
        //return 0.5*(theDensity->getTransmissionRadius(particle)+getUniverseRadius());
      else {
        const G4double pr = particle->getReflectionMomentum()/thePotential->getFermiMomentum(particle);
        if(pr>=1.)
          return getUniverseRadius();
        else
          return theDensity->getMaxRFromP(particle->getType(), pr);
      }
    }

    /// \brief Getter for theUniverseRadius.
    G4double getUniverseRadius() const { return theUniverseRadius; }

    /// \brief Setter for theUniverseRadius.
    void setUniverseRadius(const G4double universeRadius) { theUniverseRadius=universeRadius; }

    /// \brief Is it a nucleus-nucleus collision?
    G4bool isNucleusNucleusCollision() const { return isNucleusNucleus; }

    /// \brief Set a nucleus-nucleus collision
    void setNucleusNucleusCollision() { isNucleusNucleus=true; }

    /// \brief Set a particle-nucleus collision
    void setParticleNucleusCollision() { isNucleusNucleus=false; }

    /// \brief Set the projectile remnant
    void setProjectileRemnant(ProjectileRemnant * const c) {
      delete theProjectileRemnant;
      theProjectileRemnant = c;
    }

    /// \brief Get the projectile remnant
    ProjectileRemnant *getProjectileRemnant() const { return theProjectileRemnant; }

    /// \brief Delete the projectile remnant
    void deleteProjectileRemnant() {
      delete theProjectileRemnant;
      theProjectileRemnant = NULL;
    }

    /** \brief Finalise the projectile remnant
     *
     * Complete the treatment of the projectile remnant. If it contains
     * nucleons, assign its excitation energy and spin. Move stuff to the
     * outgoing list, if appropriate.
     *
     * \param emissionTime the emission time of the projectile remnant
     */
    void finalizeProjectileRemnant(const G4double emissionTime);

    /// \brief Update the particle potential energy.
    inline void updatePotentialEnergy(Particle *p) const {
      p->setPotentialEnergy(thePotential->computePotentialEnergy(p));
    }

    /// \brief Setter for theDensity
    void setDensity(NuclearDensity const * const d) {
      theDensity=d;
      if(theParticleSampler)
        theParticleSampler->setDensity(theDensity);
    };

    /// \brief Getter for theDensity
    NuclearDensity const *getDensity() const { return theDensity; };

    /// \brief Getter for thePotential
    NuclearPotential::INuclearPotential const *getPotential() const { return thePotential; };

  private:
    /** \brief Compute the recoil kinematics for a 1-nucleon remnant.
     *
     * Puts the remnant nucleon on mass shell and tries to enforce approximate
     * energy conservation by modifying the masses of the outgoing particles.
     */
    void computeOneNucleonRecoilKinematics();

  private:
    G4int theInitialZ, theInitialA;
    /// \brief The number of entering protons
    G4int theNpInitial;
    /// \brief The number of entering neutrons
    G4int theNnInitial;
    G4double initialInternalEnergy;
    ThreeVector incomingAngularMomentum, incomingMomentum;
    ThreeVector initialCenterOfMass;
    G4bool remnant;

    ParticleList toBeUpdated;
    ParticleList justCreated;
    Particle *blockedDelta;
    G4double initialEnergy;
    Store *theStore;
    G4bool tryCN;

    /// \brief The charge number of the projectile
    G4int projectileZ;
    /// \brief The mass number of the projectile
    G4int projectileA;

    /// \brief The radius of the universe
    G4double theUniverseRadius;

    /** \brief true if running a nucleus-nucleus collision
     *
     * Tells INCL whether to make a projectile-like pre-fragment or not.
     */
    G4bool isNucleusNucleus;

    /** \brief Pointer to the quasi-projectile
     *
     * Owned by the Nucleus object.
     */
    ProjectileRemnant *theProjectileRemnant;

    /// \brief Pointer to the NuclearDensity object
    NuclearDensity const *theDensity;

    /// \brief Pointer to the NuclearPotential object
    NuclearPotential::INuclearPotential const *thePotential;

  };

}

#endif /* G4INCLNUCLEUS_HH_ */
