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

\*---------------------------------------------------------------------------*/

#include "HLLFlux.H"
#include "surfaceInterpolate.H"
#include "fvc.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace fluxFunctions
{
    defineTypeNameAndDebug(HLL, 0);
    addToRunTimeSelectionTable(fluxFunction, HLL, dictionary);
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fluxFunctions::HLL::HLL(const fvMesh& mesh, const word& phaseName)
:
    fluxFunction(mesh, phaseName)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::fluxFunctions::HLL::~HLL()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void Foam::fluxFunctions::HLL::updateFluxes
(
    surfaceScalarField& massFlux,
    surfaceVectorField& momentumFlux,
    surfaceScalarField& energyFlux,
    const volScalarField& rho,
    const volVectorField& U,
    const volScalarField& H,
    const volScalarField& p,
    const volScalarField& a
)
{
    surfaceVectorField normal(mesh_.Sf()/mesh_.magSf());

    surfaceScalarField rhoOwn
    (
        fvc::interpolate(rho, own_, interpScheme(rho.name()))
    );
    surfaceScalarField rhoNei
    (
        fvc::interpolate(rho, nei_, interpScheme(rho.name()))
    );

    surfaceVectorField UOwn(fvc::interpolate(U, own_, interpScheme(U.name())));
    surfaceVectorField UNei(fvc::interpolate(U, nei_, interpScheme(U.name())));

    surfaceScalarField HOwn(fvc::interpolate(H, own_, interpScheme(H.name())));
    surfaceScalarField HNei(fvc::interpolate(H, nei_, interpScheme(H.name())));

    surfaceScalarField pOwn(fvc::interpolate(p, own_, interpScheme(p.name())));
    surfaceScalarField pNei(fvc::interpolate(p, nei_, interpScheme(p.name())));

    surfaceScalarField aOwn(fvc::interpolate(a, own_, interpScheme(a.name())));
    surfaceScalarField aNei(fvc::interpolate(a, nei_, interpScheme(a.name())));
    surfaceScalarField UvOwn(UOwn & normal);
    surfaceScalarField UvNei(UNei & normal);

    surfaceScalarField EOwn("EOwn", HOwn - pOwn/rhoOwn);
    surfaceScalarField ENei("ENei", HNei - pNei/rhoNei);

    // Averages
    surfaceScalarField aBar("aBar", 0.5*(aOwn + aNei));
    surfaceScalarField rhoBar("rhoBar", 0.5*(rhoOwn + rhoNei));

    // Fluxes
    surfaceScalarField massFluxOwn(rhoOwn*UvOwn);
    surfaceScalarField massFluxNei(rhoNei*UvNei);

    surfaceVectorField momentumFluxOwn(UOwn*massFluxOwn + pOwn*normal);
    surfaceVectorField momentumFluxNei(UNei*massFluxNei + pNei*normal);

    surfaceScalarField energyFluxOwn(HOwn*massFluxOwn);
    surfaceScalarField energyFluxNei(HNei*massFluxNei);

    surfaceScalarField SOwn("SOwn", min(UvOwn - aOwn, UvNei - aNei));
    surfaceScalarField SNei("SNei", max(UvNei + aNei, UvOwn + aNei));
    surfaceScalarField rDeltaS("rDeltaS", 1.0/(SNei - SOwn));

    // Compute fluxes
    massFlux =
        mesh_.magSf()
       *(
            pos0(SOwn)*massFluxOwn
          + pos0(SNei)*neg(SOwn)
           *(
                SNei*massFluxOwn - SOwn*massFluxNei
              + SOwn*SNei*(rhoNei - rhoOwn)
            )*rDeltaS
          + neg(SNei)*massFluxNei
        );

    momentumFlux =
        mesh_.magSf()
       *(
            pos0(SOwn)*momentumFluxOwn
          + pos0(SNei)*neg(SOwn)
           *(
                SNei*momentumFluxOwn - SOwn*momentumFluxNei
              + SOwn*SNei*(rhoNei*UNei - rhoOwn*UOwn)
            )*rDeltaS
          + neg(SNei)*momentumFluxNei
        );

    energyFlux =
        mesh_.magSf()
       *(
            pos0(SOwn)*energyFluxOwn
          + pos0(SNei)*neg(SOwn)
           *(
                SNei*energyFluxOwn - SOwn*energyFluxNei
              + SOwn*SNei*(rhoNei*ENei - rhoOwn*EOwn)
            )*rDeltaS
          + neg(SNei)*energyFluxNei
        );
}
