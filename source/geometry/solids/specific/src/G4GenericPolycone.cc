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
// $Id: G4GenericPolycone.cc 72131 2013-07-11 07:12:24Z gcosmo $
//
// 
// --------------------------------------------------------------------
// GEANT 4 class source file
//
//
// G4GenericPolycone.cc
//
// Implementation of a CSG polycone
//
// --------------------------------------------------------------------

#include "G4GenericPolycone.hh"

#if !defined(G4GEOM_USE_UGENERICPOLYCONE)

#include "G4PolyconeSide.hh"
#include "G4PolyPhiFace.hh"

#include "Randomize.hh"

#include "G4Polyhedron.hh"
#include "G4EnclosingCylinder.hh"
#include "G4ReduciblePolygon.hh"
#include "G4VPVParameterisation.hh"

using namespace CLHEP;

//
// Constructor (generic parameters)
//
G4GenericPolycone::G4GenericPolycone( const G4String& name, 
                              G4double phiStart,
                              G4double phiTotal,
                              G4int    numRZ,
                        const G4double r[],
                        const G4double z[]   )
  : G4VCSGfaceted( name )
{ 
  
  G4ReduciblePolygon *rz = new G4ReduciblePolygon( r, z, numRZ );
  
  Create( phiStart, phiTotal, rz );
  
  // Set original_parameters struct for consistency
  //
  //SetOriginalParameters(rz);

  delete rz;
}

//
// Create
//
// Generic create routine, called by each constructor after
// conversion of arguments
//
void G4GenericPolycone::Create( G4double phiStart,
                         G4double phiTotal,
                         G4ReduciblePolygon *rz    )
{
  //
  // Perform checks of rz values
  //
  if (rz->Amin() < 0.0)
  {
    std::ostringstream message;
    message << "Illegal input parameters - " << GetName() << G4endl
            << "        All R values must be >= 0 !";
    G4Exception("G4GenericPolycone::Create()", "GeomSolids0002",
                FatalErrorInArgument, message);
  }
    
  G4double rzArea = rz->Area();
  if (rzArea < -kCarTolerance)
    rz->ReverseOrder();

  else if (rzArea < -kCarTolerance)
  {
    std::ostringstream message;
    message << "Illegal input parameters - " << GetName() << G4endl
            << "        R/Z cross section is zero or near zero: " << rzArea;
    G4Exception("G4GenericPolycone::Create()", "GeomSolids0002",
                FatalErrorInArgument, message);
  }
    
  if ( (!rz->RemoveDuplicateVertices( kCarTolerance ))
    || (!rz->RemoveRedundantVertices( kCarTolerance ))     ) 
  {
    std::ostringstream message;
    message << "Illegal input parameters - " << GetName() << G4endl
            << "        Too few unique R/Z values !";
    G4Exception("G4GenericPolycone::Create()", "GeomSolids0002",
                FatalErrorInArgument, message);
  }

  if (rz->CrossesItself(1/kInfinity)) 
  {
    std::ostringstream message;
    message << "Illegal input parameters - " << GetName() << G4endl
            << "        R/Z segments cross !";
    G4Exception("G4GenericPolycone::Create()", "GeomSolids0002",
                FatalErrorInArgument, message);
  }

  numCorner = rz->NumVertices();

  //
  // Phi opening? Account for some possible roundoff, and interpret
  // nonsense value as representing no phi opening
  //
  if (phiTotal <= 0 || phiTotal > twopi-1E-10)
  {
    phiIsOpen = false;
    startPhi = 0;
    endPhi = twopi;
  }
  else
  {
    phiIsOpen = true;
    
    //
    // Convert phi into our convention
    //
    startPhi = phiStart;
    while( startPhi < 0 ) startPhi += twopi;
    
    endPhi = phiStart+phiTotal;
    while( endPhi < startPhi ) endPhi += twopi;
  }
  
  //
  // Allocate corner array. 
  //
  corners = new G4PolyconeSideRZ[numCorner];

  //
  // Copy corners
  //
  G4ReduciblePolygonIterator iterRZ(rz);
  
  G4PolyconeSideRZ *next = corners;
  iterRZ.Begin();
  do
  {
    next->r = iterRZ.GetA();
    next->z = iterRZ.GetB();
  } while( ++next, iterRZ.Next() );
  
  //
  // Allocate face pointer array
  //
  numFace = phiIsOpen ? numCorner+2 : numCorner;
  faces = new G4VCSGface*[numFace];
  
  //
  // Construct conical faces
  //
  // But! Don't construct a face if both points are at zero radius!
  //
  G4PolyconeSideRZ *corner = corners,
                   *prev = corners + numCorner-1,
                   *nextNext;
  G4VCSGface  **face = faces;
  do
  {
    next = corner+1;
    if (next >= corners+numCorner) next = corners;
    nextNext = next+1;
    if (nextNext >= corners+numCorner) nextNext = corners;
    
    if (corner->r < 1/kInfinity && next->r < 1/kInfinity) continue;
    
    //
    // We must decide here if we can dare declare one of our faces
    // as having a "valid" normal (i.e. allBehind = true). This
    // is never possible if the face faces "inward" in r.
    //
    G4bool allBehind;
    if (corner->z > next->z)
    {
      allBehind = false;
    }
    else
    {
      //
      // Otherwise, it is only true if the line passing
      // through the two points of the segment do not
      // split the r/z cross section
      //
      allBehind = !rz->BisectedBy( corner->r, corner->z,
                 next->r, next->z, kCarTolerance );
    }
    
    *face++ = new G4PolyconeSide( prev, corner, next, nextNext,
                startPhi, endPhi-startPhi, phiIsOpen, allBehind );
  } while( prev=corner, corner=next, corner > corners );
  
  if (phiIsOpen)
  {
    //
    // Construct phi open edges
    //
    *face++ = new G4PolyPhiFace( rz, startPhi, 0, endPhi  );
    *face++ = new G4PolyPhiFace( rz, endPhi,   0, startPhi );
  }
  
  //
  // We might have dropped a face or two: recalculate numFace
  //
  numFace = face-faces;
  
  //
  // Make enclosingCylinder
  //
  enclosingCylinder =
    new G4EnclosingCylinder( rz, phiIsOpen, phiStart, phiTotal );
}


//
// Fake default constructor - sets only member data and allocates memory
//                            for usage restricted to object persistency.
//
G4GenericPolycone::G4GenericPolycone( __void__& a )
  : G4VCSGfaceted(a), startPhi(0.),  endPhi(0.), phiIsOpen(false),
     numCorner(0), corners(0), enclosingCylinder(0)
{
}


//
// Destructor
//
G4GenericPolycone::~G4GenericPolycone()
{
  delete [] corners;
  delete enclosingCylinder;
}


//
// Copy constructor
//
G4GenericPolycone::G4GenericPolycone( const G4GenericPolycone &source )
  : G4VCSGfaceted( source )
{
  CopyStuff( source );
}


//
// Assignment operator
//
G4GenericPolycone &G4GenericPolycone::operator=( const G4GenericPolycone &source )
{
  if (this == &source) return *this;
  
  G4VCSGfaceted::operator=( source );
  
  delete [] corners;
  // if (original_parameters) delete original_parameters;
  
  delete enclosingCylinder;
  
  CopyStuff( source );
  
  return *this;
}


//
// CopyStuff
//
void G4GenericPolycone::CopyStuff( const G4GenericPolycone &source )
{
  //
  // Simple stuff
  //
  startPhi  = source.startPhi;
  endPhi    = source.endPhi;
  phiIsOpen  = source.phiIsOpen;
  numCorner  = source.numCorner;

  //
  // The corner array
  //
  corners = new G4PolyconeSideRZ[numCorner];
  
  G4PolyconeSideRZ  *corn = corners,
        *sourceCorn = source.corners;
  do
  {
    *corn = *sourceCorn;
  } while( ++sourceCorn, ++corn < corners+numCorner );
  
  //
  // Enclosing cylinder
  //
  enclosingCylinder = new G4EnclosingCylinder( *source.enclosingCylinder );

  fpPolyhedron = GetPolyhedron();
}


//
// Reset
//
G4bool G4GenericPolycone::Reset()
{
 
    std::ostringstream message;
    message << "Solid " << GetName() << " built using generic construct."
            << G4endl << "Not applicable to the generic construct !";
    G4Exception("G4GenericPolycone::Reset()", "GeomSolids1001",
                JustWarning, message, "Parameters NOT resetted.");
    return 1;
 
}


//
// Inside
//
// This is an override of G4VCSGfaceted::Inside, created in order
// to speed things up by first checking with G4EnclosingCylinder.
//
EInside G4GenericPolycone::Inside( const G4ThreeVector &p ) const
{
  //
  // Quick test
  //
  if (enclosingCylinder->MustBeOutside(p)) return kOutside;

  //
  // Long answer
  //
  return G4VCSGfaceted::Inside(p);
}


//
// DistanceToIn
//
// This is an override of G4VCSGfaceted::Inside, created in order
// to speed things up by first checking with G4EnclosingCylinder.
//
G4double G4GenericPolycone::DistanceToIn( const G4ThreeVector &p,
                                   const G4ThreeVector &v ) const
{
  //
  // Quick test
  //
  if (enclosingCylinder->ShouldMiss(p,v))
    return kInfinity;
  
  //
  // Long answer
  //
  return G4VCSGfaceted::DistanceToIn( p, v );
}


//
// DistanceToIn
//
G4double G4GenericPolycone::DistanceToIn( const G4ThreeVector &p ) const
{
  return G4VCSGfaceted::DistanceToIn(p);
}


//
// ComputeDimensions
//
/*void G4GenericPolycone::ComputeDimensions(       G4VPVParameterisation* p,
                                      const G4int n,
                                    const G4VPhysicalVolume* pRep )
{
  p->ComputeDimensions(*this,n,pRep);
}
*/
//
// GetEntityType
//
G4GeometryType  G4GenericPolycone::GetEntityType() const
{
  return G4String("G4GenericPolycone");
}


//
// Make a clone of the object
//
G4VSolid* G4GenericPolycone::Clone() const
{
  return new G4GenericPolycone(*this);
}

//
// Stream object contents to an output stream
//
std::ostream& G4GenericPolycone::StreamInfo( std::ostream& os ) const
{
  G4int oldprc = os.precision(16);
  os << "-----------------------------------------------------------\n"
     << "    *** Dump for solid - " << GetName() << " ***\n"
     << "    ===================================================\n"
     << " Solid type: G4GenericPolycone\n"
     << " Parameters: \n"
     << "    starting phi angle : " << startPhi/degree << " degrees \n"
     << "    ending phi angle   : " << endPhi/degree << " degrees \n";
  G4int i=0;
 
  os << "    number of RZ points: " << numCorner << "\n"
     << "              RZ values (corners): \n";
     for (i=0; i<numCorner; i++)
     {
       os << "                         "
          << corners[i].r << ", " << corners[i].z << "\n";
     }
  os << "-----------------------------------------------------------\n";
  os.precision(oldprc);

  return os;
}



//
// GetPointOnSurface
//
G4ThreeVector G4GenericPolycone::GetPointOnSurface() const
{
  return GetPointOnSurfaceGeneric();  

}

//
// CreatePolyhedron
//
G4Polyhedron* G4GenericPolycone::CreatePolyhedron() const
{ 
    // The following code prepares for:
    // HepPolyhedron::createPolyhedron(int Nnodes, int Nfaces,
    //                                  const double xyz[][3],
    //                                  const int faces_vec[][4])
    // Here is an extract from the header file HepPolyhedron.h:
    /**
     * Creates user defined polyhedron.
     * This function allows to the user to define arbitrary polyhedron.
     * The faces of the polyhedron should be either triangles or planar
     * quadrilateral. Nodes of a face are defined by indexes pointing to
     * the elements in the xyz array. Numeration of the elements in the
     * array starts from 1 (like in fortran). The indexes can be positive
     * or negative. Negative sign means that the corresponding edge is
     * invisible. The normal of the face should be directed to exterior
     * of the polyhedron. 
     * 
     * @param  Nnodes number of nodes
     * @param  Nfaces number of faces
     * @param  xyz    nodes
     * @param  faces_vec  faces (quadrilaterals or triangles)
     * @return status of the operation - is non-zero in case of problem
     */
    const G4int numSide =
          G4int(G4Polyhedron::GetNumberOfRotationSteps()
                * (endPhi - startPhi) / twopi) + 1;
    G4int nNodes;
    G4int nFaces;
    typedef G4double double3[3];
    double3* xyz;
    typedef G4int int4[4];
    int4* faces_vec;
    if (phiIsOpen)
    {
      // Triangulate open ends. Simple ear-chopping algorithm...
      // I'm not sure how robust this algorithm is (J.Allison).
      //
      std::vector<G4bool> chopped(numCorner, false);
      std::vector<G4int*> triQuads;
      G4int remaining = numCorner;
      G4int iStarter = 0;
      while (remaining >= 3)
      {
        // Find unchopped corners...
        //
        G4int A = -1, B = -1, C = -1;
        G4int iStepper = iStarter;
        do
        {
          if (A < 0)      { A = iStepper; }
          else if (B < 0) { B = iStepper; }
          else if (C < 0) { C = iStepper; }
          do
          {
            if (++iStepper >= numCorner) { iStepper = 0; }
          }
          while (chopped[iStepper]);
        }
        while (C < 0 && iStepper != iStarter);

        // Check triangle at B is pointing outward (an "ear").
        // Sign of z cross product determines...
        //
        G4double BAr = corners[A].r - corners[B].r;
        G4double BAz = corners[A].z - corners[B].z;
        G4double BCr = corners[C].r - corners[B].r;
        G4double BCz = corners[C].z - corners[B].z;
        if (BAr * BCz - BAz * BCr < kCarTolerance)
        {
          G4int* tq = new G4int[3];
          tq[0] = A + 1;
          tq[1] = B + 1;
          tq[2] = C + 1;
          triQuads.push_back(tq);
          chopped[B] = true;
          --remaining;
        }
        else
        {
          do
          {
            if (++iStarter >= numCorner) { iStarter = 0; }
          }
          while (chopped[iStarter]);
        }
      }
      // Transfer to faces...
      //
      nNodes = (numSide + 1) * numCorner;
      nFaces = numSide * numCorner + 2 * triQuads.size();
      faces_vec = new int4[nFaces];
      G4int iface = 0;
      G4int addition = numCorner * numSide;
      G4int d = numCorner - 1;
      for (G4int iEnd = 0; iEnd < 2; ++iEnd)
      {
        for (size_t i = 0; i < triQuads.size(); ++i)
        {
          // Negative for soft/auxiliary/normally invisible edges...
          //
          G4int a, b, c;
          if (iEnd == 0)
          {
            a = triQuads[i][0];
            b = triQuads[i][1];
            c = triQuads[i][2];
          }
          else
          {
            a = triQuads[i][0] + addition;
            b = triQuads[i][2] + addition;
            c = triQuads[i][1] + addition;
          }
          G4int ab = std::abs(b - a);
          G4int bc = std::abs(c - b);
          G4int ca = std::abs(a - c);
          faces_vec[iface][0] = (ab == 1 || ab == d)? a: -a;
          faces_vec[iface][1] = (bc == 1 || bc == d)? b: -b;
          faces_vec[iface][2] = (ca == 1 || ca == d)? c: -c;
          faces_vec[iface][3] = 0;
          ++iface;
        }
      }

      // Continue with sides...

      xyz = new double3[nNodes];
      const G4double dPhi = (endPhi - startPhi) / numSide;
      G4double phi = startPhi;
      G4int ixyz = 0;
      for (G4int iSide = 0; iSide < numSide; ++iSide)
      {
        for (G4int iCorner = 0; iCorner < numCorner; ++iCorner)
        {
          xyz[ixyz][0] = corners[iCorner].r * std::cos(phi);
          xyz[ixyz][1] = corners[iCorner].r * std::sin(phi);
          xyz[ixyz][2] = corners[iCorner].z;
          if (iSide == 0)   // startPhi
          {
            if (iCorner < numCorner - 1)
            {
              faces_vec[iface][0] = ixyz + 1;
              faces_vec[iface][1] = -(ixyz + numCorner + 1);
              faces_vec[iface][2] = ixyz + numCorner + 2;
              faces_vec[iface][3] = ixyz + 2;
            }
            else
            {
              faces_vec[iface][0] = ixyz + 1;
              faces_vec[iface][1] = -(ixyz + numCorner + 1);
              faces_vec[iface][2] = ixyz + 2;
              faces_vec[iface][3] = ixyz - numCorner + 2;
            }
          }
          else if (iSide == numSide - 1)   // endPhi
          {
            if (iCorner < numCorner - 1)
              {
                faces_vec[iface][0] = ixyz + 1;
                faces_vec[iface][1] = ixyz + numCorner + 1;
                faces_vec[iface][2] = ixyz + numCorner + 2;
                faces_vec[iface][3] = -(ixyz + 2);
              }
            else
              {
                faces_vec[iface][0] = ixyz + 1;
                faces_vec[iface][1] = ixyz + numCorner + 1;
                faces_vec[iface][2] = ixyz + 2;
                faces_vec[iface][3] = -(ixyz - numCorner + 2);
              }
          }
          else
          {
            if (iCorner < numCorner - 1)
              {
                faces_vec[iface][0] = ixyz + 1;
                faces_vec[iface][1] = -(ixyz + numCorner + 1);
                faces_vec[iface][2] = ixyz + numCorner + 2;
                faces_vec[iface][3] = -(ixyz + 2);
              }
              else
              {
                faces_vec[iface][0] = ixyz + 1;
                faces_vec[iface][1] = -(ixyz + numCorner + 1);
                faces_vec[iface][2] = ixyz + 2;
                faces_vec[iface][3] = -(ixyz - numCorner + 2);
              }
            }
            ++iface;
            ++ixyz;
        }
        phi += dPhi;
      }

      // Last corners...

      for (G4int iCorner = 0; iCorner < numCorner; ++iCorner)
      {
        xyz[ixyz][0] = corners[iCorner].r * std::cos(phi);
        xyz[ixyz][1] = corners[iCorner].r * std::sin(phi);
        xyz[ixyz][2] = corners[iCorner].z;
        ++ixyz;
      }
    }
    else  // !phiIsOpen - i.e., a complete 360 degrees.
    {
      nNodes = numSide * numCorner;
      nFaces = numSide * numCorner;;
      xyz = new double3[nNodes];
      faces_vec = new int4[nFaces];
      const G4double dPhi = (endPhi - startPhi) / numSide;
      G4double phi = startPhi;
      G4int ixyz = 0, iface = 0;
      for (G4int iSide = 0; iSide < numSide; ++iSide)
      {
        for (G4int iCorner = 0; iCorner < numCorner; ++iCorner)
        {
          xyz[ixyz][0] = corners[iCorner].r * std::cos(phi);
          xyz[ixyz][1] = corners[iCorner].r * std::sin(phi);
          xyz[ixyz][2] = corners[iCorner].z;

          if (iSide < numSide - 1)
          {
            if (iCorner < numCorner - 1)
            {
              faces_vec[iface][0] = ixyz + 1;
              faces_vec[iface][1] = -(ixyz + numCorner + 1);
              faces_vec[iface][2] = ixyz + numCorner + 2;
              faces_vec[iface][3] = -(ixyz + 2);
            }
            else
            {
              faces_vec[iface][0] = ixyz + 1;
              faces_vec[iface][1] = -(ixyz + numCorner + 1);
              faces_vec[iface][2] = ixyz + 2;
              faces_vec[iface][3] = -(ixyz - numCorner + 2);
            }
          }
          else   // Last side joins ends...
          {
            if (iCorner < numCorner - 1)
            {
              faces_vec[iface][0] = ixyz + 1;
              faces_vec[iface][1] = -(ixyz + numCorner - nFaces + 1);
              faces_vec[iface][2] = ixyz + numCorner - nFaces + 2;
              faces_vec[iface][3] = -(ixyz + 2);
            }
            else
            {
              faces_vec[iface][0] = ixyz + 1;
              faces_vec[iface][1] = -(ixyz - nFaces + numCorner + 1);
              faces_vec[iface][2] = ixyz - nFaces + 2;
              faces_vec[iface][3] = -(ixyz - numCorner + 2);
            }
          }
          ++ixyz;
          ++iface;
        }
        phi += dPhi;
      }
    }
    G4Polyhedron* polyhedron = new G4Polyhedron;
    G4int problem = polyhedron->createPolyhedron(nNodes, nFaces, xyz, faces_vec);
    delete [] faces_vec;
    delete [] xyz;
    if (problem)
    {
      std::ostringstream message;
      message << "Problem creating G4Polyhedron for: " << GetName();
      G4Exception("G4GenericPolycone::CreatePolyhedron()", "GeomSolids1002",
                  JustWarning, message);
      delete polyhedron;
      return 0;
    }
    else
    {
      return polyhedron;
    }
}

#endif
