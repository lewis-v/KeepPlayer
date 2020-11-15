//
// Created by lewis-v on 2020/10/18.
//

#ifndef PLAYER_YUVGLRENDER_H
#define PLAYER_YUVGLRENDER_H


#include "../base/Macros.h"
#include "BaseGLRender.h"

NS_KP_BEGIN
    class YUVGLRender : public BaseGLRender {
    public:
        YUVGLRender();

        ~YUVGLRender();

        virtual void render(AVFrame *frame, int width, int height) override;

    private:
//        GLfloat _vertices[8] = {
//                0.f, 0.5f,//左上
//                0.0f, -0.5f,//左下
//                -0.5f, 0.0f,//右上
//                0.5f, -0.5f//右下
//        };
        GLuint _program = 0;
        GLint vertAttributePositionLocation = -1;
        GLint vertAttributeTexCoordLocation = -1;
        GLint yTextureLocation = -1;
        GLint uTextureLocation = -1;
        GLint vTextureLocation = -1;
        GLuint yTexture = 0;
        GLuint uTexture = 0;
        GLuint vTexture = 0;
        GLuint ebo;
        GLuint vao;
        GLuint vbo;

        bool initProgram();

        bool parseVertexAttribs();

        void initTexture(GLuint *texture);
    };
NS_KP_END


#endif //PLAYER_YUVGLRENDER_H
