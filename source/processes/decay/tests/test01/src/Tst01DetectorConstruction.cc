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
// $Id: Tst01DetectorConstruction.cc 71045 2013-06-10 09:34:33Z gcosmo $
//

#include "Tst01DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"

Tst01DetectorConstruction::Tst01DetectorConstruction()
{
}

Tst01DetectorConstruction::~Tst01DetectorConstruction()
{
}



G4VPhysicalVolume* Tst01DetectorConstruction::Construct()
{ 
  G4double density     = universe_mean_density;    //from PhysicalConstants.h
  G4double pressure    = 3.e-18*pascal;
  G4double temperature = 2.73*kelvin;
  G4Material* vacuum = new G4Material("Vacuum", 1., 1.01*g/mole, density,kStateGas,temperature,pressure);

  G4Box * mySimpleBox = new G4Box("SBox", 1.0*km, 1.0*km, 1.0*km);
  G4LogicalVolume * simpleBoxLog = new G4LogicalVolume( mySimpleBox,vacuum ,"SLog",0,0,0);
  G4VPhysicalVolume* simpleBoxDetector = new G4PVPlacement(0,G4ThreeVector(),"SPhys",simpleBoxLog,0,false,0);

  return simpleBoxDetector;
}

