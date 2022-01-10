#ifndef APP_CPP
#define APP_CPP

////////////////////////////////////////////////////////////////////////////////
// include                                                                    //
////////////////////////////////////////////////////////////////////////////////

#include "poe.cpp"
#include "linalg.cpp"

#ifdef _WIN64 // Windows: glad and glfw
#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include "windows_ext/glfw/include/GLFW/glfw3.h"
#include "windows_ext/glfw/include/GLFW/glfw3native.h"
#include "windows_ext/glad/include/KHR/khrplatform.h"
#include "windows_ext/glad/include/glad/glad.h"
#include "windows_ext/glad/src/glad.c"
#else        // Mac: just glfw
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#endif

////////////////////////////////////////////////////////////////////////////////
// window                                                                     //
////////////////////////////////////////////////////////////////////////////////

static GLFWwindow *window;
vec2 get_window_dimensions() {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    return { real(w), real(h) };
}
real get_window_aspect() {
    int width, height; {
        glfwGetFramebufferSize(window, &width, &height);
    }
    return real(width) / height;
}
real get_window_height() {
    int width, height; {
        glfwGetFramebufferSize(window, &width, &height);
    }
    return height;
}
void framebuffer_size_callback(GLFWwindow *, int width, int height) {
    glViewport(0, 0, width, height);
}
void CLEAR_DRAW_BUFFER(vec3 CLEAR_COLOR = { 0, 0, 0 }, real CLEAR_ALPHA = 1) {
    glClearColor((float) CLEAR_COLOR.r, (float) CLEAR_COLOR.g, (float) CLEAR_COLOR.b, float(CLEAR_ALPHA));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void SWAP_DRAW_BUFFERS() {
    glfwSwapBuffers(window);
}
#define FULLSCREEN_TOGGLE(character) do {         \
    bool fullscreen = KEY_TOGGLE[character];      \
    int fullscreen_fac = fullscreen ? 1 : 2;      \
    int width = 1920 / fullscreen_fac;            \
    int height = 1080 / fullscreen_fac;           \
    int _width, _height;                          \
    glfwGetWindowSize(window, &_width, &_height); \
    if (width != _width || height != height) {    \
        glfwSetWindowSize(window, width, height); \
        CLEAR_DRAW_BUFFER();                      \
    }                                             \
} while (0)

BEGIN_PREMAIN_BLOCK {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    #ifndef _WIN64
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1024, 512, "", NULL, NULL);
    glfwSetWindowPos(window, 0, 30);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowTitle(window, "poe");
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    #ifdef _WIN64
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    #endif

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(4);
    glPointSize(8);

    glfwSwapInterval(1);
    CLEAR_DRAW_BUFFER();
} END_PREMAIN_BLOCK

////////////////////////////////////////////////////////////////////////////////
// camera (and transforms)                                                    //
////////////////////////////////////////////////////////////////////////////////

mat4 BasicOrthographicProjection(real H) {
    real W = get_window_aspect() * H;
    mat4 tmp = {}; {

        // H                       
        // |                       
        // |                       
        // y'----------y object    
        // |           |           
        // |           |           
        // --------------> minus_z 

        tmp(0, 0) = 1;
        tmp(1, 1) = 1;
        tmp(2, 2) = -1e-3; // z sorting
        tmp(3, 3) = 1;
    }
    return Scaling(1 / W, 1 / H) * tmp;
}
mat4 BasicPerspectiveProjection(real cameraDistance, real theta) {
    mat4 tmp = {};

    //                H              
    //               -|              
    //              - |              
    //             -  |           y  
    //            -   |       -   |  
    //           -    |   -       |  
    //          -     y'          |  
    //         -) -   |           |  
    //        ---------------------->
    //        eye     z'         -z  

    // similar triangles: y' / z' = y / -z   
    //                      => y' = y z' / -z

    //   [   0      0  0] [x]   [     ]    [       ]
    //   [0  1      0  0] [y] = [    y] -> [-y z'/z]
    //   [0  0      0   ] [z]   [     ]    [       ]
    //   [0  0  -1/z'  0] [w] = [-z/z']             

    real H = cameraDistance * tan(theta * PI / 180);
    real W = get_window_aspect() * H;

    tmp(0, 0) = 1;
    tmp(1, 1) = 1;
    tmp(2, 3) = -1e-4; // z sorting
    tmp(3, 2) = -tan(theta * PI / 180) / H;

    return Scaling(1 / W, 1 /H) * tmp;
}
mat4 GetViewMatrix(vec3 WorldCameraPosition) {
    vec3 WorldCameraTarget = {};
    vec3 Y = { 0, 1, 0 };
    vec3 minus_z = normalized(WorldCameraTarget - WorldCameraPosition);
    vec3 x = normalized(cross(minus_z, Y));
    vec3 y = cross(x, minus_z);
    ASSERT(IS_EQUAL(norm(y), 1));
    mat4 C = assembleCoordinateSystem(x, y, -minus_z, WorldCameraPosition);
    return inverse(C);
}
mat4 getPV_Ortho2D(vec2 s_camera_world, real screen_height_in_world_units) { // note PV == NDC_from_World
    mat4 P = BasicOrthographicProjection(screen_height_in_world_units);
    mat4 V = Translation(-s_camera_world.x, -s_camera_world.y);
    return P * V;
}
mat4 getPV_Ortho3D(vec3 WorldCameraPosition, real screen_height_in_world_units) {
    mat4 V = GetViewMatrix(WorldCameraPosition);
    mat4 P = BasicOrthographicProjection(screen_height_in_world_units);
    return P * V;
}
mat4 getPV_Perspective(vec3 WorldCameraPosition, real theta) {
    mat4 V = GetViewMatrix(WorldCameraPosition);
    mat4 P = BasicPerspectiveProjection(norm(WorldCameraPosition), theta);
    return P * V;
}
mat4 get_NDC_from_Screen() {
    vec2 dims = get_window_dimensions();
    return Scaling(2 / dims.x, -2 / dims.y, 1) * Translation(-dims.x / 2, -dims.y / 2, 0);
}
#define CAMERA_2D(PV, ScreenHeight)                                                                                                       \
    mat4 PV = {}; {                                                                                                                       \
        static real ScreenHalfHeightInWorldCoordinates = ScreenHeight / 2.;                                                               \
        static vec2 CameraPositionInWorldCoordinates;                                                                                     \
        PV = getPV_Ortho2D(CameraPositionInWorldCoordinates, ScreenHalfHeightInWorldCoordinates);                                         \
        SET_DRAW_TRANSFORM_AND_BAKE_MOUSE(PV);                                                                                            \
        if (MOUSE_RIGHT_HELD || (KEY_HELD[GLFW_KEY_SPACE] && MOUSE_LEFT_HELD)) CameraPositionInWorldCoordinates -= MOUSE_POSITION_OFFSET; \
        elif (NON_ZERO(MOUSE_WHEEL_OFFSET)) {                                                                                             \
            ScreenHalfHeightInWorldCoordinates -= real(ScreenHeight) / 100 * MOUSE_WHEEL_OFFSET;                                          \
            ScreenHalfHeightInWorldCoordinates = MAX(ScreenHalfHeightInWorldCoordinates, .001);                                           \
            vec2 s_mouse_NDC = transformPoint(PV, MOUSE_POSITION);                                                                        \
            mat4 P_prime = BasicOrthographicProjection(ScreenHalfHeightInWorldCoordinates);                                               \
            vec2 RHS = transformPoint(inverse(P_prime), s_mouse_NDC);                                                                     \
            CameraPositionInWorldCoordinates = MOUSE_POSITION - RHS;                                                                      \
        }                                                                                                                                 \
        PV = getPV_Ortho2D(CameraPositionInWorldCoordinates, ScreenHalfHeightInWorldCoordinates);                                         \
        SET_DRAW_TRANSFORM_AND_BAKE_MOUSE(PV);                                                                                            \
    }


#define CAMERA_3D(PV, ScreenHeight, mouseHandled, C)                                                                                           \
    mat4 PV = {}; {                                                                                                                            \
        static real H_camera = ScreenHeight / 2.;                                                                                              \
        static real theta_camera = 25;                                                                                                         \
        static real euler_X_camera, euler_Y_camera;                                                                                            \
        if (!mouseHandled && NON_ZERO(MOUSE_WHEEL_OFFSET)) {                                                                                   \
            H_camera -= MOUSE_WHEEL_OFFSET / 20;                                                                                               \
            H_camera = MAX(H_camera, .0001);                                                                                                   \
        }                                                                                                                                      \
        if (!mouseHandled && MOUSE_LEFT_HELD) {                                                                                                \
            vec2 tmp = _MOUSE_POSITION_OFFSET_Screen / 100;                                                                                    \
            euler_Y_camera -= tmp.x;                                                                                                           \
            euler_X_camera -= tmp.y;                                                                                                           \
        }                                                                                                                                      \
        real CameraDistanceFromOrigin = H_camera / tan(RAD(theta_camera));                                                                     \
        vec3 CameraPosition = transformPoint(RotationY(euler_Y_camera) * RotationX(euler_X_camera), vec3({ 0, 0, CameraDistanceFromOrigin })); \
        PV = (!KEY_TOGGLE[C]) ? getPV_Perspective(CameraPosition, theta_camera) : getPV_Ortho3D(CameraPosition, H_camera);                     \
        SET_DRAW_TRANSFORM(PV);                                                                                        \
    }

////////////////////////////////////////////////////////////////////////////////
// keys and mouse                                                             //
////////////////////////////////////////////////////////////////////////////////

#define NUM_KEYS 512
bool ANY_KEY_PRESSED;
bool KEY_PRESSED[NUM_KEYS];
bool KEY_RELEASED[NUM_KEYS];
bool KEY_HELD[NUM_KEYS];
bool KEY_TOGGLE[NUM_KEYS];
void key_callback(GLFWwindow*, int key, int, int action, int) {
    if (key < 0) { return; }
    if (action == GLFW_PRESS) {
        ANY_KEY_PRESSED = true;
        KEY_PRESSED[key] = true;
        KEY_HELD[key] = true;
        KEY_TOGGLE[key] = !KEY_TOGGLE[key];
    } elif (action == GLFW_RELEASE) {
        KEY_RELEASED[key] = true;
        KEY_HELD[key] = false;
    }
}
vec2 _MOUSE_POSITION_Screen;
vec2 _MOUSE_POSITION_NDC;
vec2 MOUSE_POSITION;
vec2 _MOUSE_POSITION_OFFSET_Screen;
vec2 MOUSE_POSITION_OFFSET;
static void cursor_position_callback(GLFWwindow *, double xpos, double ypos) {
    vec2 tmp = _MOUSE_POSITION_Screen;
    _MOUSE_POSITION_Screen = { xpos, ypos };
    _MOUSE_POSITION_OFFSET_Screen = _MOUSE_POSITION_Screen - tmp; 
    _MOUSE_POSITION_NDC = transformPoint(get_NDC_from_Screen(), _MOUSE_POSITION_Screen);
}
void BAKE_MOUSE(mat4 NDC_from_World) {
    mat4 World_from_NDC = inverse(NDC_from_World);
    mat4 World_from_Screen = World_from_NDC * get_NDC_from_Screen();
    MOUSE_POSITION = transformPoint(World_from_Screen, _MOUSE_POSITION_Screen);
    MOUSE_POSITION_OFFSET = transformVector(World_from_Screen, _MOUSE_POSITION_OFFSET_Screen);
}
bool MOUSE_LEFT_PRESSED;
bool MOUSE_LEFT_RELEASED;
bool MOUSE_RIGHT_PRESSED;
bool MOUSE_PRESS_HANDLED;
bool MOUSE_LEFT_HELD;
bool MOUSE_RIGHT_HELD;
real MOUSE_WHEEL_OFFSET;
void mouse_button_callback(GLFWwindow *, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) { 
            MOUSE_LEFT_PRESSED = true;
            MOUSE_LEFT_HELD = true;
        } elif (action == GLFW_RELEASE) { 
            MOUSE_LEFT_RELEASED = true;
            MOUSE_LEFT_HELD = false;
        }
    } elif (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) { 
            MOUSE_RIGHT_PRESSED = true;
            MOUSE_RIGHT_HELD = true;
        } elif (action == GLFW_RELEASE) { 
            MOUSE_RIGHT_HELD = false;
        }
    }
}
void scroll_callback(GLFWwindow *, double, double yoffset) {
    MOUSE_WHEEL_OFFSET = yoffset;
}
void POLL_INPUT() {
    ANY_KEY_PRESSED = false;
    for_(i, NUM_KEYS) { KEY_PRESSED[i] = KEY_RELEASED[i] = false; }
    MOUSE_LEFT_PRESSED = false;
    MOUSE_LEFT_RELEASED = false;
    MOUSE_RIGHT_PRESSED = false;
    _MOUSE_POSITION_OFFSET_Screen = {};
    MOUSE_WHEEL_OFFSET = 0;
    MOUSE_PRESS_HANDLED = false; // fornow
    glfwPollEvents();
}

BEGIN_PREMAIN_BLOCK {
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
} END_PREMAIN_BLOCK

////////////////////////////////////////////////////////////////////////////////
// DRAW_                                                                      //
////////////////////////////////////////////////////////////////////////////////

GLint COMPILE_SHADER(const char **source, GLenum type) {
    int shader = glCreateShader(type);
    glShaderSource(shader, 1, source, NULL);
    glCompileShader(shader);
    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        { // log
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            printf("%s", infoLog);
        }
        ASSERT(0);
    }
    return shader;
};
GLint BUILD_SHADER_PROGRAM(int vertexShader, int fragmentShader) {
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    int success = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        { // log
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            printf("%s", infoLog);
        }
        ASSERT(0);
    }
    return shaderProgram;
};
GLint BUILD_SHADER_PROGRAM(const char *vertexShaderSource, const char *fragmentShaderSource) {
    return BUILD_SHADER_PROGRAM( \
            COMPILE_SHADER(&vertexShaderSource, GL_VERTEX_SHADER), \
            COMPILE_SHADER(&fragmentShaderSource, GL_FRAGMENT_SHADER));
}
int getCurrentShaderProgram() {
    GLint shaderProgram; {
        glGetIntegerv(GL_CURRENT_PROGRAM, &shaderProgram);
    }
    return shaderProgram;
}
void glUniform_mat4(const char *name, mat4 A) {
    float A_float[16]; {
        for_(i, 16) A_float[i] = float(A.data[i]);
    }
    glUniformMatrix4fv(glGetUniformLocation(getCurrentShaderProgram(), name), 1, GL_TRUE, A_float);
}
#define SET_DRAW_TRANSFORM(A) glUniform_mat4("transform", A)
void SET_DRAW_TRANSFORM_AND_BAKE_MOUSE(mat4 PV) {
    SET_DRAW_TRANSFORM(PV);
    BAKE_MOUSE(PV);
}
GLuint basicVAO, basicVBO[2];
BEGIN_PREMAIN_BLOCK {
    glGenVertexArrays(1, &basicVAO);
    glGenBuffers(2, basicVBO);
} END_PREMAIN_BLOCK
const char *basicVertexShaderSource = R""""(
    #version 330 core
    layout (location = 0) in vec3 vertex;
    layout (location = 1) in vec3 color;
    out vec3 io_color;
    uniform mat4 transform;
    void main() {
        io_color = color;
        gl_Position = transform * vec4(vertex, 1);
    }
)"""";
const char *basicFragmentShaderSource = R""""(
    #version 330 core
    uniform vec4 color;
    uniform float fillPattern;
    in vec3 io_color;
    out vec4 FragColor;
    void main() {
        FragColor = color;
        if (FragColor.a < .0001) {
            FragColor = vec4(io_color, 1);
        }
    }
)"""";
int basicShaderProgram = BUILD_SHADER_PROGRAM(basicVertexShaderSource, basicFragmentShaderSource);
void BASIC_DRAWING_API_ENTRY_POINT_() {
    glUseProgram(basicShaderProgram);
    glBindVertexArray(basicVAO);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, basicVBO[0]);
}
void glColor(vec3 color, real CLEAR_ALPHA) {
    BASIC_DRAWING_API_ENTRY_POINT_();
    ASSERT(IN_RANGE(CLEAR_ALPHA, 0, 1));
    float tmp[4] = { (float) color.r, (float) color.g, (float) color.b, (float) CLEAR_ALPHA };
    glUniform4fv(glGetUniformLocation(basicShaderProgram, "color"), 1, tmp); // FORNOW
}
void glColor(vec3 color) { glColor(color, 1); } // sugar
#define _HACK_GL_QUADS 254
void DRAW_(int numDimensions, int PRIMITIVE, int numVertices, void *vertices, void *colors = 0, real size = 0) {
    ASSERT(numDimensions == 1 || numDimensions == 2 || numDimensions == 3);
    if (!numVertices) return;

    int GL_PRIMITIVE = PRIMITIVE;

    if (GL_PRIMITIVE == GL_LINES) ASSERT(numVertices % 2 == 0);
    if (GL_PRIMITIVE == GL_TRIANGLES) ASSERT(numVertices % 3 == 0);
    if (GL_PRIMITIVE == _HACK_GL_QUADS) ASSERT(numVertices % 4 == 0);

    if (PRIMITIVE == _HACK_GL_QUADS) {
        GLint polygonMode;
        glGetIntegerv(GL_POLYGON_MODE, &polygonMode);

        if (polygonMode == GL_FILL) {
            GL_PRIMITIVE = GL_TRIANGLES;
            int numQuads = numVertices / 4;
            numVertices = 6 * numQuads;
            void *_vertices = vertices;
            void *_colors = colors;
            vertices = calloc(numVertices, numDimensions * sizeof(real));
            if (_colors) colors = calloc(numVertices, sizeof(vec3));
            int k = 0;
            for_(quad_i, numQuads) {
                int index[] = { 0, 1, 2, 0, 2, 3 };
                for_(ii, NITEMS(index)) { 
                    int j = 4 * quad_i + index[ii];
                    memcpy(((real *) vertices) + numDimensions * k, ((real *) _vertices) + numDimensions * j, numDimensions * sizeof(real));
                    if (_colors) ((vec3 *) colors)[k] = ((vec3 *) _colors)[j];
                    ++k;
                }
            }
        } else if (polygonMode == GL_LINE) {
            GL_PRIMITIVE = GL_LINES;
            int numQuads = numVertices / 4;
            int numLines = 4 * numQuads;
            numVertices = 2 * numLines;
            void *_vertices = vertices;
            void *_colors = colors;
            vertices = calloc(numVertices, numDimensions * sizeof(real));
            if (_colors) colors = calloc(numVertices, sizeof(vec3));
            int k = 0;
            for_(quad_i, numQuads) {
                int index[] = { 0, 1, 1, 2, 2, 3, 3, 0 };
                for_(ii, NITEMS(index)) { 
                    int j = 4 * quad_i + index[ii];
                    memcpy(((real *) vertices) + numDimensions * k, ((real *) _vertices) + numDimensions * j, numDimensions * sizeof(real));
                    if (_colors) ((vec3 *) colors)[k] = ((vec3 *) _colors)[j];
                    ++k;
                }
            }
        }
    }

    BASIC_DRAWING_API_ENTRY_POINT_();
    glBufferData(GL_ARRAY_BUFFER, numVertices * numDimensions * sizeof(real), vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, numDimensions, GL_DOUBLE, 0, 0, 0);
    if (colors) {
        glColor({}, 0);
        glBindBuffer(GL_ARRAY_BUFFER, basicVBO[1]);
        glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vec3), colors, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_DOUBLE, 0, 0, 0);
        glEnableVertexAttribArray(1); 
    } else {
        glDisableVertexAttribArray(1); 
    }

    if (NON_ZERO(size)) {
        if (GL_PRIMITIVE == GL_POINTS) {
            glPointSize(GLfloat(size));
        } else {
            glLineWidth(GLfloat(size));
        }
    }
    glDrawArrays(GL_PRIMITIVE, 0, numVertices);

    if (PRIMITIVE == _HACK_GL_QUADS) {
        free(vertices);
        free(colors);
    }
}
void DRAW_(int numDimensions, int GL_PRIMITIVE, int numVertices, void *vertices, vec3 color, real size = 0) {
    glColor(color);
    DRAW_(numDimensions, GL_PRIMITIVE, numVertices, vertices, 0, size);
}

////////////////////////////////////////////////////////////////////////////////
// colors                                                                     //
////////////////////////////////////////////////////////////////////////////////

vec3 monokaiRed = { 249./255,  38./255, 114./ 255 };
vec3 monokaiOrange = { 253./255, 151./255,  31./ 255 };
vec3 monokaiYellow = { 255./255, 255./255,  50./ 255 };
vec3 monokaiGreen  = { 166./255, 226./255,  46./ 255 };
vec3 monokaiBlue   = { 102./255, 217./255, 239./ 255 };
vec3 monokaiPurple = { 174./255, 129./255, 255./ 255 };
vec3 monokaiWhite  = { 255./255, 255./255, 255./ 255 };
vec3 monokaiGray   = { 127./255, 127./255, 127./ 255 };
vec3 monokaiBlack  = {   0./255,   0./255,   0./ 255 };
vec3 kelly_[] = { { 255./255, 179./255,   0./255 }, { 128./255,  62./255, 117./255 }, { 255./255, 104./255,   0./255 }, { 166./255, 189./255, 215./255 }, { 193./255,   0./255,  32./255 }, { 206./255, 162./255,  98./255 }, { 129./255, 112./255, 102./255 }, {   0./255, 125./255,  52./255 }, { 246./255, 118./255, 142./255 }, {   0./255,  83./255, 138./255 }, { 255./255, 122./255,  92./255 }, {  83./255,  55./255, 122./255 }, { 255./255, 142./255,   0./255 }, { 179./255,  40./255,  81./255 }, { 244./255, 200./255,   0./255 }, { 127./255,  24./255,  13./255 }, { 147./255, 170./255,   0./255 }, {  89./255,  51./255,  21./255 }, { 241./255,  58./255,  19./255 }, {  35./255,  44./255,  22./255 } };
vec3 kelly(int i) { return kelly_[i % NITEMS(kelly_)]; }
vec3 monokai_[] = { monokaiRed, monokaiOrange, monokaiYellow, monokaiGreen, monokaiBlue, monokaiPurple };
vec3 monokai(int i) { return monokai_[i % NITEMS(monokai_)]; }

void KELLY_DRAW_(int numDimensions, int GL_PRIMITIVE, int numVertices, void *vertices, real size = 0) {
    vec3 *colors = (vec3 *) malloc(numVertices * sizeof(vec3));
    for_(i, numVertices) {
        colors[i] = kelly(i);
    }
    DRAW_(numDimensions, GL_PRIMITIVE, numVertices, vertices, colors, size);
    free(colors);
}

#endif
