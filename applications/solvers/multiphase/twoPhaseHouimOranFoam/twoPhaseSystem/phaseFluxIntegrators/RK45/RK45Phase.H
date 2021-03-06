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
    Foam::phaseFluxIntegrators::RK45Phase

Description
    Fourth order Runge-Kutta ode time integrater

SourceFiles
    RK45Phase.C

\*---------------------------------------------------------------------------*/

#ifndef RK45Phase_H
#define RK45Phase_H

#include "phaseFluxIntegrator.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace phaseFluxIntegrators
{

/*---------------------------------------------------------------------------*\
                        Class RK45 Declaration
\*---------------------------------------------------------------------------*/

class RK45Phase
:
    public phaseFluxIntegrator
{
    //- Number of steps
    virtual label nSteps() const
    {
        return 4;
    }

    //- Conserved variable coeffients
    virtual List<scalarList> coeffs() const;

    //- Flux coefficients
    virtual List<scalarList> Fcoeffs() const;


public:

    //- Runtime type information
    TypeName("RK45");

    // Constructor
    RK45Phase(phaseModel& phase1, phaseModel& phase2);

    //- Destructor
    virtual ~RK45Phase();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace phaseFluxIntegrators
} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
