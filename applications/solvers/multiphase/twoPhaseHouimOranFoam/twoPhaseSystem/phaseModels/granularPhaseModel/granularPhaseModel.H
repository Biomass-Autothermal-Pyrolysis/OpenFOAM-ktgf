/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2017 OpenFOAM Foundation
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
    Foam::granularPhaseModel

SourceFiles
    granularPhaseModel.C

\*---------------------------------------------------------------------------*/

#ifndef granularPhaseModel_H
#define granularPhaseModel_H

#include "phaseModel.H"
#include "dragModel.H"
#include "granularViscosityModel.H"
#include "conductivityModel.H"
#include "radialModel.H"
#include "granularPressureModel.H"
#include "frictionalStressModel.H"
#include "granularFluxFunction.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class granularPhaseModel Declaration
\*---------------------------------------------------------------------------*/

class granularPhaseModel
:
    public phaseModel
{
protected:
    // Protected data

        // Sub-models

            //- Run-time selected viscosity model
            autoPtr<kineticTheoryModels::viscosityModel>
                viscosityModel_;

            //- Run-time selected conductivity model
            autoPtr<kineticTheoryModels::conductivityModel>
                conductivityModel_;

            //- Run-time selected radial distribution model
            autoPtr<kineticTheoryModels::radialModel> radialModel_;

            //- Run-time selected granular pressure model
            autoPtr<kineticTheoryModels::granularPressureModel>
                granularPressureModel_;

            //- Run-time selected frictional stress model
            autoPtr<kineticTheoryModels::frictionalStressModel>
                frictionalStressModel_;


        // Kinetic Theory Model coefficients

            //- Use equilibrium approximation: generation == dissipation
            Switch equilibrium_;

            //- Coefficient of restitution
            dimensionedScalar e_;

            //- Maximum packing phase-fraction
            dimensionedScalar alphaMax_;

            //- Min value for which the frictional stresses are zero
            dimensionedScalar alphaMinFriction_;

            //- Maximum turbulent viscosity
            dimensionedScalar maxNut_;


        // Kinetic Theory Model Fields

            //- The granular energy/temperature
            volScalarField Theta_;

            //- The granular pressure (collisional and frictional)
            volScalarField Ps_;

            //- Frictional pressure
            volScalarField Pfric_;

            //- The granular bulk viscosity
            volScalarField lambda_;

            //- The granular radial distribution
            volScalarField gs0_;

            //- The granular "thermal" conductivity
            volScalarField kappa_;

            //- The effective viscosity
            volScalarField nut_;

            //- The frictional viscosity
            volScalarField nuFric_;


        //- Phase psudo-thermal energy
        volScalarField alphaRhoPTE_;

        //- Phase psudo thermal energy flux
        surfaceScalarField PTEFlux_;

        //- Model to calculate hyperbolic phase flux
        autoPtr<granularFluxFunction> fluxFunction_;


public:

    // Constructors
        granularPhaseModel
        (
            const twoPhaseSystem& fluid,
            const dictionary& dict,
            const word& phaseName
        );


    //- Destructor
    virtual ~granularPhaseModel();


    // Access

        //- Return that the phase uses a slave pressure
        virtual bool slavePressure() const
        {
            return false;
        }

        //- Is the phase granular
        virtual bool granular() const
        {
            return true;
        }

        //- Const access to the phase pseudo thermal energy
        virtual const volScalarField& Theta() const
        {
            return Theta_;
        }

        //- Non-const access to the phase pseudo thermal energy
        virtual volScalarField& Theta()
        {
            return Theta_;
        }

        //- Return the particle pressure
        virtual const volScalarField& p() const
        {
            return Ps_;
        }

        //- Non-const access to the phase pressure
        virtual volScalarField& p()
        {
            return Ps_;
        }

        //- Speed of sound in the phase
        virtual tmp<volScalarField> c() const;

        //- Const access to phasic thermal energy
        virtual const volScalarField& alphaRhoPTE() const
        {
            return alphaRhoPTE_;
        }

        //- Non-const access to phasic thermal energy
        virtual volScalarField& alphaRhoPTE()
        {
            return alphaRhoPTE_;
        }

        //- Const access to phase thermal energy flux
        virtual const surfaceScalarField& PTEFlux() const
        {
            return PTEFlux_;
        }

        //- Non-const access to phase thermal energy flux
        virtual surfaceScalarField& thermalEnergyFlux()
        {
            return PTEFlux_;
        }

        //- Return the phase volumentric flux
        virtual const surfaceScalarField& phi() const
        {
            return fluxFunction_->phi();
        }

        //- Return the phase volumentric flux
        virtual tmp<surfaceScalarField> alphaPhi() const
        {
            return fluxFunction_->alphaPhi();
        }

        //- Return the gradient of pressure
        virtual const surfaceScalarField& alphaf() const
        {
            return fluxFunction_->alphaf();
        }

        //- Return the gradient of velocity
        virtual tmp<volTensorField> gradU() const
        {
            return fluxFunction_->gradU();
        }

        //- Return the gradient of pressure
        virtual tmp<volVectorField> gradp() const
        {
            return fluxFunction_->gradp();
        }

        //- Return the turbulence kinetic energy
        virtual tmp<volScalarField> k() const;

        //- Return the turbulence kinetic energy dissipation rate
        virtual tmp<volScalarField> epsilon() const;

        //- Return the Reynolds stress tensor
        virtual tmp<volSymmTensorField> R() const;

        //- Return the phase-pressure'
        // (derivative of phase-pressure w.r.t. phase-fraction)
        virtual tmp<volScalarField> pPrime() const;

        //- Return the face-phase-pressure'
        // (derivative of phase-pressure w.r.t. phase-fraction)
        virtual tmp<surfaceScalarField> pPrimef() const;

        //- Return the effective stress tensor
        virtual tmp<volSymmTensorField> devRhoReff() const;

        //- Return the source term for the momentum equation
        virtual tmp<fvVectorMatrix> divDevRhoReff(volVectorField& U) const;


    // Member Functions

        //- Return granular dissipation coefficient
        virtual tmp<volScalarField> dissipationCoeff() const;

        //- Return granular production coefficient
        virtual tmp<volScalarField> productionCoeff() const;

        //- Advect with conservative fluxes
        virtual void advect
        (
            const label stepi,
            const scalarList& coeffs,
            const scalarList& Fcoeffs,
            const dimensionedScalar& deltaT,
            const dimensionedVector& g,
            const volVectorField& F,
            const volVectorField& Ui,
            const volScalarField& pi
        );

        //- Update hyperbolic fluxes
        virtual void updateFluxes();

        //- Update hyperbolic fluxes using a set interplated volume fraction
        virtual void updateFluxes(const surfaceScalarField& alphaf);

        //- Correct primatives based on internal energy/enthalpy
        virtual void correctThermo();

        //- Encode primative variables
        virtual void encode();

        //- Decode conservative variables
        virtual void decode();

        //- Set old time values to current
        virtual void store();

        //- Read phaseProperties dictionary
        virtual bool read(const dictionary& phaseProperties);
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
