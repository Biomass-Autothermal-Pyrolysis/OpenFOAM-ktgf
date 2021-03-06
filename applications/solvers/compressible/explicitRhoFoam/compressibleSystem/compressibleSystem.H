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
    Foam::compressibleSystem

Description
    Class used to handle encoding/decoding of primative and conservative
    variables used in compressible flows. Also includes funcitons to calculate
    the conservative fluxes

SourceFiles
    compressibleSystem.C

\*---------------------------------------------------------------------------*/

#ifndef compressibleSystem_H
#define compressibleSystem_H

#include "IOdictionary.H"
#include "rhoThermo.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "fvMatrix.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class fluxIntegrator;
class fluxFunction;

/*---------------------------------------------------------------------------*\
                  Class compressibleSystem Declaration
\*---------------------------------------------------------------------------*/

class compressibleSystem
{
protected:
    // Protected data

        //- Name of compressible phase ('\0' by default)
        word phaseName_;

        //- Reference to the mesh
        const fvMesh& mesh_;

        //- Rho thermo class
        autoPtr<rhoThermo> thermoPtr_;

        //- Density field
        volScalarField rho_;

        //- velocity field
        volVectorField U_;

        //- Volumetric flux
        autoPtr<surfaceScalarField> phiPtr_;

        //- Refrence to thermodynamic pressure
        volScalarField& p_;

        //- Total internal energy
        volScalarField E_;

        //- Total enthalpy
        volScalarField H_;

        //  Conservative quantities

            //- Momentum
            volVectorField rhoU_;

            //- Total energy
            volScalarField rhoE_;

        //- Flux integrator
        autoPtr<fluxIntegrator> integrator_;

        //- Flux function
        autoPtr<fluxFunction> fluxFunction_;

        //  Conservative fluxes

            //- Mass flux
            surfaceScalarField massFlux_;

            //- Momentum flux
            surfaceVectorField momentumFlux_;

            //- Energy flux
            surfaceScalarField energyFlux_;


public:

    // Constructor
    compressibleSystem(const fvMesh& mesh, const word& name=word::null);

    //- Destructor
    virtual ~compressibleSystem();


    // Member Functions

        //- Return name
        inline const word& name() const
        {
            return phaseName_;
        }

        //- Const refrence to thermo
        inline const rhoThermo& thermo() const;

        //- Non-const refrence to thermo
        inline rhoThermo& thermo();

        //- Const refrence to density
        inline const volScalarField& rho() const;

        //- Non-const refrence to density
        inline volScalarField& rho();

        //- Const refrence to total internal energy
        inline const volScalarField& E() const;

        //- Non-const refrence to total energy
        inline volScalarField& E();

        //- Const refrence to total enthalpy
        inline const volScalarField& H() const;

        //- Non-const refrence to total enthalpy
        inline volScalarField& H();

        //- Const refrence to velocity
        inline const volVectorField& U() const;

        //- Non-const refrence to velocity
        inline volVectorField& U();

        //- Const refrence to volumetric flux
        inline const surfaceScalarField& phi() const;

        //- Non-const refrence to volumetric Flux
        inline surfaceScalarField& phi();

        //- Const refrence to thermodynamic pressure
        inline const volScalarField& p() const;

        //- Non-const refrence to thermodynamic pressure
        inline volScalarField& p();

        //- Const refrence to momentum
        inline const volVectorField& rhoU() const;

        //- Non-const refrence to momentum
        inline volVectorField& rhoU();

        //- Const refrence to total internal energy
        inline const volScalarField& rhoE() const;

        //- Non-const refrence to total internal energy
        inline volScalarField& rhoE();

        //- Constant refrence to mass flux
        inline const surfaceScalarField& massFlux() const;

        //- Non-constant refrence to mass flux
        inline surfaceScalarField& massFlux();

        //- Constant refrence to momentum flux
        inline const surfaceVectorField& momentumFlux() const;

        //- Non-constant refrence to momentum flux
        inline surfaceVectorField& momentumFlux();

        //- Constant refrence to energy flux
        inline const surfaceScalarField& energyFlux() const;

        //- Non-constant refrence to energy flux
        inline surfaceScalarField& energyFlux();


    // Edit functions

        //- Transport conservative variables
        virtual void integrateFluxes();

        //- Decode conservative variables
        virtual void updateFluxes();

        //- Correct primatives based on internal energy/enthalpy
        virtual void correctThermo();

        //- Encode primative variables
        virtual void encode();

        //- Decode conservative variables
        virtual void decode();

        //- Read base phaseProperties dictionary
        bool read();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "compressibleSystemI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
