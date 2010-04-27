/*****************************************************************************
*
* diffpy.srreal     by DANSE Diffraction group
*                   Simon J. L. Billinge
*                   (c) 2010 Trustees of the Columbia University
*                   in the City of New York.  All rights reserved.
*
* File coded by:    Pavol Juhas
*
* See AUTHORS.txt for a list of people who contributed.
* See LICENSE.txt for license information.
*
******************************************************************************
*
* class HasClassRegistry -- template class providing registry for factory
*   functions that produce derived classes.  To be used as a base for a
*   specific class branch.  For a proper function, the HasClassRegistry
*   template must be instantiated in just one cpp file that includes the
*   implementatios in HasClassRegistry.ipp.
*
*   A Base class with registry should be declared as
*     class Base : public HasClassRegistry<Base>
*   The derived classes need to implement create, clone and type methods.
*
* $Id$
*
*****************************************************************************/

#ifndef CLASSREGISTRY1_HPP_INCLUDED
#define CLASSREGISTRY1_HPP_INCLUDED

#include <string>
#include <sstream>
#include <sstream>
#include <stdexcept>
#include <set>
#include <map>

#include <boost/shared_ptr.hpp>

namespace diffpy {

template <class TBase>
class HasClassRegistry
{
    public:

        // types
        typedef boost::shared_ptr<TBase> SharedPtr;

        // Contains virtual methods
        virtual ~HasClassRegistry()  { }

        // methods that have to be implemented in the TBase-derived classes
        virtual SharedPtr create() const = 0;
        virtual SharedPtr clone() const = 0;
        virtual const std::string& type() const = 0;

        // methods provided by HasClassRegistry

        /// Add a prototype of this instance to the registry
        bool registerThisType() const;

        /// Make registered type tp available under a different alias
        static bool aliasType(const std::string& tp, const std::string& al);

        /// Create new instance of a specified string type.
        static SharedPtr createByType(const std::string& tp);

        /// Return a set of all registered string types
        static std::set<std::string> getRegisteredTypes();

    private:

        typedef std::map<std::string, SharedPtr> RegistryStorage;

        /// Return a singleton instance of internal registry
        static RegistryStorage& getRegistry();
};

}   // namespace diffpy

#endif  // CLASSREGISTRY1_HPP_INCLUDED