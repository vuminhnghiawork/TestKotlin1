#include <GLES3/gl31.h>
#include <math.h>

#if ANDROID_LOG
#include <android/log.h>
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, "RENDERER", __VA_ARGS__);
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, "RENDERER", __VA_ARGS__);
#else
#include <stdio.h>
#define ALOGI(...) { printf(__VA_ARGS__); printf("\r\n"); }
#define ALOGE(...) { printf(__VA_ARGS__); printf("\r\n"); }
#endif

GLuint rectangleProgram;
GLuint lineProgram;
GLuint vbo[2];
float lineOffset = 0.0f;

// Rectangle shaders
const char* rectangleVertexShaderSource = R"(
    #version 310 es
    layout(location = 0) in vec4 aPosition;
    void main() {
        gl_Position = aPosition;
    }
)";

const char* rectangleFragmentShaderSource = R"(
    #version 310 es
    precision mediump float;
    out vec4 fragColor;
    void main() {
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
)";

// Line shaders
const char* lineVertexShaderSource = R"(
    #version 310 es
    layout(location = 0) in vec4 aPosition;
    uniform float uOffset;
    out vec2 vPosition;
    void main() {
        gl_Position = aPosition + vec4(uOffset, 0.0, 0.0, 0.0);
        vPosition = gl_Position.xy;
    }
)";

const char* lineFragmentShaderSource = R"(
    #version 310 es
    precision mediump float;
    in vec2 vPosition;
    out vec4 fragColor;
    void main() {
        float alpha = 1.0 - (vPosition.x + 1.0) / 2.0;
        fragColor = vec4(1.0, 1.0, 1.0, alpha);
    }
)";

GLfloat rectangleVertices[] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f,  0.5f,
        -0.5f,  0.5f
};

GLfloat lineVertices[] = {
        -1.0f,  0.0f,
        1.0f,  0.0f
};

GLuint loadShader(GLenum type, const char* shaderSource) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(shader, 512, NULL, buffer);
        ALOGE("Shader compilation failed: %s", buffer);
    }

    return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        char buffer[512];
        glGetProgramInfoLog(program, 512, NULL, buffer);
        ALOGE("Program linking failed: %s", buffer);
    }

    return program;
}

void init_gl(int width, int height) {
    ALOGI("init_gl %d x %d", width, height);

    rectangleProgram = createProgram(rectangleVertexShaderSource, rectangleFragmentShaderSource);
    lineProgram = createProgram(lineVertexShaderSource, lineFragmentShaderSource);

    glGenBuffers(2, vbo);

    // Rectangle
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    // Line
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
}

void resize_gl(int new_width, int new_height) {
    ALOGI("resize_gl %d x %d", new_width, new_height);
    glViewport(0, 0, new_width, new_height);
}

void render_gl(int width, int height) {
    ALOGI("render_gl %d x %d", width, height);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw rectangle
    glUseProgram(rectangleProgram);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Update line offset for animation
    lineOffset += 0.01f;
    if (lineOffset > 2.0f) {
        lineOffset -= 2.0f;
    }

    // Draw moving line
    glUseProgram(lineProgram);
    GLuint offsetLocation = glGetUniformLocation(lineProgram, "uOffset");
    glUniform1f(offsetLocation, lineOffset - 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_LINES, 0, 2);

    glFinish();
}

void deinit_gl(int width, int height) {
    ALOGI("deinit_gl %d x %d", width, height);
    glDeleteProgram(rectangleProgram);
    glDeleteProgram(lineProgram);
    glDeleteBuffers(2, vbo);
}