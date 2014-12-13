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
// $Id: WLSActionInitialization.cc 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file optical/wls/src/WLSActionInitialization.cc
/// \brief Implementation of the WLSActionInitialization class

#include "WLSActionInitialization.hh"
#include "WLSDetectorConstruction.hh"

#include "WLSPrimaryGeneratorAction.hh"

#include "WLSRunAction.hh"
#include "WLSEventAction.hh"
#include "WLSTrackingAction.hh"
#include "WLSSteppingAction.hh"
#include "WLSStackingAction.hh"
#include "WLSSteppingVerbose.hh"
#include "G4GeneralParticleSource.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WLSActionInitialization::WLSActionInitialization(WLSDetectorConstruction* det)
 : G4VUserActionInitialization(), fDetector(det)
{
    //AND->3June2014, temporary to take into account new GPS
    //Create an instance of GPS in master so shared resources and messenger
    //exist in master.
    masterGPS = new G4GeneralParticleSource();
    //AND<-3June2014
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WLSActionInitialization::~WLSActionInitialization()
{
  delete masterGPS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSActionInitialization::BuildForMaster() const
{
  SetUserAction(new WLSRunAction());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSActionInitialization::Build() const
{
  SetUserAction(new WLSPrimaryGeneratorAction(fDetector));

  WLSRunAction* runAction = new WLSRunAction();
  WLSEventAction* eventAction = new WLSEventAction(runAction);

  SetUserAction(runAction);
  SetUserAction(eventAction);
  SetUserAction(new WLSTrackingAction());
  SetUserAction(new WLSSteppingAction(fDetector));
  SetUserAction(new WLSStackingAction());
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSteppingVerbose* WLSActionInitialization::InitializeSteppingVerbose() const
{
  return new WLSSteppingVerbose();
}
