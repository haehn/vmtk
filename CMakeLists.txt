PROJECT(VMTK)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# vmtk versions number
SET(VMTK_VERSION_MAJOR 0)
SET(VMTK_VERSION_MINOR 9)
SET(VMTK_VERSION_PATCH 0)
SET(VMTK_VERSION 
    "${VMTK_VERSION_MAJOR}.${VMTK_VERSION_MINOR}.${VMTK_VERSION_PATCH}")

# Append the library version information to the library target properties.
OPTION(VMTK_WITH_LIBRARY_VERSION "Build with library version information" OFF)
IF(VMTK_WITH_LIBRARY_VERSION)
  # This setting of SOVERSION assumes that any API change
  # will increment either the minor or major version number of vmtk.
  SET(VMTK_LIBRARY_PROPERTIES
      VERSION "${VMTK_VERSION}"
      SOVERSION "${VMTK_VERSION_MAJOR}.${VMTK_VERSION_MINOR}"
     )
ENDIF(VMTK_WITH_LIBRARY_VERSION)

#-----------------------------------------------------------------------------
# ITK Setup
option( USE_SYSTEM_ITK
  "Exclude ITK from SuperBuild and use an existing build instead." OFF )
if( USE_SYSTEM_ITK )
  find_package( ITK REQUIRED )
  include( ${ITK_USE_FILE} )
endif( USE_SYSTEM_ITK )

#-----------------------------------------------------------------------------
# VTK Setup
option( USE_SYSTEM_VTK
  "Exclude VTK from SuperBuild and use an existing build instead." OFF )
if( USE_SYSTEM_VTK )
  find_package( VTK REQUIRED )
  include( ${VTK_USE_FILE} )
endif( USE_SYSTEM_VTK )

#-----------------------------------------------------------------------------
# Superbuild setup
option( VMTK_USE_SUPERBUILD
  "Build VMTK and the projects it depends on via SuperBuild.cmake." ON )

if( VMTK_USE_SUPERBUILD )

  include( "${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake" )
  return()

else( VMTK_USE_SUPERBUILD )

  if ( NOT ITK_FOUND )
    find_package( ITK REQUIRED )
    include( ${ITK_USE_FILE} )
  endif( NOT ITK_FOUND )

  if ( NOT VTK_FOUND )
    find_package( VTK REQUIRED )
    include( ${VTK_USE_FILE} )
  endif( NOT VTK_FOUND )

endif( VMTK_USE_SUPERBUILD )

OPTION(VMTK_SCRIPTS_ENABLED "Build and install vmtkScripts and Pypes" ON)
MARK_AS_ADVANCED(VMTK_SCRIPTS_ENABLED)

IF(VMTK_SCRIPTS_ENABLED)
  SUBDIRS(
    PypeS
    vtkVmtk
    vmtkScripts
    )
ELSE(VMTK_SCRIPTS_ENABLED)
  SUBDIRS(
    vtkVmtk
    )
ENDIF(VMTK_SCRIPTS_ENABLED)

OPTION(VMTK_MINIMAL_INSTALL "Do not install individual scripts, only vmtk.py" OFF)

SET(LIBRARY_OUTPUT_PATH ${VMTK_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all libraries.")
SET(EXECUTABLE_OUTPUT_PATH ${VMTK_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

SET(VMTK_SRCS
   vmtk.py
   )

SET(VMTK_MODULE_SRCS
   __init__.py
   )

IF(NOT VMTK_INSTALL_BIN_DIR)
  #SET(VMTK_INSTALL_BIN_DIR ${VMTK_INSTALL_ROOT}/bin)
  SET(VMTK_INSTALL_BIN_DIR bin)
ENDIF(NOT VMTK_INSTALL_BIN_DIR)

IF(NOT VMTK_MODULE_INSTALL_LIB_DIR)
  #SET(VMTK_MODULE_INSTALL_LIB_DIR ${VMTK_SCRIPTS_INSTALL_ROOT}/lib/vmtk/vmtk)
  SET(VMTK_MODULE_INSTALL_LIB_DIR lib/vmtk/vmtk)
ENDIF(NOT VMTK_MODULE_INSTALL_LIB_DIR)

IF(VMTK_SCRIPTS_ENABLED)

  FOREACH (SCRIPT_FILE ${VMTK_SRCS} ${VMTK_MODULE_SRCS})
    CONFIGURE_FILE(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
  ENDFOREACH (SCRIPT_FILE)
  
  #INSTALL_FILES(${VMTK_MODULE_INSTALL_LIB_DIR} .py ${VMTK_MODULE_SRCS})
  INSTALL(FILES ${VMTK_MODULE_SRCS}
    DESTINATION ${VMTK_MODULE_INSTALL_LIB_DIR}
    COMPONENT RuntimeLibraries)
  
  IF (NOT WIN32)
  
  SET (STRIPPED_SCRIPTS_SRCS )
    
    FOREACH (SCRIPT_FILE  ${VMTK_SRCS})
      CONFIGURE_FILE(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
      STRING(REGEX REPLACE ".py" "" STRIPPED_SCRIPT_FILE ${SCRIPT_FILE})
      CONFIGURE_FILE(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE} COPYONLY)
      SET (STRIPPED_SCRIPTS_SRCS ${STRIPPED_SCRIPTS_SRCS} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
    ENDFOREACH (SCRIPT_FILE)
  
    #INSTALL_PROGRAMS(${VMTK_INSTALL_BIN_DIR} FILES ${STRIPPED_SCRIPTS_SRCS})
    INSTALL(PROGRAMS ${STRIPPED_SCRIPTS_SRCS}
      DESTINATION ${VMTK_INSTALL_BIN_DIR}
      COMPONENT RuntimeExecutables)
  
  ELSE (NOT WIN32)
  
    FOREACH (SCRIPT_FILE  ${VMTK_SRCS})
      CONFIGURE_FILE(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${SCRIPT_FILE} COPYONLY)
      STRING(REGEX REPLACE ".py" "-exe.py" STRIPPED_SCRIPT_FILE ${SCRIPT_FILE})
      CONFIGURE_FILE(${VMTK_SOURCE_DIR}/${SCRIPT_FILE} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE} COPYONLY)
      SET (STRIPPED_SCRIPTS_SRCS ${STRIPPED_SCRIPTS_SRCS} ${VMTK_BINARY_DIR}/${STRIPPED_SCRIPT_FILE})
    ENDFOREACH (SCRIPT_FILE)
  
    #INSTALL_PROGRAMS(${VMTK_INSTALL_BIN_DIR} FILES ${STRIPPED_SCRIPTS_SRCS})
    INSTALL(PROGRAMS ${STRIPPED_SCRIPTS_SRCS}
      DESTINATION ${VMTK_INSTALL_BIN_DIR}
      COMPONENT RuntimeExecutables)
  
  ENDIF (NOT WIN32)
  
  OPTION(VMTK_ENABLE_DISTRIBUTION "Enable distribution targets." OFF)
  IF(VMTK_ENABLE_DISTRIBUTION)
    ADD_SUBDIRECTORY(distribution)
  ENDIF(VMTK_ENABLE_DISTRIBUTION)
  
  # Export Build settings and library dependencies
  #  INCLUDE(CMakeExportBuildSettings)
  #  
  #  CMAKE_EXPORT_BUILD_SETTINGS(
  #    ${VMTK_BINARY_DIR}/VMTKBuildSettings.cmake
  #  )
  #  
  #  EXPORT_LIBRARY_DEPENDENCIES(
  #    ${VMTK_BINARY_DIR}/VMTKLibraryDepends.cmake
  #  )
  
    CONFIGURE_FILE(
      ${VMTK_SOURCE_DIR}/CMakeInput/VMTKConfig.cmake.in
      ${VMTK_BINARY_DIR}/VMTKConfig.cmake
    @ONLY IMMEDIATE
    )
    
    CONFIGURE_FILE(
      ${VMTK_SOURCE_DIR}/CMakeInput/VMTKUse.cmake.in
      ${VMTK_BINARY_DIR}/VMTKUse.cmake
    @ONLY IMMEDIATE
    )
 
ENDIF(VMTK_SCRIPTS_ENABLED)
