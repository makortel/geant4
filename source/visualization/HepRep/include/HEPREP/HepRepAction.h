// -*- C++ -*-
// AID-GENERATED
// =========================================================================
// This class was generated by AID - Abstract Interface Definition          
// DO NOT MODIFY, but use the org.freehep.aid.Aid utility to regenerate it. 
// =========================================================================
#ifndef HEPREP_HEPREPACTION_H
#define HEPREP_HEPREPACTION_H 1

// Copyright 2000-2005, FreeHEP.

#include <string>

namespace HEPREP {

/**
 * HepRepAction interface.
 *
 * @author Mark Donszelmann
 */
class HepRepAction {

public: 
    /// Destructor.
    virtual ~HepRepAction() { /* nop */; }

    /**
     * Returns name of this action.
     *
     * @return name of this action.
     */
    virtual std::string getName() = 0;

    /**
     * Returns expression associated with this action.
     *
     * @return expression of this action.
     */
    virtual std::string getExpression() = 0;

    /**
     * Returns a deep copy of this action.
     *
     * @return copy of this action.
     */
    virtual HepRepAction * copy() = 0;
}; // class
} // namespace HEPREP
#endif /* ifndef HEPREP_HEPREPACTION_H */
