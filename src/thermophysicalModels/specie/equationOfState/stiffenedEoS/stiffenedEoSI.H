/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2013-2017 OpenFOAM Foundation
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

#include "stiffenedEoS.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class Specie>
inline Foam::stiffenedEoS<Specie>::stiffenedEoS
(
    const Specie& sp,
    const scalar p0,
    const scalar rho0,
    const scalar gamma,
    const scalar B
)
:
    Specie(sp),
    p0_(p0),
    rho0_(rho0),
    gamma_(gamma),
    B_(B)
{}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Specie>
inline Foam::stiffenedEoS<Specie>::stiffenedEoS
(
    const word& name,
    const stiffenedEoS<Specie>& pf
)
:
    Specie(name, pf),
    p0_(pf.p0_),
    rho0_(pf.rho0_),
    gamma_(pf.gamma_),
    B_(pf.B_)
{}


template<class Specie>
inline Foam::autoPtr<Foam::stiffenedEoS<Specie>>
Foam::stiffenedEoS<Specie>::clone() const
{
    return autoPtr<stiffenedEoS<Specie>>
    (
        new stiffenedEoS<Specie>(*this)
    );
}


template<class Specie>
inline Foam::autoPtr<Foam::stiffenedEoS<Specie>>
Foam::stiffenedEoS<Specie>::New
(
    const dictionary& dict
)
{
    return autoPtr<stiffenedEoS<Specie>>
    (
        new stiffenedEoS<Specie>(dict)
    );
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Specie>
inline Foam::scalar Foam::stiffenedEoS<Specie>::rho
(
    scalar p,
    scalar T
) const
{
    return rho0_*pow((gamma_*p + B_)/(gamma_*p0_ + B_), 1.0/gamma_);
}


template<class Specie>
inline Foam::scalar Foam::stiffenedEoS<Specie>::p
(
    scalar T,
    scalar rho
) const
{
    return ((gamma_*p0_ + B_)*pow(rho/rho0_, gamma_) - B_)/gamma_;
}


template<class Specie>
inline Foam::scalar Foam::stiffenedEoS<Specie>::H
(
    scalar p,
    scalar T
) const
{
    return 0;
}


template<class Specie>
inline Foam::scalar Foam::stiffenedEoS<Specie>::Cp
(
    scalar p,
    scalar T
) const
{
    return 0;
}


template<class Specie>
inline Foam::scalar Foam::stiffenedEoS<Specie>::S
(
    scalar p,
    scalar T
) const
{
    scalar n = 1 - 1.0/gamma_;
    return
      - pow(gamma_*p0_ + B_, 1.0/gamma_)*(pow((p + B_), n)
      - pow((Pstd + B_), n))
       /(rho0_*T*n);
}


template<class Specie>
inline Foam::scalar Foam::stiffenedEoS<Specie>::psi
(
    scalar p,
    scalar T
) const
{
    return
        (rho0_/(gamma_*(gamma_*p0_ + B_)))
       *pow((gamma_*p + B_)/(gamma_*p0_ + B_), 1.0/gamma_ - 1.0);
}


template<class Specie>
inline Foam::scalar Foam::stiffenedEoS<Specie>::Z(scalar, scalar) const
{
    return 1;
}


template<class Specie>
inline Foam::scalar Foam::stiffenedEoS<Specie>::CpMCv
(
    scalar p,
    scalar T
) const
{
    return 0;
}


template<class Specie>
inline Foam::scalar Foam::stiffenedEoS<Specie>::dpdT
(
    scalar T,
    scalar rho
) const
{
    return 0;
}


template<class Specie>
inline Foam::scalar Foam::stiffenedEoS<Specie>::dpdv
(
    scalar T,
    scalar rho
) const
{
    return -(gamma_*p0_ + B_)*rho0_*pow(rho/rho0_, gamma_ + 1);
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class Specie>
inline void Foam::stiffenedEoS<Specie>::operator+=
(
    const stiffenedEoS<Specie>& pf
)
{
    scalar Y1 = this->Y();
    Specie::operator+=(pf);

    if (mag(this->Y()) > SMALL)
    {
        Y1 /= this->Y();
        const scalar Y2 = pf.Y()/this->Y();

        p0_ = Y1*p0_ + Y2*pf.p0_;
        rho0_ = Y1*rho0_ + Y2*pf.rho0_;
        gamma_ = Y1*gamma_ + Y2*pf.gamma_;
        B_ = Y1*B_ + Y2*pf.B_;
    }
}


template<class Specie>
inline void Foam::stiffenedEoS<Specie>::operator*=(const scalar s)
{
    Specie::operator*=(s);
}


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

template<class Specie>
inline Foam::stiffenedEoS<Specie> Foam::operator+
(
    const stiffenedEoS<Specie>& pf1,
    const stiffenedEoS<Specie>& pf2
)
{
    Specie sp
    (
        static_cast<const Specie&>(pf1)
      + static_cast<const Specie&>(pf2)
    );

    if (mag(sp.Y()) < SMALL)
    {
        return stiffenedEoS<Specie>
        (
            sp,
            pf1.p0_,
            pf1.rho0_,
            pf1.gamma_,
            pf1.B_
        );
    }
    else
    {
        const scalar Y1 = pf1.Y()/sp.Y();
        const scalar Y2 = pf2.Y()/sp.Y();

        return stiffenedEoS<Specie>
        (
            sp,
            Y1*pf1.p0_ + Y2*pf2.p0_,
            Y1*pf1.rho0_ + Y2*pf2.rho0_,
            Y1*pf1.gamma_ + Y2*pf2.gamma_,
            Y1*pf1.B_ + Y2*pf2.B_
        );
    }
}


template<class Specie>
inline Foam::stiffenedEoS<Specie> Foam::operator*
(
    const scalar s,
    const stiffenedEoS<Specie>& pf
)
{
    return stiffenedEoS<Specie>
    (
        s*static_cast<const Specie&>(pf),
        pf.p0_,
        pf.rho0_,
        pf.gamma_,
        pf.B_
    );
}


template<class Specie>
inline Foam::stiffenedEoS<Specie> Foam::operator==
(
    const stiffenedEoS<Specie>& pf1,
    const stiffenedEoS<Specie>& pf2
)
{
    Specie sp
    (
        static_cast<const Specie&>(pf1)
     == static_cast<const Specie&>(pf2)
    );

    const scalar Y1 = pf1.Y()/sp.Y();
    const scalar Y2 = pf2.Y()/sp.Y();

    return stiffenedEoS<Specie>
    (
        sp,
        Y2*pf2.p0_    - Y1*pf1.p0_,
        Y2*pf2.rho0_  - Y1*pf1.rho0_,
        Y2*pf2.gamma_ - Y1*pf1.gamma_,
        Y2*pf2.B_     - Y1*pf1.B_
    );
}


// ************************************************************************* //
