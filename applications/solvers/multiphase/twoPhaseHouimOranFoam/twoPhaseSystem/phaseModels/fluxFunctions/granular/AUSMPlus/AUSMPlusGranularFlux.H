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
    Foam::granularFluxFunctions::AUSMPlusFlux

Description
    Interpolates conservative and primative variables and updates fluxes

SourceFiles
    AUSMPlusGranularFlux.C

\*---------------------------------------------------------------------------*/

#ifndef AUSMPlusGranularFlux_H
#define AUSMPlusGranularFlux_H

#include "granularFluxFunction.H"
#include "volFields.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace granularFluxFunctions
{

/*---------------------------------------------------------------------------*\
                  Class fluxFunction Declaration
\*---------------------------------------------------------------------------*/

class AUSMPlusFlux
:
    public granularFluxFunction
{
    // Private Data

        //- Coefficients
        scalar beta_ = 0.125;
        scalar fa_;

        //- Dispersion coefficient
        scalar D_;

        //- Max volume fraction
        scalar alphaMax_;

        //- Minimum volume fraction where frictional pressure is present
        scalar alphaMinFriction_;

        //- Cut off mach Number
        dimensionedScalar cutOffMa_;

        //- Residual density
        dimensionedScalar residualRho_;

        //- Minimum velocity
        dimensionedScalar residualU_;


        // Private functions

            //- First polynomial
            tmp<surfaceScalarField> M1
            (
                const surfaceScalarField& M,
                const label sign
            );

            //- Second polynomial
            tmp<surfaceScalarField> M2
            (
                const surfaceScalarField& M,
                const label sign
            );


public:

    //- Runtime type information
    TypeName("AUSMPlus");

    // Costructor
    AUSMPlusFlux(const fvMesh& mesh, const word& phaseName);


    //- Destructor
    virtual ~AUSMPlusFlux();


    // Public Functions

        //- Update fluxes for a granular phase
        virtual void updateFluxes
        (
            surfaceScalarField& massFlux,
            surfaceVectorField& momentumFlux,
            surfaceScalarField& energyFlux,
            surfaceScalarField& PTEFlux,
            const volScalarField& alpha,
            const volScalarField& rho,
            const volVectorField& U,
            const volScalarField& E,
            const volScalarField& Theta,
            const volScalarField& p,
            const volScalarField& a,
            const volScalarField& pi
        );

};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace granularFluxFunction
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif