//  Plugin Defines removed from stdafx and placed here.
//

#ifndef GAMEPLUGINMODULE_H_INCLUDED
#define GAMEPLUGINMODULE_H_INCLUDED

#ifdef WIN32
  #ifdef SAMPLEPLUGIN_EXPORTS
    #define SAMPLEPLUGIN_IMPEXP __declspec(dllexport)
  #elif SAMPLEPLUGIN_IMPORTS
    #define SAMPLEPLUGIN_IMPEXP __declspec(dllimport)
  #else
    #define SAMPLEPLUGIN_IMPEXP __declspec()
  #endif

#elif defined (_VISION_IOS) || defined(_VISION_ANDROID)
  #define SAMPLEPLUGIN_IMPEXP

#else
  #error Undefined platform!
#endif

#endif //  GAMEPLUGINMODULE_H_INCLUDED
