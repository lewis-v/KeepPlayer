//
// Created by lewis-v on 2020/11/1.
//

#include <map>
#include <string>
#include "RGBGLRender.h"

#include "../base/Macros.h"
#include "../base/ImageParseUtil.h"

NS_KP_BEGIN

    static GLfloat _vertices[] = {-1.f, 1.f,
                                  -1.f, -1.f,
                                  1.f, 1.f,
                                  1.f, -1.f};

    static const GLfloat texCoords[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};

    RGBGLRender::RGBGLRender() {
        if (!initProgram()) {
            return;
        }
        if (!parseVertexAttribs()) {
            return;
        }
        glGenTextures(1, &texture);
        GLUtil::checkGLError("glGenTextures");
        glBindTexture(GL_TEXTURE_2D, texture);
        GLUtil::checkGLError("glBindTexture 1");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GLUtil::checkGLError("glTexParameteri");
    }

    RGBGLRender::~RGBGLRender() {
        glDeleteProgram(_program);
    }

    void RGBGLRender::render(AVFrame *frame, int width, int height) {
        glUseProgram(_program);

        //设置定点缓存指针
        glVertexAttribPointer(static_cast<GLuint>(vertAttributePositionLocation), 2, GL_FLOAT,
                              GL_FALSE, 0, _vertices);
        glEnableVertexAttribArray(static_cast<GLuint>(vertAttributePositionLocation));
        //纹理坐标对应
        //设置纹理缓存指针，varying变量会被插值传入片元着色器
        glVertexAttribPointer(static_cast<GLuint>(vertAttributeTexCoordLocation), 2, GL_FLOAT,
                              GL_FALSE, 0, texCoords);
        glEnableVertexAttribArray(static_cast<GLuint>(vertAttributeTexCoordLocation));

        logI("width:%D  height:%d", width, height);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(texture));
        GLUtil::checkGLError("rgb glBindTexture");
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGB,
                     frame->width,
                     frame->height,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     frame->data[0]);

        GLUtil::checkGLError("rgb glTexImage2D");
        glUniform1i(rgbTextureLocation, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        GLUtil::checkGLError("rgb draw");
    }


    bool RGBGLRender::initProgram() {
        GLuint vertShader = 0;
        GLuint fragShader = 0;
//初始化普通的画图片的定点着色器和片元着色器
        const char *vert = R"(

precision mediump float;

attribute vec2 a_Position;
attribute vec2 a_TextureCoordinates;

varying vec2 v_TextureCoordinates;

void main()
{
    gl_Position = vec4(a_Position, 0,1);
    v_TextureCoordinates = a_TextureCoordinates;
}

    )";
        if (!compileShader(vertShader, GL_VERTEX_SHADER, vert)) {
            logE("RenderTexture: ERROR: Failed to compile vertex shader");
            return false;
        }
        logI("vertData finish");
        const char *frag = R"(
precision mediump float;

uniform sampler2D tex_rgb;

varying vec2 v_TextureCoordinates;

void main()
{
    vec4 rgba = texture2D(tex_rgb, v_TextureCoordinates);
    gl_FragColor = vec4(rgba.rgb, 1);
}

    )";
        if (!compileShader(fragShader, GL_FRAGMENT_SHADER, frag)) {
            logE("RenderTexture: ERROR: Failed to compile fragment shader");
            glDeleteShader(vertShader);
            return false;
        }
        logI("fragData finish");
        // create program
        _program = glCreateProgram();
        if (!_program) {
            glDeleteShader(vertShader);
            glDeleteShader(fragShader);
            return false;
        }
        //将shader关联到程序中
        glAttachShader(_program, vertShader);
        GLUtil::checkGLError("glAttachShader v");
        glAttachShader(_program, fragShader);
        GLUtil::checkGLError("glAttachShader f");
        glLinkProgram(_program);
        GLUtil::checkGLError("linkProgram");
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);

        GLint status = 0;
        glGetProgramiv(_program, GL_LINK_STATUS, &status);
        if (GL_FALSE == status) {
            logE("RenderTexture: ERROR: %s: failed to link program status:%d", __FUNCTION__,
                 status);
            glDeleteProgram(_program);
            _program = 0;
            return false;
        }

        //这里会获取程序中参数对应的id，之后参数传递需要通过这些id来传递
        if (!parseVertexAttribs()) {
            logE("parseVertexAttribs: ERROR: %s", __FUNCTION__);
            return false;
        }
        return true;
    }

    bool RGBGLRender::parseVertexAttribs() {
        vertAttributePositionLocation = glGetAttribLocation(_program, "a_Position");
        if (-1 == vertAttributePositionLocation) {
            logE("RGBRender: %s: can not find vertex attribute of a_Position",
                 __FUNCTION__);
            return false;
        }

        vertAttributeTexCoordLocation = glGetAttribLocation(_program,
                                                            "a_TextureCoordinates");
        if (-1 == vertAttributeTexCoordLocation) {
            logE("RGBRender: %s %d: can not find vertex uniform of a_TextureCoordinates",
                 __FUNCTION__, __LINE__);
            return false;
        }

        rgbTextureLocation = glGetUniformLocation(_program, "tex_rgb");
        if (-1 == rgbTextureLocation) {
            logE("RGBRender: %s %d: can not find vertex uniform of tex_rgb",
                 __FUNCTION__, __LINE__);
            return false;
        }

        return true;
    }

NS_KP_END
