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
// $Id: G4eBremsstrahlung.cc 75582 2013-11-04 12:13:01Z gcosmo $
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:     G4eBremsstrahlung
//
// Author:        Michel Maire
//
// Creation date: 26.06.1996
//
// Modifications:
//
// 26-09-96 extension of the total crosssection above 100 GeV, M.Maire
//  1-10-96 new type G4OrderedTable; ComputePartialSumSigma(), M.Maire
// 16-10-96 DoIt() call to the non static GetEnergyCuts(), L.Urban
// 13-12-96 Sign corrected in grejmax and greject
//          error definition of screenvar, L.Urban
// 20-03-97 new energy loss+ionisation+brems scheme, L.Urban
// 07-04-98 remove 'tracking cut' of the diffracted particle, MMa
// 13-08-98 new methods SetBining() PrintInfo()
// 03-03-99 Bug fixed in LPM effect, L.Urban
// 10-02-00 modifications , new e.m. structure, L.Urban
// 07-08-00 new cross section/en.loss parametrisation, LPM flag , L.Urban
// 21-09-00 corrections in the LPM implementation, L.Urban
// 28-05-01 V.Ivanchenko minor changes to provide ANSI -wall compilation
// 09-08-01 new methods Store/Retrieve PhysicsTable (mma)
// 17-09-01 migration of Materials to pure STL (mma)
// 21-09-01 completion of RetrievePhysicsTable() (mma)
// 29-10-01 all static functions no more inlined (mma)
// 08-11-01 particleMass becomes a local variable
// 30-04-02 V.Ivanchenko update to new design
// 23-12-02 Change interface in order to move to cut per region (VI)
// 26-12-02 Secondary production moved to derived classes (VI)
// 23-05-03 Define default integral + BohrFluctuations (V.Ivanchenko)
// 08-08-03 STD substitute standard  (V.Ivanchenko)
// 12-11-03 G4EnergyLossSTD -> G4EnergyLossProcess (V.Ivanchenko)
// 04-11-04 add gamma threshold (V.Ivanchenko)
// 08-11-04 Migration to new interface of Store/Retrieve tables (V.Ivantchenko)
// 08-04-05 Major optimisation of internal interfaces (V.Ivantchenko)
// 22-05-06 Use gammaThreshold from manager (V.Ivantchenko)
// 15-01-07 use SetEmModel() from G4VEnergyLossProcess (mma)
//          use RelEmModel above 1GeV (AS &  VI)  
// 13-11-08 reenable LPM switch (A.Schaelicke)
// -------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "G4eBremsstrahlung.hh"
#include "G4SystemOfUnits.hh"
#include "G4Gamma.hh"
#include "G4SeltzerBergerModel.hh"
#include "G4eBremsstrahlungRelModel.hh"
#include "G4UnitsTable.hh"
#include "G4LossTableManager.hh"

#include "G4ProductionCutsTable.hh"
#include "G4MaterialCutsCouple.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

using namespace std;
 
G4eBremsstrahlung::G4eBremsstrahlung(const G4String& name):
  G4VEnergyLossProcess(name), 
  isInitialised(false)
{
  SetProcessSubType(fBremsstrahlung);
  SetSecondaryParticle(G4Gamma::Gamma());
  SetIonisation(false);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4eBremsstrahlung::~G4eBremsstrahlung()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4bool G4eBremsstrahlung::IsApplicable(const G4ParticleDefinition& p)
{
  return (&p == G4Electron::Electron() || &p == G4Positron::Positron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void 
G4eBremsstrahlung::InitialiseEnergyLossProcess(const G4ParticleDefinition*,
					       const G4ParticleDefinition*)
{
  if(!isInitialised) {

    //    if (!EmModel(1)) { SetEmModel(new G4eBremsstrahlungModel(), 1); }
    if (!EmModel(1)) { SetEmModel(new G4SeltzerBergerModel(), 1); }
    if (!EmModel(2)) { SetEmModel(new G4eBremsstrahlungRelModel(), 2); }

    G4double energyLimit = 1*GeV;

    EmModel(1)->SetLowEnergyLimit(MinKinEnergy());
    EmModel(1)->SetHighEnergyLimit(energyLimit);
    EmModel(2)->SetLowEnergyLimit(energyLimit);
    EmModel(2)->SetHighEnergyLimit(MaxKinEnergy());
                
    G4VEmFluctuationModel* fm = 0;
    AddEmModel(1, EmModel(1), fm);
    AddEmModel(2, EmModel(2), fm);
    isInitialised = true;
  }
  G4LossTableManager* man = G4LossTableManager::Instance(); 
  G4double eth = man->BremsstrahlungTh(); 
  EmModel(1)->SetSecondaryThreshold(eth);
  EmModel(2)->SetSecondaryThreshold(eth);

  // Only high energy model LMP flag is ON/OFF
  EmModel(1)->SetLPMFlag(false);
  EmModel(2)->SetLPMFlag(man->LPMFlag());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4eBremsstrahlung::PrintInfo()
{
  if(EmModel(1)) {
    G4LossTableManager* man = G4LossTableManager::Instance(); 
    G4double eth = man->BremsstrahlungTh(); 
    G4cout << "      LPM flag: " << man->LPMFlag() << " for E > " 
	   << EmModel(1)->HighEnergyLimit()/GeV << " GeV";
    if(eth < DBL_MAX) { 
      G4cout << ",  HighEnergyThreshold(GeV)= " << eth/GeV; 
    }
    G4cout << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.... 
