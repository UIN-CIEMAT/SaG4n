#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.16...3.27)
project(SaG4nproject)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${PROJECT_SOURCE_DIR}/PhysicsListFiles/G4SaG4nParticleHP_include
                    ${Geant4_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc
                  ${PROJECT_SOURCE_DIR}/PhysicsListFiles/G4SaG4nParticleHP_src/*.cc)

file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh
                   ${PROJECT_SOURCE_DIR}/PhysicsListFiles/G4SaG4nParticleHP_include/*.hh)



#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#

add_executable(SaG4n SaG4n.cc ${sources} ${headers})
target_link_libraries(SaG4n ${Geant4_LIBRARIES} )


#----------------------------------------------------------------------------
# Add program to the project targets
# (this avoids the need of typing the program name after make)
#
add_custom_target(SaG4nproject DEPENDS PHP03)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#install(TARGETS SaG4n DESTINATION bin)

