set(phonon_fake_PART_SRCS
   abstractaudiooutput.cpp
   audiodataoutput.cpp
   delayaudioeffect.cpp
   effect.cpp
   audiooutput.cpp
   mediaobject.cpp
   stream.cpp
   videodataoutput.cpp
   visualization.cpp
   backend.cpp
   volumefadereffect.cpp
   videowidget.cpp
   )

automoc4_add_library(phonon_fake SHARED ${phonon_fake_PART_SRCS})
set_target_properties(phonon_fake PROPERTIES DEFINE_SYMBOL MAKE_PHONON_FAKE_LIB)
target_link_libraries(phonon_fake ${PHONON_LIBS} phononexperimental)
