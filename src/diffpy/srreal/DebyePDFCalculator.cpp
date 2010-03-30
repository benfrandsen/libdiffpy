/*****************************************************************************
*
* diffpy.srreal     by DANSE Diffraction group
*                   Simon J. L. Billinge
*                   (c) 2010 Trustees of the Columbia University
*                   in the City of New York.  All rights reserved.
*
* File coded by:    Pavol Juhas, Chris Farrow
*
* See AUTHORS.txt for a list of people who contributed.
* See LICENSE.txt for license information.
*
******************************************************************************
*
* class DebyePDFCalculator -- calculate PDF from the Debye equation.
*
* $Id$
*
*****************************************************************************/

#include <cassert>
#include <valarray>
#include <stdexcept>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_complex.h>

#include <diffpy/srreal/DebyePDFCalculator.hpp>
#include <diffpy/mathutils.hpp>

using namespace std;
using diffpy::mathutils::eps_gt;

namespace diffpy {
namespace srreal {

// Constructor ---------------------------------------------------------------

DebyePDFCalculator::DebyePDFCalculator()
{
    // default configuration
    this->setPeakWidthModel("jeong");
    this->setRstep(0.01);
    this->setMaxExtension(10.0);
    // attributes
    // FIXME
}

// Public Methods ------------------------------------------------------------

QuantityType DebyePDFCalculator::getPDF() const
{
    int nfromdr = M_PI / this->getRstep() / this->getQstep();
    int nrequired = max(this->totalQPoints(), nfromdr);
    int nlog2 = int(floor(log2(nrequired))) + 1;
    int padlen = int(pow(2, nlog2));
    // complex valarray needs to have twice as many elements
    valarray<double> ftog(0.0, 2 * padlen);
    QuantityType fqext = this->getExtendedF();
    assert(fqext.size() * 2 <= ftog.size());
    QuantityType::const_iterator fe = fqext.begin();
    double* pfc = &(ftog[0]);
    for (; fe != fqext.end(); ++fe, pfc += 2)  { *pfc = *fe; }
    // apply inverse fft
    int status;
    status = gsl_fft_complex_radix2_inverse(&(ftog[0]), 1, padlen);
    if (status != GSL_SUCCESS)
    {
        const char* emsgft = "Fourier Transformation failed.";
        throw invalid_argument(emsgft);
    }
    // normalize complex part by drpad = 2 * pi / Qmax_padded
    double drpad = 2 * M_PI / (padlen * this->getQstep());
    valarray<double> gpad(0.0, padlen / 2);
    for (int i = 0; i < padlen / 2; ++i)
    {
        gpad[i] = drpad * ftog[2 * i + 1];
    }
    // interpolate to the output r-grid
    QuantityType rgrid = this->getRgrid();
    QuantityType pdf(rgrid.size());
    QuantityType::const_iterator ri = rgrid.begin();
    QuantityType::iterator pdfi = pdf.begin();
    for (; ri != rgrid.end(); ++ri, ++pdfi)
    {
        double xdrp = *ri / drpad;
        int iplo = int(xdrp);
        int iphi = iplo + 1;
        double wphi = xdrp - iplo;
        double wplo = 1.0 - wphi;
        assert(iphi < int(gpad.size()));
        *pdfi = wplo * gpad[iplo] + wphi * gpad[iphi];
    }
    // Heureka should be ready here
    return pdf;
}


QuantityType DebyePDFCalculator::getRgrid() const
{
    double ndrmin = int(this->getRmin() / this->getRstep());
    double ndrmax = int(ceil(this->getRmax() / this->getRstep()));
    QuantityType rv;
    rv.reserve(ndrmax - ndrmin);
    for (int ndr = ndrmin; ndr < ndrmax; ++ndr)
    {
        rv.push_back(ndr * this->getRstep());
    }
    return rv;
}


void DebyePDFCalculator::setRstep(double rstep)
{
    if (!(eps_gt(rstep, 0.0)))
    {
        const char* emsg = "Rstep must be positive.";
        throw invalid_argument(emsg);
    }
    mrstep = rstep;
}


const double& DebyePDFCalculator::getRstep() const
{
    return mrstep;
}


void DebyePDFCalculator::setMaxExtension(double maxextension)
{
    if (maxextension < 0.0)
    {
        const char* emsg = "maxextension cannot be negative.";
        throw invalid_argument(emsg);
    }
    mmaxextension = maxextension;
}


const double& DebyePDFCalculator::getMaxExtension() const
{
    return mmaxextension;
}


const double& DebyePDFCalculator::getExtendedRmin() const
{
    // FIXME
    return this->getRmin();
}


const double& DebyePDFCalculator::getExtendedRmax() const
{
    // FIXME
    return this->getRmax();
}

// Protected Methods ---------------------------------------------------------

// attributes overload to direct visitors around data structures

void DebyePDFCalculator::accept(diffpy::BaseAttributesVisitor& v)
{
    // FIXME
}


void DebyePDFCalculator::accept(diffpy::BaseAttributesVisitor& v) const
{
    // FIXME
}

// BaseDebyeSum overloads

void DebyePDFCalculator::setupPairScale(const BaseBondGenerator& bnds)
{
    // FIXME
}


double DebyePDFCalculator::pairScale(const double& Q) const
{
    // FIXME
    return 1.0;
}


double DebyePDFCalculator::sfSiteAtQ(int, const double& Q) const
{
    // FIXME
    return 1.0;
}


}   // namespace srreal
}   // namespace diffpy

// End of file
