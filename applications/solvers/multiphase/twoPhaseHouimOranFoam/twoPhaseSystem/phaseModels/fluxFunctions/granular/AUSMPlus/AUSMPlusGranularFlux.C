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

#include "AUSMPlusGranularFlux.H"
#include "surfaceInterpolate.H"
#include "fvc.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace granularFluxFunctions
{
    defineTypeNameAndDebug(AUSMPlusFlux, 0);
    addToRunTimeSelectionTable
    (
        granularFluxFunction,
        AUSMPlusFlux,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

Foam::tmp<Foam::surfaceScalarField>
Foam::granularFluxFunctions::AUSMPlusFlux::M1
(
    const surfaceScalarField& M,
    const label sign
)
{
    return 0.5*(M + sign*mag(M));
}

Foam::tmp<Foam::surfaceScalarField>
Foam::granularFluxFunctions::AUSMPlusFlux::M2
(
    const surfaceScalarField& M,
    const label sign
)
{
    return sign*0.25*sqr(M + sign);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::granularFluxFunctions::AUSMPlusFlux::AUSMPlusFlux
(
    const fvMesh& mesh,
    const word& phaseName
)
:
    granularFluxFunction(mesh, phaseName),
    fa_(dict_.lookupOrDefault("fa", 1.0)),
    D_(dict_.lookupOrDefault("D", 1.0)),
    alphaMax_(dict_.lookupOrDefault("alphaMax", 0.63)),
    alphaMinFriction_(dict_.lookupOrDefault("alphaMinFriction", 0.5)),
    cutOffMa_("small", dimless, epsilon_),
    residualRho_("small", dimDensity, epsilon_),
    residualU_("small", dimVelocity, epsilon_)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::granularFluxFunctions::AUSMPlusFlux::~AUSMPlusFlux()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void Foam::granularFluxFunctions::AUSMPlusFlux::updateFluxes
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
)
{
    surfaceVectorField normal(mesh_.Sf()/mesh_.magSf());
    surfaceScalarField alphaOwn
    (
        fvc::interpolate(alpha, own_, interpScheme(alpha.name()))
    );
    surfaceScalarField alphaNei
    (
        fvc::interpolate(alpha, nei_, interpScheme(alpha.name()))
    );

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

    surfaceScalarField EOwn(fvc::interpolate(E, own_, interpScheme(E.name())));
    surfaceScalarField ENei(fvc::interpolate(E, nei_, interpScheme(E.name())));

    surfaceScalarField ThetaOwn
    (
        fvc::interpolate(Theta, own_, interpScheme(Theta.name()))
    );
    surfaceScalarField ThetaNei
    (
        fvc::interpolate(Theta, nei_, interpScheme(Theta.name()))
    );

    surfaceScalarField pOwn(fvc::interpolate(p, own_, interpScheme(p.name())));
    surfaceScalarField pNei(fvc::interpolate(p, nei_, interpScheme(p.name())));

    surfaceScalarField aOwn(fvc::interpolate(a, own_, interpScheme(a.name())));
    surfaceScalarField aNei(fvc::interpolate(a, nei_, interpScheme(a.name())));


    surfaceScalarField UvOwn(UOwn & normal);
    surfaceScalarField UvNei(UNei & normal);

    surfaceScalarField a12
    (
        "aStar",
        sqrt
        (
            (alphaOwn*rhoOwn*sqr(aOwn) + alphaNei*rhoNei*sqr(aNei))
           /(alphaOwn*rhoOwn + alphaNei*rhoNei)
        ) + residualU_
    );

    // Compute slpit Mach numbers
    surfaceScalarField MaOwn("MaOwn", UvOwn/a12);
    surfaceScalarField MaNei("MaNei", UvNei/a12);
    surfaceScalarField magMaOwn(mag(MaOwn));
    surfaceScalarField magMaNei(mag(MaNei));
    surfaceScalarField MaBarSqr((sqr(UvOwn) + sqr(UvNei))/(2.0*sqr(a12)));

    surfaceScalarField Ma4Own
    (
        "Ma4Own",
        pos0(magMaOwn - 1)*M1(MaOwn, 1)
      + neg(magMaOwn - 1)*M2(MaOwn, 1)*(1.0 - 16.0*beta_*M2(MaOwn, -1))
    );
    surfaceScalarField Ma4Nei
    (
        "Ma4Nei",
        pos0(magMaNei - 1)*M1(MaNei, -1)
      + neg(magMaNei - 1)*M2(MaNei, -1)*(1.0 + 16.0*beta_*M2(MaNei, 1))
    );

    surfaceScalarField zeta
    (
        max
        (
            (max(alphaOwn, alphaNei) - alphaMinFriction_)
           /(alphaMax_ - alphaMinFriction_),
            0.0
        )
    );
    surfaceScalarField G(max(2.0*(1.0 - D_*sqr(zeta)), 0.0));

    scalar xi = 3.0/16.0*(5.0*sqr(fa_) - 4.0);
    surfaceScalarField Kp(0.25 + 0.75*(1.0 - G/2.0));
    surfaceScalarField Ku(0.75 + 0.25*(1.0 - G/2.0));
    surfaceScalarField sigma(0.75*G/2.0);

    surfaceScalarField Ma12
    (
        Ma4Own + Ma4Nei
      - 2.0*Kp/fa_*max(1.0 - sigma*MaBarSqr, 0.0)*(pNei - pOwn)
       /((alphaOwn*rhoOwn + alphaOwn*rhoOwn + residualRho_)*sqr(a12))
    );

    surfaceScalarField p5Own
    (
        "p5Own",
        pos0(magMaOwn - 1)*pos(MaOwn)
      + neg(magMaOwn - 1)
       *(M2(MaOwn, 1)*(2.0 - MaOwn) - 16.0*xi*MaOwn*M2(MaOwn, -1))
    );

    surfaceScalarField p5Nei
    (
        "p5Nei",
        pos0(magMaNei - 1)*neg(MaNei)
      + neg(magMaNei - 1)
       *(M2(MaNei, -1)*(-2.0 - MaNei) + 16.0*xi*MaNei*M2(MaNei, 1))
    );

    pf_ =
        -Ku*fa_*(a12 - residualU_)*p5Own*p5Nei
       *(alphaOwn*rhoNei + alphaNei*rhoNei)*(UvNei - UvOwn)
      + p5Own*pOwn + p5Nei*pNei;

    surfaceScalarField F
    (
        (a12 - residualU_)
       *(1.0 + mag(Ma12)*(1.0 - G/2.0))
       *max(alphaOwn, alphaNei)
       *(alphaOwn*rhoOwn - alphaNei*rhoNei)/(2.0*alphaMax_)
    );

    surfaceScalarField mDot
    (
        F
      + a12*Ma12
       *(
            pos(Ma12)*alphaOwn*rhoOwn
          + neg0(Ma12)*alphaNei*rhoNei
        )
    );

    alphaf_ = pos(mDot)*alphaOwn + neg0(mDot)*alphaNei;
    Uf_ = pos(mDot)*UOwn + neg0(mDot)*UNei;
    phi_ = Uf_ & mesh_.Sf();

    massFlux = mesh_.magSf()*mDot;
    momentumFlux = massFlux*(Uf_) + pf_*mesh_.Sf();
    energyFlux = massFlux*(pos(mDot)*EOwn + neg0(mDot)*ENei);
    PTEFlux =
        massFlux
       *(
            pos(mDot)*ThetaOwn
          + neg0(mDot)*ThetaNei
        )*3.0/2.0;
}