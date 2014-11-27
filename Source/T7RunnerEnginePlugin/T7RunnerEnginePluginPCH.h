#if !defined(GAMEPLUGINPCH_H_INCLUDED)
#define GAMEPLUGINPCH_H_INCLUDED

#if defined(WIN32)
  #define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif

#include <Vision/Runtime/Engine/System/Vision.hpp>

extern VModule g_myComponentModule;

#endif
