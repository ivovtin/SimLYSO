# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(SimLYSO)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
#list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
##option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" OFF)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif(
)

##set(QT_USE_IMPORTED_TARGETS ON)
##find_package(Qt5 REQUIRED)

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
#find_package(ROOT REQUIRED COMPONENTS RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread)
#find_package(ROOT)
#include_directories(/home/ovtin/root/)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO)

include(${ROOT_USE_FILE})

include_directories(${ROOT_INCLUDE_DIRS})

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${Geant4_INCLUDE_DIR})
                    ##${ROOTSYS})
EXEC_PROGRAM(root-config ARGS --incdir OUTPUT_VARIABLE rootIncDir)
include_directories(${rootIncDir})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#set(ROOT_LIBRARIES -L${ROOT_LIBRARY_DIR} -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic)
set(ROOT_LIBRARIES -L${ROOT_LIBRARY_DIR} -lCore -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
EXEC_PROGRAM(root-config ARGS --glibs OUTPUT_VARIABLE rootLibs)
add_executable(SimLYSO SimLYSO.cpp ${sources} ${headers})
#target_link_libraries(SimLYSO ${Geant4_LIBRARIES} ${rootLibs})
target_link_libraries(SimLYSO ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B1. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(SimLYSO_SCRIPTS
  input.mac
  visualization.mac
  )

# new data (in OPTICS directory)
set(OPTICS_SHARE
	Bialkali_QE.csv
	Bialkali_Reflectance.csv
	EASIROC.txt
	EASIROC_LTSPICE.txt
	EASIROC_LTSPICE_15nS.txt
	KETEK_QE15.txt
	KETEK_QE32.txt
	S13360-QE.csv
	si.txt
	SPIROC.txt
        SiO2-refraction.txt
	csi_rindex.txt
	csi_spectrum.txt
	LYSO_rindex.txt
	LYSO_Spectrum.csv
        lyso_abslength.txt
	Meltmount1582_AbsLength_cm.txt
	Meltmount1582_Refraction.txt
        al_reflectivity.txt
  )


#----------------------------------------------------------------------------
# Copy files

foreach(_script ${OPTICS_SHARE})
  configure_file(
    ${PROJECT_SOURCE_DIR}/OPTICS/${_script}
    ${PROJECT_BINARY_DIR}/OPTICS/${_script}
    COPYONLY
    )
endforeach()


foreach(_script ${SimLYSO_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target( DEPENDS SimLYSO)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS SimLYSO DESTINATION bin)


