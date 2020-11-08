//
// Created by lewis-v on 2020/10/18.
//

#include "BaseGLRender.h"

#include "../base/Macros.h"

NS_KP_BEGIN
    bool BaseGLRender::compileShader(GLuint &shader, GLenum type, const GLchar *source) const {
        logI("load shader %s", source);
        shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint status = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (!status) {
            logE("RenderTexture: BaseRender ERROR status %d: Failed to compile shader:\n%s", status, source);
            logE("RenderTexture: %s", logForOpenGLShader(shader).c_str());

            return false;
        }

        return (GL_TRUE == status);
    }

    std::string BaseGLRender::logForOpenGLShader(GLuint shader) const {
        GLint logLength = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength < 1)
            return "";

        char *logBytes = (char *) malloc(sizeof(char) * logLength);
        glGetShaderInfoLog(shader, logLength, nullptr, logBytes);
        std::string ret(logBytes);

        free(logBytes);
        return ret;
    }

NS_KP_END

