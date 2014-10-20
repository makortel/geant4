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
// $Id: field03.cc 66356 2012-12-18 09:02:32Z gcosmo $
//
// 
// --------------------------------------------------------------
//      GEANT 4 - TestF01 
//
// --------------------------------------------------------------
// Comments
//     
//   
// --------------------------------------------------------------

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "Randomize.hh"

#ifdef G4VIS_USE
#include "F03VisManager.hh"
#endif

#include "F03DetectorConstruction.hh"
#include "F03ElectroMagneticField.hh"
#include "F03PhysicsList.hh"
#include "F03PrimaryGeneratorAction.hh"
#include "F03RunAction.hh"
#include "F03EventAction.hh"
#include "F03SteppingAction.hh"
#include "F03SteppingVerbose.hh"

int main(int argc,char** argv) 
{

  //choose the Random engine

  HepRandom::setTheEngine(new RanecuEngine);
  
  //my Verbose output class

  G4VSteppingVerbose::SetInstance(new F03SteppingVerbose);
  
  F03ElectroMagneticField* field = new F03ElectroMagneticField() ;
    
  // Construct the default run manager

  G4RunManager * runManager = new G4RunManager;

  // set mandatory initialization classes

  F03DetectorConstruction* detector;
  detector = new F03DetectorConstruction;
  runManager->SetUserInitialization(detector);
  runManager->SetUserInitialization(new F03PhysicsList(detector));
  
#ifdef G4VIS_USE

  // visualization manager

  G4VisManager* visManager = new F03VisManager;
  visManager->Initialize();

#endif 
 
  // set user action classes

  runManager->SetUserAction(new F03PrimaryGeneratorAction(detector));

  F03RunAction* runAction = new F03RunAction;

  runManager->SetUserAction(runAction);

  F03EventAction* eventAction = new F03EventAction(runAction);

  runManager->SetUserAction(eventAction);

  F03SteppingAction* steppingAction = new F03SteppingAction(detector,
                                                            eventAction, 
                                                            runAction);
  runManager->SetUserAction(steppingAction);
  
  //Initialize G4 kernel, physics tables ...

  runManager->Initialize();
    
  // get the pointer to the User Interface manager 

  G4UImanager* UI = G4UImanager::GetUIpointer();  
 
  if (argc==1)   // Define UI terminal for interactive mode  
  { 
     G4UIsession * session = new G4UIterminal;
     UI->ApplyCommand("/control/execute init.mac");    
     session->SessionStart();
     delete session;
  }
  else           // Batch mode
  { 
     G4String command = "/control/execute ";
     G4String fileName = argv[1];
     UI->ApplyCommand(command+fileName);
  }
    
  // job termination

#ifdef G4VIS_USE
  delete visManager;
#endif  
  delete runManager;

  return 0;
}

