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
// $Id: G4FTFBinaryNeutronBuilder.cc 68750 2013-04-05 10:19:04Z gcosmo $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4FTFBinaryNeutronBuilder
//
// Author: 2008 G.Folger
//
// Modified:
// 18.11.2010 G.Folger, use G4CrossSectionPairGG for relativistic rise of cross
//             section at high energies.
// 30.03.2009 V.Ivanchenko create cross section by new
//
//----------------------------------------------------------------------------
//
#include "G4FTFBinaryNeutronBuilder.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4NeutronInelasticCrossSection.hh"
#include "G4BGGNucleonInelasticXS.hh"

G4FTFBinaryNeutronBuilder::
G4FTFBinaryNeutronBuilder(G4bool quasiElastic) 
{
  theMin = 4*GeV;
  theMax = 100*TeV;
  theModel = new G4TheoFSGenerator("FTFB");

  theStringModel = new G4FTFModel;
  theStringDecay = new G4ExcitedStringDecay(new G4LundStringFragmentation);
  theStringModel->SetFragmentationModel(theStringDecay);

  theCascade = new G4BinaryCascade;
  theModel->SetTransport(theCascade);
  theModel->SetMinEnergy(theMin);
  theModel->SetMaxEnergy(theMax);

  theModel->SetHighEnergyGenerator(theStringModel);
  if (quasiElastic)
  {
     theQuasiElastic=new G4QuasiElasticChannel;
     theModel->SetQuasiElasticChannel(theQuasiElastic);
  } else 
  {  theQuasiElastic=0;}  
}

G4FTFBinaryNeutronBuilder::
~G4FTFBinaryNeutronBuilder() 
{
  delete theStringDecay;
  delete theStringModel;
  delete theCascade;
  if ( theQuasiElastic ) delete theQuasiElastic;
}

void G4FTFBinaryNeutronBuilder::
Build(G4HadronElasticProcess * )
{
}

void G4FTFBinaryNeutronBuilder::
Build(G4HadronFissionProcess * )
{
}

void G4FTFBinaryNeutronBuilder::
Build(G4HadronCaptureProcess * )
{
}

void G4FTFBinaryNeutronBuilder::
Build(G4NeutronInelasticProcess * aP)
{
  theModel->SetMinEnergy(theMin);
  theModel->SetMaxEnergy(theMax);
  aP->RegisterMe(theModel);
  aP->AddDataSet(new G4BGGNucleonInelasticXS(G4Neutron::Neutron()));
}
