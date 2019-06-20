#-----------------------------------------------------------------------------
# ITK
#-----------------------------------------------------------------------------

# Sanity checks
if(DEFINED ITK_DIR AND NOT EXISTS ${ITK_DIR})
  message(FATAL_ERROR "ITK_DIR variable is defined but corresponds to non-existing directory")
endif()

set(proj ITK)
set(proj_DEPENDENCIES GDCM VTK)

if(MITK_USE_OpenCV)
  list(APPEND proj_DEPENDENCIES OpenCV)
endif()

if(MITK_USE_HDF5)
  list(APPEND proj_DEPENDENCIES HDF5)
endif()

#if(MITK_USE_VTK)
#  list(APPEND proj_DEPENDENCIES VTK)
#endif()

#if(MITK_USE_DCMTK)
#  list(APPEND proj_DEPENDENCIES DCMTK)
#endif()

set(ITK_DEPENDS ${proj})

if(NOT DEFINED ITK_DIR)

  set(additional_cmake_args -DUSE_WRAP_ITK:BOOL=OFF)

  if(MITK_USE_OpenCV)
    list(APPEND additional_cmake_args
         -DModule_ITKVideoBridgeOpenCV:BOOL=ON
         -DOpenCV_DIR:PATH=${OpenCV_DIR}
        )
  endif()

  #if(MITK_USE_VTK)
  #  list(APPEND additional_cmake_args
  #       -DModule_ITKVtkGlue:BOOL=ON
  #       -DVTK_DIR:PATH=${VTK_DIR}
  #      )
  #endif()

  #if(MITK_USE_DCMTK)
  #  list(APPEND additional_cmake_args
  #      -DModule_ITKDCMTK:BOOL=ON
  #      -DModule_ITKIODCMTK:BOOL=ON
  #       -DDCMTK_USE_ICU:BOOL=OFF
  #       -DDCMTK_DIR:PATH=${DCMTK_DIR}
  #      )
  #endif()

  # Keep the behaviour of ITK 4.3 which by default turned on ITK Review
  # see MITK bug #17338
  list(APPEND additional_cmake_args
    -DModule_ITKReview:BOOL=ON
  # for 4.7, the OpenJPEG is needed by review but the variable must be set
    -DModule_ITKOpenJPEG:BOOL=ON
  )

  if(CTEST_USE_LAUNCHERS)
    list(APPEND additional_cmake_args
      "-DCMAKE_PROJECT_${proj}_INCLUDE:FILEPATH=${CMAKE_ROOT}/Modules/CTestUseLaunchers.cmake"
    )
  endif()

  ExternalProject_Add(${proj}
     LIST_SEPARATOR ${sep}
     URL ${MITK_THIRDPARTY_DOWNLOAD_PREFIX_URL}/InsightToolkit-4.13.1.tar.xz
     URL_MD5 bc7296e7faccdcb5656a7669d4d875d2
     CMAKE_GENERATOR ${gen}
     CMAKE_ARGS
       ${ep_common_args}
       ${additional_cmake_args}
       -DBUILD_EXAMPLES:BOOL=OFF
       -DITK_USE_SYSTEM_GDCM:BOOL=ON
       -DGDCM_DIR:PATH=${GDCM_DIR}
       -DITK_USE_SYSTEM_HDF5:BOOL=ON
       -DHDF5_DIR:PATH=${HDF5_DIR}
       -DModule_ITKReview:BOOL=ON
       #-DModule_LesionSizingToolkit:BOOL=ON
       -DModule_SkullStrip:BOOL=ON
       -DModule_TextureFeatures:BOOL=ON
       -DModule_RLEImage:BOOL=ON
       -DModule_IsotropicWavelets:BOOL=ON
       -DModule_PrincipalComponentsAnalysis:BOOL=ON
       -DModule_MGHIO:BOOL=ON
       -DVCL_INCLUDE_CXX_0X:BOOL=ON
       -DVCL_INCLUDE_CXX_0X:BOOL=ON
     CMAKE_CACHE_ARGS
       ${ep_common_cache_args}
     CMAKE_CACHE_DEFAULT_ARGS
       ${ep_common_cache_default_args}
     DEPENDS ${proj_DEPENDENCIES}
    )

  set(ITK_DIR ${ep_prefix})
  mitkFunctionInstallExternalCMakeProject(${proj})

else()

  mitkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")

endif()
