//
// Created by lewis-v on 2020/10/18.
//

#ifndef PLAYER_GLUTIL_H
#define PLAYER_GLUTIL_H

#include <string>
//todo other platform GL header def
#ifdef ANDROID

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include "Log.h"

#endif
NS_KP_BEGIN
    class GLUtil {
    public:
        static bool checkGLError(std::string tag) {
            int err = glGetError();
            if (err != GL_NO_ERROR) {
                logE("tag %s gl err %d\n", tag.c_str(), err);
                return false;
            }
            return true;
        }
    };
NS_KP_END

#endif //PLAYER_GLUTIL_H
