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
// Unit test for coherent elastic models
//
//  18.05.07 V. Grichine
//
//

#include "G4ios.hh"
#include <fstream>
#include <cmath>
#include "globals.hh"
#include "Randomize.hh"
#include "G4UnitsTable.hh"
#include <iomanip>
#include <complex>


#include "G4Element.hh"
#include "G4NistManager.hh"


#include "G4ParticleDefinition.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4Proton.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"

#include "G4DynamicParticle.hh"
#include "G4ParticleMomentum.hh"

#include "G4VEmModel.hh"

// Ionisation dE/dx check

#include "G4MollerBhabhaModel.hh"
#include "G4BraggModel.hh"
#include "G4PAIPhotonModel.hh"
#include "G4LivermoreIonisationModel.hh"
#include "G4PenelopeIonisationModel.hh"
#include "G4Penelope08IonisationModel.hh"






using namespace std;



int main()
{

  G4int i, j, k, iMax;
  G4double x;
  G4double expXrad=0., g4Xrad;

  std::ofstream writef("angle.dat", std::ios::out ) ;
  writef.setf( std::ios::scientific, std::ios::floatfield );


  G4Element*     theElement;
  G4Material*    theMaterial;
  G4NistManager* man = G4NistManager::Instance();
  man->SetVerbose(1);

  G4cout << " 1 hydrogen" << G4endl;
  G4cout << " 2 helium" << G4endl;
  G4cout << " 4 berillium" << G4endl;
  G4cout << " 6 carbon" << G4endl;
  G4cout << " 7 nitrogen" << G4endl;
  G4cout << " 8 oxigen" << G4endl;
  G4cout << "10 neon" << G4endl;
  G4cout << "13 aluminium" << G4endl;
  G4cout << "14 silicon" << G4endl;
  G4cout << "18 argon" << G4endl;
  G4cout << "26 iron" << G4endl;
  G4cout << "29 copper" << G4endl;
  G4cout << "48 cadmium" << G4endl;
  G4cout << "54 xenon" << G4endl;
  G4cout << "74 tugnsten" << G4endl;
  G4cout << "77 iridium" << G4endl;
  G4cout << "79 gold" << G4endl;
  G4cout << "82 lead" << G4endl;
  G4cout << "92 uranium" << G4endl;
  G4cout << "200 water" << G4endl;
  G4cout << "201 CO2" << G4endl;


  G4int choice;
  // G4cin >> choice;
  choice = 200;


  switch (choice)
  {
    case 1:

      theElement  = man->FindOrBuildElement("H");
      theMaterial = man->FindOrBuildMaterial("G4_H");
      g4Xrad = theMaterial->GetRadlen();
      break;

    case 2:

      theElement  = man->FindOrBuildElement("He");
      theMaterial = man->FindOrBuildMaterial("G4_He");
      g4Xrad = theMaterial->GetRadlen();
      break;

    case 4:

      theElement  = man->FindOrBuildElement("Be");
      theMaterial = man->FindOrBuildMaterial("G4_Be");
      g4Xrad = theMaterial->GetRadlen();
      break;

    case 6:

      theElement  = man->FindOrBuildElement("C");
      theMaterial = man->FindOrBuildMaterial("G4_C");
      g4Xrad = theMaterial->GetRadlen();
      expXrad = 19.6*cm;
      break;

    case 7:

      theElement  = man->FindOrBuildElement("N");
      theMaterial = man->FindOrBuildMaterial("G4_N");
      g4Xrad = theMaterial->GetRadlen();
      break;


    case 8:

      theElement  = man->FindOrBuildElement("O");
      theMaterial = man->FindOrBuildMaterial("G4_O");
      g4Xrad = theMaterial->GetRadlen();
      break;

    case 10:

      theElement  = man->FindOrBuildElement("Ne");
      theMaterial = man->FindOrBuildMaterial("G4_Ne");
      g4Xrad = theMaterial->GetRadlen();
      break;

    case 13:

      theElement  = man->FindOrBuildElement("Al");
      theMaterial = man->FindOrBuildMaterial("G4_Al");
      g4Xrad = theMaterial->GetRadlen();
      expXrad = 8.9*cm;
      break;

    case 14:

      theElement  = man->FindOrBuildElement("Si");
      theMaterial = man->FindOrBuildMaterial("G4_Si");
      g4Xrad = theMaterial->GetRadlen();
      break;

    case 18:

      theElement  = man->FindOrBuildElement("Ar");
      theMaterial = man->FindOrBuildMaterial("G4_Ar");
      break;

    case 26:

      theElement  = man->FindOrBuildElement("Fe");
      theMaterial = man->FindOrBuildMaterial("G4_Fe");
      g4Xrad = theMaterial->GetRadlen();
      expXrad = 1.76*cm;
      break;

    case 29:

      theElement  = man->FindOrBuildElement("Cu");
      theMaterial = man->FindOrBuildMaterial("G4_Cu");
      g4Xrad = theMaterial->GetRadlen();
      break;

    case 48:

      theElement  = man->FindOrBuildElement("Cd");
      theMaterial = man->FindOrBuildMaterial("G4_Cd");
      g4Xrad = theMaterial->GetRadlen();
      break;


    case 54:

      theElement  = man->FindOrBuildElement("Xe");
      theMaterial = man->FindOrBuildMaterial("G4_Xe");
      g4Xrad = theMaterial->GetRadlen();
      break;


    case 74:

      theElement  = man->FindOrBuildElement("W");
      theMaterial = man->FindOrBuildMaterial("G4_W");
      g4Xrad = theMaterial->GetRadlen();
      expXrad = 0.35*cm;
      break;

    case 77:

      theElement  = man->FindOrBuildElement("Ir");
      theMaterial = man->FindOrBuildMaterial("G4_Ir");
      g4Xrad = theMaterial->GetRadlen();
      break;

    case 79:

      theElement  = man->FindOrBuildElement("Au");
      theMaterial = man->FindOrBuildMaterial("G4_Au");
      g4Xrad = theMaterial->GetRadlen();
      break;

    case 82:

      theElement  = man->FindOrBuildElement("Pb");
      theMaterial = man->FindOrBuildMaterial("G4_Pb");
      g4Xrad = theMaterial->GetRadlen();
      expXrad = 0.56*cm;
      break;

    case 92:

      theElement  = man->FindOrBuildElement("U");
      theMaterial = man->FindOrBuildMaterial("G4_U");
      g4Xrad = theMaterial->GetRadlen();
      expXrad = 0.35*cm;
      break;

    case 200:
     
      theMaterial = man->FindOrBuildMaterial("G4_WATER");
      g4Xrad = theMaterial->GetRadlen();
      break;

    case 201:

      theMaterial = man->FindOrBuildMaterial("G4_CARBON_DIOXIDE");
      // g4Xrad = theMaterial->GetRadlen();
      break;



  }

// Particle definition


  G4cout << " 0 gamma" << G4endl;
  G4cout << " 1 electron" << G4endl;
  G4cout << " 2 proton" << G4endl;
  G4cout << " 3 pion+" << G4endl;
  G4cout << " 4 pion-" << G4endl;
  G4cout << " 4 muon+" << G4endl;
  G4cout << " 5 muon-" << G4endl;

  //  G4cin >> choice;
  choice = 1;

  G4ParticleDefinition* theParticleDefinition;


  switch (choice)
  {
    case 0:

      theParticleDefinition = G4Gamma::GammaDefinition();  
      break;

    case 1:

      theParticleDefinition = G4Electron::ElectronDefinition();  
      break;

    case 2:

      theParticleDefinition = G4Proton::ProtonDefinition();
      break;

    case 3:

      theParticleDefinition = G4PionPlus::PionPlusDefinition(); 
      break;

    case 4:

      theParticleDefinition = G4PionMinus::PionMinusDefinition();
      break;
 
    case 5:
     
      theParticleDefinition = G4MuonPlus::MuonPlusDefinition(); 
      break;

    case 6:
     
      theParticleDefinition = G4MuonMinus::MuonMinusDefinition();
      break;

  }

  G4double energyMscXR, xsc=0., kinEnergy, Tmax, dEdx, Efermi;

  // EM ionisation models

  const G4DataVector cuts(1,1.);

  G4VEmModel* paiph   = new G4PAIPhotonModel(theParticleDefinition,"paiph");
              paiph->Initialise(theParticleDefinition,cuts);

	      // G4VEmModel* bragg   = new G4BraggModel(theParticleDefinition,"bragg");

  G4VEmModel* bhabha  = new G4MollerBhabhaModel(theParticleDefinition,"bhabha");
  // G4VEmModel* liver   = new G4LivermoreIonisationModel(theParticleDefinition,"liver");
  // liver->Initialise(theParticleDefinition,cuts);
  // G4VEmModel* penelop = new G4PenelopeIonisationModel(theParticleDefinition,"penelop");
  // G4VEmModel* pen08 = new G4PenelopeIonisationModel(theParticleDefinition,"penelop");






  G4DynamicParticle*  theDynamicParticle = new G4DynamicParticle(theParticleDefinition,
                                              G4ParticleMomentum(0.,0.,1.),
                                              kinEnergy);

  G4double m1 = theParticleDefinition->GetPDGMass();
  G4double plab = theDynamicParticle->GetTotalMomentum();
  G4cout <<"lab momentum, plab = "<<plab/GeV<<" GeV"<<G4endl;
  G4double plabLowLimit = 20.0*MeV;

  G4int Z   = G4int(theElement->GetZ());
  G4int A    = G4int(theElement->GetN()+0.5);

  G4double step = 4.10*mm;

  step = expXrad;

  G4double m2 = man->GetAtomicMassAmu(Z)*GeV;
  // G4double m2 = man->GetAtomicMass( Z, A);
  G4cout <<" target mass, m2 = "<<m2/GeV<<" GeV"<<G4endl<<G4endl;
  G4cout <<"step = "<<step<<" mm; g4Xrad = "<<g4Xrad<<" mm; expXrad = "
         <<expXrad<<"  mm"<<G4endl<<G4endl;



  // Check dE/dx for different models and wide energies

  G4double Nlon = (2.68e19)/cm3;
  G4double rateMass = electron_mass_c2/theParticleDefinition->GetPDGMass();
  Efermi = 7.0*eV;  
  iMax = 200;
  writef<<iMax<<G4endl;
  for( i = 0; i < iMax; i++ )
  {
    // kinEnergy = std::exp(i*0.032)*17.*keV; // protons
    kinEnergy = std::exp(i*0.065)*20.*eV; // electrons

    Tmax = 0.5*kinEnergy; // electrons
    /*
    G4double   tau      = kinEnergy/theParticleDefinition->GetPDGMass(); // other particles
    // tau      = kineticEnergy/electron_mass_c2;
    // G4double    gamma    = tau +1.0;
    G4double    bg2      = tau*(tau + 2.0);
    //   bg = std::sqrt(bg2);       
    Tmax = 2.0*electron_mass_c2*bg2/(1.0+2.0*gamma*rateMass+rateMass*rateMass);
    */
    // dEdx = bragg->ComputeDEDXPerVolume(theMaterial, theParticleDefinition, kinEnergy, Tmax);
    dEdx = bhabha->ComputeDEDXPerVolume(theMaterial, theParticleDefinition, kinEnergy, Tmax);
    // dEdx = penelop->ComputeDEDXPerVolume(theMaterial, theParticleDefinition, kinEnergy, Tmax);
    // dEdx = pen08->ComputeDEDXPerVolume(theMaterial, theParticleDefinition, kinEnergy, Tmax);
    // dEdx = liver->ComputeDEDXPerVolume(theMaterial, theParticleDefinition, kinEnergy, Tmax);
    // dEdx = paiph->ComputeDEDXPerVolume(theMaterial, theParticleDefinition, kinEnergy, Tmax);

    // dEdx *= angstrom/eV;
    dEdx *= g/theMaterial->GetDensity()/MeV/cm2;
    // dEdx *= (1.e15)/Nlon/eV/cm2;

    // kinEnergy -= Efermi;
     // kinEnergy *= amu_c2/proton_mass_c2;

    G4cout<<kinEnergy/eV<<"\t\t"<<dEdx<<G4endl;
    writef<<kinEnergy/eV<<"\t\t"<<dEdx<<G4endl;

  }

  return 1;
} // end of main