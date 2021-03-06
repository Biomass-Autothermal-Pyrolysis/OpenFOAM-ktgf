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

#include "AUSMPlusFlux.H"
#include "surfaceInterpolate.H"
#include "fvc.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace fluxFunctions
{
    defineTypeNameAndDebug(AUSMPlus, 0);
    addToRunTimeSelectionTable(fluxFunction, AUSMPlus, dictionary);
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fluxFunctions::AUSMPlus::AUSMPlus
(
    const fvMesh& mesh,
    const word& phaseName
)
:
    fluxFunction(mesh, phaseName)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::fluxFunctions::AUSMPlus::~AUSMPlus()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void Foam::fluxFunctions::AUSMPlus::updateFluxes
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
    dimensionedScalar minU("smallU", dimVelocity, SMALL);

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

    surfaceScalarField aOwn
    (
        fvc::interpolate(a, own_, interpScheme(a.name()))
    );
    surfaceScalarField aNei
    (
        fvc::interpolate(a, nei_, interpScheme(a.name()))
    );

    surfaceScalarField UvOwn(UOwn & normal);
    surfaceScalarField UvNei(UNei & normal);

    // Compute slpit Mach numbers
    surfaceScalarField MaOwn("MaOwn", UvOwn/max(aOwn, minU));
    surfaceScalarField MaNei("MaNei", UvNei/max(aNei, minU));
    surfaceScalarField magMaOwn(mag(MaOwn));
    surfaceScalarField magMaNei(mag(MaNei));

    surfaceScalarField deltapOwn
    (
        "deltapOwn",
        pos0(magMaOwn - 1)*sign(MaOwn)
      + neg(magMaOwn - 1)
       *MaOwn/2.0
       *(
            3.0
          - sqr(MaOwn)
          + 4.0*alpha_*sqr(sqr(MaOwn - 1.0))
        )
    );

    surfaceScalarField deltapNei
    (
        "deltapNei",
        pos0(magMaNei - 1)*sign(MaNei)
      + neg(magMaNei - 1)
       *MaNei/2.0
       *(
            3.0
          - sqr(MaNei)
          + 4.0*alpha_*sqr(sqr(MaNei - 1.0))
        )
    );

    surfaceScalarField deltap("deltap", deltapOwn*pOwn + deltapNei*pNei);

    surfaceScalarField magMachOwn
    (
        "magMachOwn",
        pos0(magMaOwn - 1)*magMaOwn
      + neg(magMaOwn - 1)
       *(
            0.5*(sqr(MaOwn) + 1.0)
          + 2.0*beta_*sqr(sqr(MaOwn) - 1.0)
        )
    );
    surfaceScalarField magMachNei
    (
        "magMachNei",
        pos0(magMaNei - 1)*magMaNei
      + neg(magMaNei - 1)
       *(
            0.5*(sqr(MaNei) + 1.0)
          + 2.0*beta_*sqr(sqr(MaNei) - 1.0)
        )
    );
    surfaceScalarField deltaMa12
    (
        "deltaMa12",
        magMachOwn - magMachNei
    );
    surfaceScalarField Ma12
    (
        "Ma12",
        MaOwn + MaNei - deltaMa12
    );

    surfaceScalarField a12("a12", sqrt(aOwn*aNei));
    surfaceScalarField rhoPhi(fvc::interpolate(rho*U) & normal);
    surfaceVectorField rhoUPhi
    (
        (fvc::interpolate(rho*U*U) & normal)
      + fvc::interpolate(p)*normal
    );
    surfaceScalarField rhoHPhi(fvc::interpolate(rho*H*U) & normal);

    massFlux =
        mesh_.magSf()
       *(
            rhoPhi
          - 0.5*a12
           *(
                (0.5*deltaMa12 - mag(Ma12))*rhoOwn
              + (0.5*deltaMa12 + mag(Ma12))*rhoNei
            )
        );

    momentumFlux =
        mesh_.magSf()
       *(
            rhoUPhi
          - 0.5*a12
           *(
                (0.5*deltaMa12 - mag(Ma12))*rhoOwn*UOwn
              + (0.5*deltaMa12 + mag(Ma12))*rhoNei*UNei
            )
        );
      - 0.5*deltap*mesh_.Sf();

    energyFlux =
        mesh_.magSf()
       *(
            rhoHPhi
          - 0.5*a12
           *(
                (0.5*deltaMa12 - mag(Ma12))*rhoOwn*HOwn
              + (0.5*deltaMa12 + mag(Ma12))*rhoNei*HNei
            )
        );
}
