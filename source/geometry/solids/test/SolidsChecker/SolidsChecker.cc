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
//
// $Id: SolidsChecker.cc 66356 2012-12-18 09:02:32Z gcosmo $
//
// 
// --------------------------------------------------------------
//      GEANT4 - test10 
//
// --------------------------------------------------------------
// Comments
//
// 
// --------------------------------------------------------------
#include "Sc01DetectorConstruction.hh"
#include "Sc01RunAction.hh"
#include "Sc01EventAction.hh"
#include "SteppingAction.hh"
#include "Sc01PrimaryGeneratorAction.hh"
#include "Sc01PhysicsList.hh"
#include "Sc01VisManager.hh"

#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4RunManager.hh"
#include "G4UItcsh.hh"

#include "G4ios.hh"

int main(int argc,char** argv) {

  // Set the default random engine to RanecuEngine
  CLHEP::RanecuEngine defaultEngine;
  CLHEP::HepRandom::setTheEngine(&defaultEngine);

  // Run manager
  G4RunManager * runManager = new G4RunManager;

  // UserInitialization classes
  Sc01DetectorConstruction* detector =  new Sc01DetectorConstruction();
  runManager->SetUserInitialization(detector);
  runManager->SetUserInitialization(new Sc01PhysicsList);

  // UserAction classes
  runManager->SetUserAction(new Sc01RunAction);
  runManager->SetUserAction(new Sc01EventAction);
  runManager->SetUserAction(new SteppingAction);
  runManager->SetUserAction(new Sc01PrimaryGeneratorAction(detector));

#ifdef G4VIS_USE
  G4cout << "Visualization init\n";
  G4VisManager* visManager = new Sc01VisManager();
  visManager -> Initialize ();
#endif



  // User interactions
  // Define (G)UI for interactive mode

  G4UImanager * UI = G4UImanager::GetUIpointer();

  if( argc == 1 )
  { 
    // G4UIterminal is a (dumb) terminal.
    G4UIsession * session = 0;
#ifdef G4UI_USE_TCSH
      session = new G4UIterminal(new G4UItcsh);      
#else
      session = new G4UIterminal();
#endif    
    UI->ApplyCommand("/control/execute vis.mac");
    session->SessionStart();
    delete session;
  }
  else
  // Batch mode
  { 
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UI->ApplyCommand(command+fileName);
  }

  delete runManager;
  return 0;
}

