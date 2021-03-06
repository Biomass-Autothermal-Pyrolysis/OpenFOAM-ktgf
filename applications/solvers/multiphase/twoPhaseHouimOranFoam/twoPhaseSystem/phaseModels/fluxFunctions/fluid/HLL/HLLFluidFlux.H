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
    Foam::fluidFluxFunctions::HLLFluidFlux

Description
    Interpolates conservative and primative variables and updates fluxes

SourceFiles
    HLLFluidFlux.C

\*---------------------------------------------------------------------------*/

#ifndef HLLFluidFlux_H
#define HLLFluidFlux_H

#include "fluidFluxFunction.H"
#include "volFields.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fluidFluxFunctions
{

/*---------------------------------------------------------------------------*\
                  Class HLLFunction Declaration
\*---------------------------------------------------------------------------*/

class HLLFlux
:
    public fluidFluxFunction
{
    // Private Data

public:

    //- Runtime type information
    TypeName("HLL");

    // Costructor
    HLLFlux(const fvMesh& mesh, const word& phaseName);


    //- Destructor
    virtual ~HLLFlux();


    // Public Functions

        //- Update phase fluxes
        virtual void updateFluxes
        (
            surfaceScalarField& massFlux,
            surfaceVectorField& momentumFlux,
            surfaceScalarField& energyFlux,
            const volScalarField& alpha,
            const volScalarField& rho,
            const volVectorField& U,
            const volScalarField& H,
            const volScalarField& p,
            const volScalarField& a,
            const volVectorField& Ui,
            const volScalarField& pi
        );

        //- Update phase fluxes with a predefined alpha
        virtual void updateFluxes
        (
            surfaceScalarField& massFlux,
            surfaceVectorField& momentumFlux,
            surfaceScalarField& energyFlux,
            const surfaceScalarField& alphaf,
            const volScalarField& rho,
            const volVectorField& U,
            const volScalarField& H,
            const volScalarField& p,
            const volScalarField& a,
            const volVectorField& Ui,
            const volScalarField& pi
        );

};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam
} // End namespace fluidFluxFunctions

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif