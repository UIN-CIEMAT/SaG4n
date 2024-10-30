


# SaG4n - Simulation of (alpha,xn) reactions with Geant4

Authors: Emilio Mendoza Cembranos, Daniel Cano Ott, Vicente Pesudo Fortes and Roberto Santorelli

Contact: [emilio.mendoza@ciemat.es](mailto:emilio.mendoza@ciemat.es), [daniel.cano@ciemat.es](mailto:daniel.cano@ciemat.es), [vicente.pesudo@ciemat.es](mailto:vicente.pesudo@ciemat.es), [roberto.santorelli@ciemat.es](mailto:roberto.santorelli@ciemat.es)

Date: January 2020

GitHub: https://github.com/UIN-CIEMAT/SaG4n

## TABLE OF CONTENTS

- [About](#About)
- [Directories](#Directories)
- [Installation](#Installation)
- [Data libraries](#Data-library)
- [Usage](#Usage)
- [Examples](#Examples)
- [How to reference](#How-to-reference)
- [License](#License)
- [Acknowledgements](#Acknowledgements)


## About

SaG4n is a Geant4 tool developed to calculate neutron production due to (alpha,xn) reactions in different materials. Once compiled, SaG4n takes from an input file written by the user all the information necessary to define the geometry of the problem, the source, parameters of the physics, the type of output, etc... The program uses the electromagnetic models implemented in Geant4 to perform an explicit transport of the incident alpha particles through the geometry, and data libraries originally written in ENDF-6 format to model the (alpha,xn) reactions.

The SaG4n code, together with different (alpha,xn) data libraries can be downloaded from the present webpage. Documentation concerning how the code works and how to use it is provided as well.

## Directories

- `CMakeLists.txt`: needed to compie SaG4n
- `SaG4n.cc`: SaG4n source code
- `include`: SaG4n source code
- `src`: SaG4n source code
- `PhysicsListFiles`: SaG4n source code (modified G4ParticleHP)
- `inputs/examples`: Some SaG4n input examples
- `outputs/examples_reference`: reference outputs of the input examples
- `docs`: SaG4n manual


## Installation

SaG4n is a Geant4 application, so it can be compiled in the same way as any other Geant4 application.

Geant4 has to be previously installed in the computer.

The following commands can be used to install SaG4n (replace `/home/you/geant4-install/...` with the appropriate path):
```sh
mkdir build
cd build/
source /home/you/geant4-install/share/Geant4-G4VERSION/geant4make/geant4make.sh
cmake -DGeant4_DIR=/home/you/geant4-install/lib64/Geant4-G4VERSION   ../
make
cd ../
```

If the compilation succeed, SaG4n executable has to be: `./build/SaG4n`


## Data libraries

SaG4n uses libraries originally written in ENDF-6 data format to model the (alpha,xn) reactions. They are needed to run SaG4n and are not included together with the code.

The following libraries are available for download:

- JENDL/AN-2005. It contains only (alpha,n) cross sections and secondary neutron energy-angular distribution data tables for the following isotopes: Li-6, Li-7, Be-9, B-10, B-11, C-12, C-13, N-14, N-15, O-17, O-18, F-19, Na-23, Al-27, Si-28, Si-29 and Si-30. This library relies on experimental data and theoretical calculations. This library can be downloaded from: https://cernbox.cern.ch/s/mD4gLPW5pRnHQWI

- TENDL-2017. It contains cross sections and secondary neutron energy-angular distribution data tables for a large variety of isotopes. This library relies on theoretical calculations performed with the TALYS code. This is the version of the TENDL-2017 library where all the non-elastic reactions are grouped together into a single channel. This library can be downloaded from: https://cernbox.cern.ch/s/oZXgc2TlBSEnObo

- JENDL/AN-2005_noSec01. This is the same as JENDL/AN-2005, but the data tables concerning the energy-angular distributions of the neutrons from (alpha,n) reactions (MT=4) on Li-6, Li-7, B-10, B-11, C-13, N-14, N-15, O-17 and O-18 have been removed. These energy-angular distributions are then computed by Geant4 from the information of the excited state of the residual nucleus (information provided by the library). An isotropic neutron angular distribution in the center-of-mass system is assumed, and the energy of the emitted neutrons is obtained from two-body kinematics. This library can be downloaded from: https://cernbox.cern.ch/s/4g1JQ7Ru9YE9QbE

- JENDLTENDL01 (recommended). This is the TENDL-2017 library, in which the 17 isotopes of JENDL/AN-2005 have been replaced with those of the JENDL/AN-2005_noSec01 library. This library can be downloaded from: https://cernbox.cern.ch/s/JBBzfqj4RVFjxL7


The users have to first download the library they want to use, uncompress it in any folder, and set the G4PARTICLEHPDATA environmental variable to the full path where is placed the (alpha,xn) data library. For example:

```sh
tar -xzvf JENDLTENDL01.tar.gz
export G4PARTICLEHPDATA=/[..]/JENDLTENDL01
```


## Usage

In order to execute SaG4n an input file must follow the name of the executable:

```sh
./build/SaG4n [SomeInput.inp]
``` 

Some input examples can be found in the `inputs/examples` directory.

However, some environmental variables should be defined before executing SaG4n. These are:

```sh
source /home/you/geant4-install/share/Geant4-G4VERSION/geant4make/geant4make.sh
export G4NEUTRONHP_DO_NOT_ADJUST_FINAL_STATE=1
export G4PHP_DO_NOT_ADJUST_FINAL_STATE=1
export G4NEUTRONHP_SKIP_MISSING_ISOTOPES=1
export G4PARTICLEHPDATA=/pathtoyourdatalibraries/ENDFLIBING4FORMAT
``` 

Where ENDFLIBING4FORMAT must be one of the (alpha,xn) cross section data libraries (TENDL, JENDL ...) available for download (see previous section).



## Examples


Some input examples are distributed together with the code. They are located inside the inputs/examples directory. Outputs generated with SaG4n using these inputs can be found in `outputs/examples_reference/`. They have been generated using Geant4.10.6.0 and the JENDLTENDL01 data library.

The `outputs/examples` directory does not content any output file. The idea is to place there the output files generated by SaG4n, and compare then with the reference output files.

SaG4n has to be executed in this directory in order to get the outputs in the right place. Otherwise the OUTPUTFILE field should be changed in the inputs.


### yields

`inputs/examples/yields/`: there are several inputs to calculate the neutron yields and energy spectra originated by the Th-232 series in secular equilibrium in different materials. The resulting neutron yields should be similar to those presented in Table 3 of https://arxiv.org/abs/1906.03903

To run each of these inputs, provided that the executable is located in `../build/SaG4n`:

```sh
./build/SaG4n inputs/examples/yields/yield01.inp
./build/SaG4n inputs/examples/yields/yield02.inp
./build/SaG4n inputs/examples/yields/yield03.inp
...
./build/SaG4n inputs/examples/yields/yield15.inp
```

Note: to save computing time, STEPMAX in these input examples has been set to 0.001 cm. For more accurate results (those reported in Table 3), STEPMAX has to be changed to 0.00001 cm.


### geometry

`inputs/examples/geometry/`: there are several inputs showing how to define geometries. More explanations are given in the manual.

These inputs are run in interactive mode:

```sh
./build/SaG4n inputs/examples/geometry/geom01.inp
```

And then:
```sh
/control/execute inputs/examples/geometry/vis01.mac
```

Or (maybe a better option) insert one by one in the terminal the commands from `inputs/examples/geometry/vis01.mac`

The same applies to geom02.inp and geom03.inp:

```sh
./build/SaG4n inputs/examples/geometry/geom02.inp
/control/execute inputs/examples/geometry/vis02.mac
```

```sh
./build/SaG4n inputs/examples/geometry/geom03.inp
/control/execute inputs/examples/geometry/vis03.mac
```

### components

`inputs/examples/components/`: simple geometries dealing with interfaces. See the SaG4 manual for more details.

Again, to run them:

```sh
./build/SaG4n inputs/examples/components/pcb_Th232.inp
./build/SaG4n inputs/examples/components/cable_Th232.inp
./build/SaG4n inputs/examples/components/cable_U238_mid.inp
./build/SaG4n inputs/examples/components/cable_Po210.inp
```

### decay series

Additionally, sources corresponding to the Th-232, U-235 and U-238 decay series in secular equilibrium are located in `inputs/examples/sources/`


## How to reference

The user can reference SaG4n with the following publication:

E. Mendoza et al., Neutron production induced by alpha-decay with Geant4, Nucl. Instrum. Methods A 960, 163659 (2020) https://doi.org/10.1016/j.nima.2020.163659.


## License

See the LICENSE file for license rights and limitations (GNU GPLv3).


## Acknowledgements

This work was supported in part by the Spanish Plan Nacional de I+D+i de Física de Partículas (Projects No. PGC2018-096717-B-C21 and FPA2016-76765-P) and the European Commission Seventh Framework Programme project CHANDA (No. FP7-605203).

