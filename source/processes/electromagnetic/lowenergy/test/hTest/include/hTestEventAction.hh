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
#ifndef hTestEventAction_h
#define hTestEventAction_h 1
 
// -------------------------------------------------------------
//
//
// -------------------------------------------------------------
//      GEANT4 hTest
//
//      History: based on object model of
//      2nd December 1995, G.Cosmo
//      ---------- hTestEventAction -------------
//              
//  Modified: 05.04.01 Vladimir Ivanchenko new design of hTest 
// 
// -------------------------------------------------------------

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "G4UserEventAction.hh"
#include "hTestDetectorConstruction.hh"
#include "G4Event.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class hTestEventAction : public G4UserEventAction
{
public: // Without description

    hTestEventAction(const hTestDetectorConstruction*);
   ~hTestEventAction();

    void BeginOfEventAction(const G4Event*);
    void   EndOfEventAction(const G4Event*);
    
    inline void SetDrawFlag(G4String val)  {drawFlag = val;};
    inline G4int GetEventNo() const {return nEvt;};
    
  private:

    const hTestDetectorConstruction* theDet;
    G4int nEvt;
    G4int verbose;
    G4String drawFlag;
};

#endif

    