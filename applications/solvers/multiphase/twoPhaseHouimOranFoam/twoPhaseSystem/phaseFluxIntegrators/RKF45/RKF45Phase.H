/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2017 Jeff Heylmun
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::phaseFluxIntegrators::RKF45Phase

Description
    Fourth order Runge-Kutta-Feglberg

SourceFiles
    RKF45Phase.C

\*---------------------------------------------------------------------------*/

#ifndef RKF45Phase_H
#define RKF45Phase_H

#include "phaseFluxIntegrator.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace phaseFluxIntegrators
{

/*---------------------------------------------------------------------------*\
                        Class RK45 Declaration
\*---------------------------------------------------------------------------*/

class RKF45Phase
:
    public phaseFluxIntegrator
{
    //- Number of steps
    virtual label nSteps() const
    {
        return 6;
    }

    //- Conserved variable coeffients
    virtual List<scalarList> coeffs() const;

    //- Flux coefficients
    virtual List<scalarList> Fcoeffs() const;


public:

    //- Runtime type information
    TypeName("RKF45");

    // Constructor
    RKF45Phase(phaseModel& phase1, phaseModel& phase2);

    //- Destructor
    virtual ~RKF45Phase();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace phaseFluxIntegrators
} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
