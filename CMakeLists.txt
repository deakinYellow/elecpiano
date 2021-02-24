cmake_minimum_required(VERSION 3.0.2)

project(elecpiano)

add_compile_options(-std=c++11)

option( BUILD_FOR_IMX "cross compiling for imx6ull" OFF )
option( BUILD_QT "build the gui with QT" OFF )

if( BUILD_QT )
  ###Qt Environment
  find_package( Qt5 REQUIRED Core Widgets Multimedia )
  set(QT_LIBRARIES Qt5::Widgets Qt5::Multimedia)
  ##############################################################################
  # Qt Environment
  ##############################################################################
  # this comes from qt_build's qt-ros.cmake which is automatically
  # included via the dependency call in package.xml
  #rosbuild_prepare_qt4(QtCore QtGui) # Add the appropriate components to the component list here
  ##############################################################################
  # Sections
  ##############################################################################
  file(GLOB QT_FORMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/forms/*.ui)
  file(GLOB QT_RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} resources/*.qrc)
  file(GLOB_RECURSE QT_MOC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS include/*.hpp *.h)

  QT5_ADD_RESOURCES(QT_RESOURCES_CPP ${QT_RESOURCES} )
  QT5_WRAP_UI( QT_FORMS_HPP ${QT_FORMS})
  QT5_WRAP_CPP( QT_MOC_HPP ${QT_MOC})
  message( STATUS "====QT_FORMS_HPP: ${QT_FORMS_HPP}" )
  message( STATUS "====cmake build directory: ${CMAKE_BUILD_RPATH}" )
endif(BUILD_QT)

###Fluidsynth
if( BUILD_FOR_IMX )
  set( FLUIDSYNTH_LIB /opt/imx-yocto-core/sysroots/cortexa7hf-neon-poky-linux-gnueabi/usr/lib/libfluidsynth.so )
else( BUILD_FOR_IMX)
  set( FLUIDSYNTH_LIB /usr/local/lib64/libfluidsynth.so )
endif( BUILD_FOR_IMX )


include_directories(
  include
  )
LIST( APPEND COMMON_LINK_LIBRARIES
  ${FLUIDSYNTH_LIB}
  )

#==========console APP==========================

add_executable( ${PROJECT_NAME}
  "fluidsynth/example.cc"
  )
target_link_libraries( ${PROJECT_NAME} ${COMMON_LINK_LIBRARIES} )

add_executable( sequencer
  "fluidsynth/sequencer.cc"
  )
target_link_libraries( sequencer ${COMMON_LINK_LIBRARIES} )




#==========QT APP==========================
##############################################################################
# Sources
##############################################################################
#file(GLOB_RECURSE QT_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS src/*.cpp)
##############################################################################
# Binaries
##############################################################################
#add_executable(robot_hmi ${QT_SOURCES} ${QT_RESOURCES_CPP} ${QT_FORMS_HPP} ${QT_MOC_HPP})
#target_link_libraries(robot_hmi ${QT_LIBRARIES} ${catkin_LIBRARIES})
#install(TARGETS robot_hmi RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
#set( APP "app" )
#add_executable( ${APP}
#  ${QT_SOURCES}
#  ${QT_RESOURCES_CPP}
#  "src/battery.cpp"
#  "src/frmbattery.cpp"
#  "src/app.cc"
#  )
#target_link_libraries( ${APP} ${QT_LIBRARIES} ${COMMON_LINK_LIBRARIES} )



