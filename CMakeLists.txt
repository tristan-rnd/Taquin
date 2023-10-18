cmake_minimum_required(VERSION 3.5)

project( UE3.4_Projet_TR )

set(SRCS
	Plateau.cpp
    )
    
set(HEADERS
    Plateau.h
    )


# Path for CMake to find dumpbin.exe
LIST(APPEND CMAKE_PROGRAM_PATH  "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.37.32822/bin/Hostx64/x64")

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

find_package(VTK REQUIRED)

if (NOT VTK_FOUND)
  message("Skipping ${PROJECT_NAME}: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")

if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${PROJECT_NAME}.cpp ${SRCS} ${HEADERS})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${VTK_LIBRARIES} ${ITK_LIBRARIES} )
else ()
  # include all components
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${PROJECT_NAME}.cpp ${SRCS} ${HEADERS})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${VTK_LIBRARIES} ${ITK_LIBRARIES})

  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS ${PROJECT_NAME}
    MODULES ${VTK_LIBRARIES}
    )
endif ()
