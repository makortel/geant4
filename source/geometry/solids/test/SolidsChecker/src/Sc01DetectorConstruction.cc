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
// $Id: Sc01DetectorConstruction.cc 66356 2012-12-18 09:02:32Z gcosmo $
// 
//  GEANT 4 class header file 
//
//      This is a version for maximum particle set
//  History: 
//       27.01.04  V. Grichine based on test10 detector construction
//        
// 

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "Sc01DetectorConstruction.hh"
#include "Sc01DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"

#include "G4Box.hh"
#include "G4Orb.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Cons.hh"
#include "G4Hype.hh"
#include "G4Para.hh"
#include "G4Torus.hh"
#include "G4Trd.hh"

#include "G4Polycone.hh"

#include "G4TwistedBox.hh"
#include "G4TwistedTrd.hh"
#include "G4TwistedTrap.hh"

#include "G4BooleanSolid.hh"
#include "G4DisplacedSolid.hh"
#include "G4UnionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4ReflectedSolid.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4TransportationManager.hh"
#include "G4GeometryManager.hh"
#include "G4StateManager.hh"
#include "G4UImanager.hh"
#include "G4RunManager.hh"
#include "G4ios.hh"

///////////////////////////////////////////////////////////////

Sc01DetectorConstruction::Sc01DetectorConstruction()
  : aVolume(0), PhysicalVolume(0), Water(0), Water1(0), aSurface(0)
{
  detectorMessenger = new Sc01DetectorMessenger (this);
}

////////////////////////////////////////////////////////////////

Sc01DetectorConstruction::~Sc01DetectorConstruction()
{
}

////////////////////////////////////////////////////////////////

void Sc01DetectorConstruction::SwitchDetector()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(PhysicalVolume);
}

///////////////////////////////////////////////////////////////

G4VPhysicalVolume*
Sc01DetectorConstruction::SelectDetector( const G4String& val )
{

  // fHallSize = 2.8*m;
  fHallSize = 4.8*m;

  G4Box * Hall
          = new G4Box("Hall", fHallSize,fHallSize,fHallSize );

  G4LogicalVolume * Hall_log
          = new G4LogicalVolume (Hall, Water, "Hall_L", 0,0,0);

  PhysicalVolume
          = new G4PVPlacement(0,G4ThreeVector(),"Hall_P",Hall_log,0,false,0);

  

  G4Box* b1 = new G4Box ( "b1", 100*cm, 50*cm, 50*cm );
  G4Box* b2 = new G4Box ( "b2", 50*cm, 100*cm, 50*cm );

  if (val == "Sphere")
  {
      aVolume = new G4Sphere ( "aSphere", 80.0*cm, 100.0*cm, 
                          0.0*deg, 300.0*deg,10.0*deg, 130.0*deg);

    //    aVolume = new G4Sphere("aSphere", 2400*mm, 2404*mm,
    //                          -7.349220986995449*deg,
    //                          14.6984419739909*deg,
    //                          80.16372565937344*deg,
    //                          19.67254868125311*deg      );
  }
  else if (val == "Orb")
  {
   aVolume = new G4Orb ( "aOrb", 10.0*cm );
  }
  else if (val == "Box") 
  {         
    aVolume = new G4Box ( "aBox", 10*cm, 10*cm, 10*cm );
  }
  else if (val == "Cone")
  {        
    aVolume = new G4Cons ( "aCone", 2*cm, 6*cm, 8*cm, 14*cm,
                           10*cm, 10*deg, 300*deg ); 
  //  10*cm, 10*deg, 300*deg ); 
                           //  0., pi); 
  }
  else if (val == "manyCons")
  {        
    aVolume = new G4Cons ( "aCone", 2*cm, 6*cm, 8*cm, 14*cm,
                           10*cm, 10*deg, 300*deg ); 
  //  10*cm, 10*deg, 300*deg ); 
                           //  0., pi);

 
  }
  else if (val == "Tube")
  {
    //  aVolume = new G4Tubs ( "aTube", 5*cm, 10*cm, 7*cm, 70*deg, 100*deg);
    //    aVolume = new G4Tubs ( "aTube", 5*cm, 10*cm, 7*cm, 10*deg, 300*deg);
    aVolume = new G4Tubs ( "aTube", 0.*mm, 1300*mm, 2700*mm,
                                    0.*deg, 360*deg );
  }
  else if (val == "Hype")
  {
    aVolume = new G4Hype ("aHype", 10*cm, 20*cm, 0*deg, 360*deg, 10*cm );
  }
  else if (val == "Torus")
  {
    // aVolume = new G4Torus ("aTorus", 10*cm, 15*cm, 20*cm, 0*deg, 60*deg);
    aVolume = new G4Torus ("aTorus", 25*cm, 26*cm, 290*cm, 0*deg, 90*deg);
    G4LogicalVolume* aVolume_log  = new G4LogicalVolume(aVolume, Water1, "aVolume_L", 0,0,0);
    G4RotationMatrix* rot = new G4RotationMatrix();
    rot->rotateX(-90*degree);
    // G4VPhysicalVolume * aVolume_phys1 =
      new G4PVPlacement(rot,G4ThreeVector(0*cm, 0*cm, 0*cm),val, 
                        aVolume_log, PhysicalVolume, false, 0);
  }
  else if (val == "Para")
  {
    aVolume = new G4Para ("aPara", 8*cm, 10*cm, 12*cm, 30*deg, 45*deg, 60*deg);
  }
  else if (val == "Trd")
  {
    aVolume = new G4Trd ("aTrd", 80*cm, 100*cm, 70*cm, 90*cm, 100*cm);
  }
  else if (val == "b1Ub2") 
  {         
    aVolume = new G4UnionSolid("b1Ub2",b1,b2);
    /*
    G4Box * box1 = new G4Box("Box1",1092.500000,240.103374,92.000000);
    G4Box * box2 = new G4Box("Box2",540.103374,792.500000,92.000000);

    G4double L1 = 1104;

    aVolume =
    new G4UnionSolid("ECShapeBoxes",
                     box1,
                     box2,
                     0,
                     G4ThreeVector(-L1/2.,
                                   L1/2.,
                                   0.));
    */
  }
  else if (val == "b1Ib2") 
  {         
    aVolume = new G4IntersectionSolid("b1Ib2",b1,b2);
  }
  else if (val == "b1Sb2") 
  {         
    aVolume = new G4SubtractionSolid("b1Sb2",b1,b2);
  }
  else if (val == "b1Ib1") 
  {         
    aVolume = new G4IntersectionSolid("b1Ib1",b1,b1);
  }
  else if (val == "b1Ub1") 
  {         
    aVolume = new G4UnionSolid("b1Ub1",b1,b1);
  }
  else if (val == "b1Sb1") 
  {         
    aVolume = new G4SubtractionSolid("b1Sb1",b1,b1);
  }
  else if (val == "TwistedBox")
  {
    aVolume = new G4TwistedBox("aTwistedBox",40*deg,5*cm,8*cm,15*cm) ;
  }
  else if (val == "TwistedTrd")
  {
    aVolume = new G4TwistedTrd("aTwistedTrd",5*cm,10*cm,8*cm,15*cm,18*cm,20*deg);
  }
  else if (val == "TwistedTrap")
  {
    aVolume = new G4TwistedTrap("aTwistedTrap",40*deg,5*cm,7*cm,8*cm,15*cm);
  }
  else if ( val == "TwistedTrap2") 
  {
    aVolume = new G4TwistedTrap("aTwistedTrap2",
                                20*deg,    // twist angle
                                80*cm,         // half z length
                                10*deg,      // direction between end planes
                                40*deg,        // defined by polar and azimutal angles.
                                8*cm,        // half y length at -pDz
                                11*cm,        // half x length at -pDz,-pDy
                                16*cm,        // half x length at -pDz,+pDy
                                8*cm,        // half y length at +pDz
                                11*cm,         // half x length at +pDz,-pDy
                                16*cm,        // half x length at +pDz,+pDy
                                -50*deg        // tilt angle at +pDz
                                ) ;
  }

  else
  {
    //   G4Exception("Sc01DetectorConstruction::SelectDetector()",
    //               "Sc01DC001", FatalException, "Invalid shape!");
  }
  if(val == "manyCons")
  {

    G4Cons* cons1 = new G4Cons ( "cons1", 2*cm, 3*cm, 4*cm, 6*cm,
                           10*cm, 0*deg, 360*deg );
    G4Cons* cons2 = new G4Cons ( "cons2", 3*cm, 4*cm, 6*cm, 8*cm,
                           10*cm, 0*deg, 360*deg );
    G4Cons* cons3 = new G4Cons ( "cons3", 4*cm, 5*cm, 8*cm, 10*cm,
                           10*cm, 0*deg, 360*deg );

    G4LogicalVolume* lcons1 = new G4LogicalVolume(cons1, Water1,"lcons1",0,0,0);
    G4LogicalVolume* lcons2 = new G4LogicalVolume(cons2, Water1,"lcons2",0,0,0);
    G4LogicalVolume* lcons3 = new G4LogicalVolume(cons3, Water1,"lcons3",0,0,0);

    // G4VPhysicalVolume * pcons1 =
    new G4PVPlacement(0,G4ThreeVector(0*cm, 0*cm, 0*cm),
                      "pcons1",lcons1,PhysicalVolume, false, 0); 
    // G4VPhysicalVolume * pcons2 =
    new G4PVPlacement(0,G4ThreeVector(0*cm, 0*cm, 0*cm),
                      "pcons2",lcons2,PhysicalVolume, false, 0); 
    // G4VPhysicalVolume * pcons3 =
    new G4PVPlacement(0,G4ThreeVector(0*cm, 0*cm, 0*cm),
                      "pcons3",lcons3,PhysicalVolume, false, 0); 
  }  
  else
  {
    G4Torus* solidFiber =
      new G4Torus("clad",0.,1.*cm,10.*cm,0.*deg,180.*deg);    // external

    G4Torus* solidCore =
      new G4Torus("core",0.,0.5*cm,10.*cm,0.*deg,180.*deg); // internal

    G4LogicalVolume* logicFiber =
    new G4LogicalVolume(solidFiber,Water1,"FiberClad",0,0,0);

    G4LogicalVolume* logicCore =
    new G4LogicalVolume(solidCore,Water1,"FiberCore",0,0,0);

  G4VPhysicalVolume * aVolume_phys1 =
  new G4PVPlacement(0,G4ThreeVector(0*cm, 0*cm, 0*cm),
		    "FiberClad",logicFiber,PhysicalVolume,false,0);

  // G4VPhysicalVolume * aVolume_phys2 =
  new G4PVPlacement(0,G4ThreeVector(0*cm, 0*cm, 0*cm),
		    "FiberCore",logicCore,aVolume_phys1,false,0);

  /*
  G4LogicalVolume* aVolume_log 

    //  = new G4LogicalVolume(aVolume, Water1, "aVolume_L", 0,0,0);

    //  =  GetConePolycone();
   
  G4VPhysicalVolume * aVolume_phys1
    = new G4PVPlacement(0,G4ThreeVector(0*cm, 0*cm, 0*cm),val, 
                        aVolume_log, PhysicalVolume, false, 0);


  */
//--------- Visualization attributes -------------------------------

  //  G4VisAttributes* BoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  //  Hall_log  ->SetVisAttributes(BoxVisAtt);
  Hall_log  ->SetVisAttributes (G4VisAttributes::Invisible);

  // aVolume_log ->SetVisAttributes(BoxVisAtt);
  // logicTracker->SetVisAttributes(BoxVisAtt);

  // G4VisAttributes* ChamberVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
  //  aVolume_log->SetVisAttributes(ChamberVisAtt);

//--------- example of User Limits -------------------------------



  /*
  G4VPhysicalVolume * aVolume_phys1
    = new G4PVPlacement(0,G4ThreeVector(50*cm, 0*cm, 0*cm),val, 
                        aVolume_log, PhysicalVolume, false, 0);
  G4VPhysicalVolume * aVolume_phys2
    = new G4PVPlacement(0,G4ThreeVector(-50*cm, 0*cm, 0*cm),val, 
                        aVolume_log, PhysicalVolume, false, 0);
  G4VPhysicalVolume * aVolume_phys3
    = new G4PVPlacement(0,G4ThreeVector(0*cm, 50*cm, 0*cm),val, 
                        aVolume_log, PhysicalVolume, false, 0);
  G4VPhysicalVolume * aVolume_phys4
    = new G4PVPlacement(0,G4ThreeVector(0*cm, -50*cm, 0*cm),val, 
                        aVolume_log, PhysicalVolume, false, 0);
  G4VPhysicalVolume * aVolume_phys5
    = new G4PVPlacement(0,G4ThreeVector(0*cm, 0*cm, 50*cm),val, 
                        aVolume_log, PhysicalVolume, false, 0);
  G4VPhysicalVolume * aVolume_phys6
    = new G4PVPlacement(0,G4ThreeVector(0*cm, 0*cm, -50*cm),val, 
                    aVolume_log, PhysicalVolume, false, 0);
  */

  // ------------ Surfaces definition ------------------

//  G4LogicalBorderSurface* BorderSurfaces[12];   
//  G4LogicalBorderSurface* BorderSurfaces[2];   
  
// BorderSurfaces[0] =
   new G4LogicalBorderSurface("VolumeSurface",
                               PhysicalVolume,
                               aVolume_phys1,
                               aSurface);

// BorderSurfaces[6] =
   new G4LogicalBorderSurface("VolumeSurface",
                               aVolume_phys1,
                               PhysicalVolume,
                               aSurface);
/*
  BorderSurfaces[1] = new G4LogicalBorderSurface("VolumeSurface",
                               PhysicalVolume,
                               aVolume_phys2,
                               aSurface);
  BorderSurfaces[2] = new G4LogicalBorderSurface("VolumeSurface",
                               PhysicalVolume,
                               aVolume_phys3,
                               aSurface);
  BorderSurfaces[3] = new G4LogicalBorderSurface("VolumeSurface",
                               PhysicalVolume,
                               aVolume_phys4,
                               aSurface);
  BorderSurfaces[4] = new G4LogicalBorderSurface("VolumeSurface",
                               PhysicalVolume,
                               aVolume_phys5,
                               aSurface);
  BorderSurfaces[5] = new G4LogicalBorderSurface("VolumeSurface",
                               PhysicalVolume,
                               aVolume_phys6,
                               aSurface);
  BorderSurfaces[6] = new G4LogicalBorderSurface("VolumeSurface",
                               aVolume_phys1,
                               PhysicalVolume,
                               aSurface);
  BorderSurfaces[7] = new G4LogicalBorderSurface("VolumeSurface",
                               aVolume_phys2,
                               PhysicalVolume,
                               aSurface);
  BorderSurfaces[8] = new G4LogicalBorderSurface("VolumeSurface",
                               aVolume_phys3,
                               PhysicalVolume,
                               aSurface);
  BorderSurfaces[9] = new G4LogicalBorderSurface("VolumeSurface",
                               aVolume_phys4,
                               PhysicalVolume,
                               aSurface);
  BorderSurfaces[10] = new G4LogicalBorderSurface("VolumeSurface",
                               aVolume_phys5,
                               PhysicalVolume,
                               aSurface);
  BorderSurfaces[11] = new G4LogicalBorderSurface("VolumeSurface",
                               aVolume_phys6,
                               PhysicalVolume,
                               aSurface);
*/
  }
  G4cout << "You select " << val << " detector" << G4endl;

  return PhysicalVolume;
}



G4VPhysicalVolume* Sc01DetectorConstruction::SelectTubeSector()
{
  G4double a,z,density;

  G4Material* Al =
    new G4Material("Aluminum", z=13., a=26.98*g/mole, density=1.782*mg/cm3);

  G4Material* Vacuum =
    new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                   kStateGas, 3.e-18*pascal, 2.73*kelvin);




  G4int i, sections_ = 12; // 12;

  G4double cof, dPhi, sPhi;

  sPhi = twopi/sections_;
  cof = 1.;
  dPhi = cof*sPhi;

  G4cout<<"sections_ = "<<sections_<<G4endl;

  G4double lab_x = 20.0*m;
  G4double lab_y = 20.0*m;
  G4double lab_z = 20.0*m;

  fHallSize = lab_x;

  G4cout<<"m = "<<m<<"; degree = "<<degree<<G4endl;

  // world volume

  G4Box* lab_S = new G4Box("lab",lab_x,lab_y,lab_z);

  G4LogicalVolume*  lab_L = new G4LogicalVolume(lab_S,
                              Vacuum, "lab", 0, 0, 0);

  // tube section

  G4Tubs* arcSection_S = new G4Tubs("arcSection",
                              6.*m, 10.*m, 2.*m,
                              0., dPhi            );

  G4cout<<"dPhi = "<<dPhi<<G4endl;

  G4LogicalVolume* arcSection_L = new G4LogicalVolume(arcSection_S,
                                     Al, "arcSection", 0, 0, 0);

  for( i = 0; i < sections_; i++)
  {
    G4RotationMatrix rot;
    rot.rotateZ( sPhi*i );

    G4cout<<"i = "<<i<<";  i*sPhi = "<<i*sPhi<<G4endl;

    G4cout<<"cos(i*sPhi)= "<<std::cos(i*sPhi)
          <<";  std::sin(i*sPhi) = "<<std::sin(i*sPhi)<<G4endl<<G4endl;

    G4cout<<rot.xx()<<"\t\t"<<rot.xy()<<"\t\t"<<rot.xz()<<G4endl;
    G4cout<<rot.yx()<<"\t\t"<<rot.yy()<<"\t\t"<<rot.yz()<<G4endl;
    G4cout<<rot.zx()<<"\t\t"<<rot.zy()<<"\t\t"<<rot.zz()<<G4endl<<G4endl;

    G4Transform3D transf(rot, G4ThreeVector() );

    // G4VPhysicalVolume* arcSection_P = 
    new G4PVPlacement(transf, arcSection_L,
                      "section", lab_L, false, i, true);
  } 
  G4VPhysicalVolume* lab_P= new G4PVPlacement(0, G4ThreeVector(),
                           lab_L, "lab", 0, false, 0);

  G4VisAttributes* labVisAtt = new G4VisAttributes(G4Colour());
  labVisAtt -> SetVisibility(1);
  lab_L -> SetVisAttributes(labVisAtt);

  G4VisAttributes* aSVisAtt =
  new G4VisAttributes(G4Colour(1.0,0.0,0.0,1.0));
  aSVisAtt -> SetForceWireframe(1);
  arcSection_L -> SetVisAttributes(aSVisAtt);

  return lab_P;

}

///////////////////////////////////////////////////////////////////////
//
//

G4LogicalVolume* Sc01DetectorConstruction::GetConePolycone()
{

  G4double cryoMotherZplan[] = {-3490.*mm,
                              -2850.*mm,
                              -2849.*mm, 
                               2849.*mm, 
                               2850.*mm, 
                               3490.*mm};

  G4double cryoMotherRin[]   = { 1149.8*mm, 
                               1149.8*mm,
                               1149.8*mm,
                               1149.8*mm,
                               1149.8*mm,
                               1149.8*mm};

  G4double cryoMotherRout[]  = { 2890.*mm, 
                               2890.*mm,
                               2250.*mm,
                               2250.*mm,
                               2890.*mm,
                               2890.*mm};  

  // LArVG4DetectorParameters* parameters = LArVG4DetectorParameters::GetInstance();

  // Get the z-Shift from the detector parameters routine.

  G4double zShift = 4.*cm;               // parameters->GetValue("LArEMECZshift");

   // Adjust mother volume size.

G4int lastPlaneCryo = ( sizeof(cryoMotherZplan) / sizeof(G4double) );

cryoMotherZplan[lastPlaneCryo-1] += zShift;

cryoMotherZplan[0] -= zShift;

G4String cryoMotherName = "LAr::Barrel::Cryostat::MotherVolume";

G4Polycone* cryoMotherShape =
   new G4Polycone(cryoMotherName,          // name
                   0.,                     // starting phi
                   twopi,                  // total phi
                   lastPlaneCryo,          // number of z planes
                   cryoMotherZplan,        // position of z planes    
                   cryoMotherRin,          // tangent distance to inner surface
                   cryoMotherRout );       // tangent distance to outer surface



 G4LogicalVolume* lvPolycone = new G4LogicalVolume(cryoMotherShape,
                                                   Water1,
                                                  "lvPolycone");


// #  Material  Zmin     Dz     Rmin1     Rmax1     Rmin2    Rmax2
// { 11,  Al, 316.000,  22.500, 120.800, 122.021, 140.779, 142.000},

// the cone is positioned at Zmin+Dz, hence 316cm+22.5cm...

  G4Cons* coneATLAS = new G4Cons("coneATLAS",120.8*cm, 122.021*cm, 
                                            140.779*cm, 142.0*cm,
                                            22.5*cm/2., 0., 2*pi              );

  G4LogicalVolume* lvCone = new G4LogicalVolume(coneATLAS,
                                       Water1,
                                      "lvCone");

// G4VPhysicalVolume*  pvCone =
    new G4PVPlacement(0, G4ThreeVector( 0, 0, 316.0*cm + 22.5*cm ),
                                       lvCone ,
                                      "pvCone",
                                       lvPolycone,
                                       false,
                                       0                );


  G4VisAttributes* ChamberVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
  lvCone->SetVisAttributes(ChamberVisAtt);

  return lvPolycone;
}



void Sc01DetectorConstruction::SetMaterial()
{
  G4String name, symbol;
  G4double density = 1.00*g/cm3;
  G4double a, iz;
  Water = new G4Material(name="Water", density, 2);
  Water1 = new G4Material(name="Water1", density, 2);
  a = 1*g/mole;
  G4Element* elH = new G4Element(name="Hydrogen", symbol="H", iz=1., a);
  a = 16.00*g/mole;
  G4Element* elO = new G4Element(name="Oxygen", symbol="O", iz=8., a);

  Water->AddElement(elH, .66);
  Water->AddElement(elO, .34);
  Water1->AddElement(elH, .66);
  Water1->AddElement(elO, .34);

  const G4int NUMENTRIES = 5;
  G4double RINDEX_WATER [NUMENTRIES];
  G4double RINDEX_WATER1 [NUMENTRIES];
  G4double REFLECTIVITY [NUMENTRIES];
  G4double EFFICIENCY [NUMENTRIES];
  
  for (int i=0; i<NUMENTRIES; i++) {
    RINDEX_WATER1[i]=5.0;
    RINDEX_WATER[i]=1.33;
    REFLECTIVITY[i]=0.9;
    EFFICIENCY[i]=1.0;
  }  
  G4double PHENERGY[NUMENTRIES] =
            { 0.0, 1.0, 2.0, 3.0, 4.0};
  G4MaterialPropertiesTable *WaterMPT = new G4MaterialPropertiesTable();
  G4MaterialPropertiesTable *WaterMPT1 = new G4MaterialPropertiesTable();
  WaterMPT->AddProperty("RINDEX", PHENERGY, RINDEX_WATER, NUMENTRIES);
  WaterMPT1->AddProperty("RINDEX", PHENERGY, RINDEX_WATER1, NUMENTRIES);
  Water->SetMaterialPropertiesTable(WaterMPT);
  Water1->SetMaterialPropertiesTable(WaterMPT1);

  aSurface = new G4OpticalSurface ( "aSurface" );
  aSurface->SetType(dielectric_metal);
  aSurface->SetFinish(polishedfrontpainted);
  aSurface->SetModel(glisur);  
  G4MaterialPropertiesTable* SurfaceMPT = new G4MaterialPropertiesTable();
  SurfaceMPT->AddProperty("REFLECTIVITY", PHENERGY, REFLECTIVITY, NUMENTRIES);
  SurfaceMPT->AddProperty("EFFICIENCY", PHENERGY, EFFICIENCY, NUMENTRIES);
  aSurface->SetMaterialPropertiesTable ( SurfaceMPT );
}

G4VPhysicalVolume* Sc01DetectorConstruction::Construct()
{
  SetMaterial();

  //-------------------Hall ----------------------------------
  // return SelectTubeSector();  
  return SelectDetector ("Torus");
}


