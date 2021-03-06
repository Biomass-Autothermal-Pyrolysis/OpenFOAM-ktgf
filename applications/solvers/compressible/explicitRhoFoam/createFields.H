#include "createRDeltaT.H"

Info<< "Reading thermophysical properties\n" << endl;

compressibleSystem fluid(mesh);

fluidThermo& thermo = fluid.thermo();
thermo.validate(args.executable(), "h", "e");

const volScalarField& rho = fluid.rho();

const volVectorField& U = fluid.U();
const surfaceScalarField& phi = fluid.phi();

autoPtr<compressible::turbulenceModel> turbulence
(
    compressible::turbulenceModel::New
    (
        rho,
        U,
        phi,
        fluid.thermo()
    )
);

const volScalarField& muEff(fluid.thermo().mu());

#include "createMRF.H"
