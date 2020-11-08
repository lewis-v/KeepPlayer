//
// Created by lewis-v on 2020/10/18.
//

#ifndef PLAYER_BASEGLRENDER_H
#define PLAYER_BASEGLRENDER_H


#include "../base/Macros.h"
#include "../base/GLUtil.h"
#include "IRender.h"
#include <string>

NS_KP_BEGIN
    class BaseGLRender : public IRender {
    protected:
        bool compileShader(GLuint &shader, GLenum type, const GLchar *source) const;

        std::string logForOpenGLShader(GLuint shader) const;
    };
NS_KP_END


#endif //PLAYER_BASEGLRENDER_H
