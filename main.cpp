#include "draw_robots.cpp"

void hello_c() { // note: step through this app with a debugger
    { // fixed size arrays
        // additional reading: https://stackoverflow.com/questions/35442414/dynamic-vs-static-array-in-c

        int a[5];                     // uninitialized (filled with garbage)
        int b[5] = {};                // zero initialized (filled with zeros)
        int c[5] = { 0, 1, 2, 3, 4 }; // fully initialized
        int d[5] = { 0, 1, 2, };      // padded with zeros
        int e[]  = { 0, 1, 2, };      // fully initialized (size implied)

        for (int i = 0; i < NITEMS(a); ++i) {
            a[i] = i;
            b[i] = 2 * i;
        }

    }

    { // static variables

        // static variables are "locally persistent"
        for (int i = 0; i < 5; ++i) {
            static int j = 7;
            ++j;
        }

        // statics (and globals) are zero-initialized by default
        static int a[16];
        static double b; 
        a[0] = 42;
        b = 3.14;

        for (int i = 0; i < 5; ++i) {
            static bool initialized;
            if (!initialized) {
                initialized = true;
                printf("this let's you do things just once inside a loop\n");
            }
        }

    }
}
void hello_linalg() {
    {
        real d = 3.14;
        d = CLAMP(d, 0, 1); // macros that work on real's: CLAMP, LERP, AVG, MIN, MAX, MID, ABS, etc.
    }

    {
        vec2 a = { 1, 2 };           // a vec2 is just a plain old data struct

        // a.x; a[0]; a.data[0];        // some equivalent ways of accessing a.data
        // a.y; a[1]; a.data[1];

        vec2 b = a + 2 * V2(5, 6);   // V2 is returns a vec2 (kinda sorta a constructor) -- let's you create a vec2 anywhere
        vec2 c = V2(1, 2);

        c = cwiseProduct(a, c); // some functions w/ same names as methods from Eigen: cwiseProduct, norm, squaredNorm, etc.

        mat4 F = Translation(1, 2) * Rotation(RAD(30)) * Scaling(2, 4); // some methods for building 4x4 transform matrices
        vec2 e = transformPoint(F, c);  // transform point -- (homogeneous coordinate w = 1)
        vec2 f = transformVector(F, c); // transform point vector -- (w = 0) -- discards translation 
    }

    {
        vec3 a = V3(1, 2, 3);

        // a.x; a.r; a[0]; a.data[0];
        // a.y; a.g; a[1]; a.data[1];
        // a.z; a.b; a[2]; a.data[2];

        mat4 F = Translation(1, 2, 3) * RotationX(RAD(45));
        a = transformPoint(F, a); // same function can transform a vec3
    }
}



void hello_triangle() {
    // // this is an app that uses app.cpp (my particular way of doing an app)
    // // it's just a function (with a big ol' while loop) that we call from main

    // features
    // - compiles quickly with a single call to the compiler of your choice
    // - draws stuff reasonbaly fast (not actually fast, just not ludicrously slow)
    // - draws stuff whenever/wherever you want (i.e we have _not_ given up control over when/where we draw())
    // - supplies some basic interactivity (respond to key presses, move camera with the mouse, drag points around, etc.)

    // dependencies / anti-features
    // - it uses some silly globals and rather horrifying macros for convenience/brevity (CAMERA_2D, KEY_PRESSED, MOUSE_POSITION, etc.)
    // - it uses my verrrry basic math library (linalg.cpp)
    // - it uses glfw for OS calls (opening a window, keyboard input, etc.) and for setting up an OpenGL context

    // bottom line: an equivalent app.cpp could be written by you in a few days or weeks (or even a few hours if you used a math library such as glm)
    //              there is nothing special about any of it
    //              there is nothing hard about any of it
    //              and if you don't want to use glfw and/or OpenGL, you could e.g. call the Windows API directly and render on the CPU (see Handmade Hero)

    // // my main goal in this class is to open your eyes a bit to what's possible
    // // and have you start to question some of the things you've heard/been taught

    vec2 points[] = { { 0, 0 }, { 1, 0 }, { 0, 1 } }; // the points we'll draw
    vec3 colors[] = { RED, GREEN, BLUE };             // the colors we'll draw them in

    while (!KEY_PRESSED['Q'] && !glfwWindowShouldClose(window)) { // loop until Q pressed or user closes window
        // glfwSwapInterval(0);   // uncomment this line to run at an uncapped frame rate (otherwise default is 60fps)
        POLL_INPUT();             // grab mouse and keyboard input and store in globals
        CAMERA_2D(PV, 2.5); // macro for a basic interactive 2D camera
        // note: creates PV = PerspectiveMatrix * ViewMatrix using initial screen height 2.5
        FULLSCREEN_TOGGLE('F');   // press this key to toggle fullscreen
        CLEAR_DRAW_BUFFER(BLACK); // clear the draw buffer to black
        // BEGIN_FRAME(PV, 2.5, 'F', BLACK); // convenience macro that does the above

        if (!KEY_TOGGLE[GLFW_KEY_SPACE]) { // press spacebar to toggle drawing on and off
            DRAW_(2, TRIANGLES, 3, points, colors); // draw 2D triangles using these 3 points in these colors
            DRAW_(2, LINE_LOOP, 3, points, PURPLE); // draw 2D triangles using these 3 points in these colors
            DRAG_2D_POINTS_(PV, 3, points, YELLOW); // drag these three points (and annotate in yellow)
        }

        SWAP_DRAW_BUFFERS();      // swap the buffers (display what we just drew) -- END_FRAME() is an alias for this
    }
}
void hello_triangle_more_as_i_would_actually_write_it() {
    vec2 points[] = { { 0, 0 }, { 1, 0 }, { 0, 1 } };
    vec3 colors[] = { RED, GREEN, BLUE };
    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 2.5, 'F', BLACK);
        if (!KEY_TOGGLE[GLFW_KEY_SPACE]) {
            DRAW_(2, TRIANGLES, 3, points, colors);
            DRAW_(2, LINE_LOOP, 3, points, PURPLE);
            DRAG_2D_POINTS_(PV, 3, points, YELLOW);
        }
        END_FRAME();
    }
}



void hello_balls() {
    const int NUM_BALLS = 20; // alternative: #define NUM_BALLS 20

    vec2 *s = (vec2 *) calloc(NUM_BALLS, sizeof(vec2));
    vec2 *v = (vec2 *) calloc(NUM_BALLS, sizeof(vec2));

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 2.5, 'F', BLACK);

        static bool initialized;
        if (!initialized || KEY_PRESSED['R']) {
            initialized = true;
            for_(i, NUM_BALLS) {
                s[i] = {};
                for_(d, 2) {
                    v[i][d] = .01 * RAND(-1, 1);
                }
            }
        }

        if (!KEY_TOGGLE['P'] || KEY_PRESSED['.']) {
            for_(i, NUM_BALLS) {
                s[i] += v[i];
                for_(d, 2) {
                    if (ABS(s[i][d]) > 1) {
                        s[i][d] = SGN(s[i][d]) * (1 - (ABS(s[i][d]) - 1));
                        v[i][d] *= -1;
                    }
                }
            }
        }

        KELLY_DRAW_(2, POINTS, NUM_BALLS, s, 4);
        real tmp[] = { -1, -1, 1, -1, 1, 1, -1, 1 };
        DRAW_(2, LINE_LOOP, NITEMS(tmp) / 2, tmp, WHITE, 1);

        END_FRAME();
    }
}
#define DIM 3 // 2 for 2D app; 3 for 3D app
#if DIM == 2
#define vecX vec2
#else
#define vecX vec3
#endif
void hello_balls_plus_plus() {
    const int NUM_BALLS = 10000; // alternative: #define NUM_BALLS 4096

    vecX *s = (vecX *) calloc(NUM_BALLS, sizeof(vecX));
    vecX *v = (vecX *) calloc(NUM_BALLS, sizeof(vecX));

    while (!KEY_PRESSED['Q']) {
        CLEAR_DRAW_BUFFER(BLACK);
        POLL_INPUT();
        #if DIM == 2
        CAMERA_2D(PV, 3);
        #else
        CAMERA_3D(PV, 3, false, 'C');
        #endif

        static bool initialized;
        if (!initialized || KEY_PRESSED['R']) {
            initialized = true;
            for_(i, NUM_BALLS) {
                s[i] = {};
                for_(d, DIM) {
                    v[i][d] = .01 * RAND(-1, 1);
                }
            }
        }

        if (!KEY_TOGGLE['P'] || KEY_PRESSED['.']) {
            for_(i, NUM_BALLS) {
                s[i] += v[i];
                for_(d, DIM) {
                    if (ABS(s[i][d]) > 1) {
                        s[i][d] = SGN(s[i][d]) * (1 - (ABS(s[i][d]) - 1));
                        v[i][d] *= -1;
                    }
                }
            }
        }

        vec3 *colors = (vec3 *) calloc(NUM_BALLS, sizeof(vec3));
        for_(i, NUM_BALLS) {
            for_(d, DIM) {
                colors[i][d] = .1 + ABS(v[i][d]) * 100;
            }
        }
        DRAW_(DIM, POINTS, NUM_BALLS, s, colors, 2);
        free(colors);

        #if DIM == 2
        real tmp[] = { -1, -1, 1, -1, 1, 1, -1, 1 };
        DRAW_(DIM, LINE_LOOP, NITEMS(tmp) / 2, tmp, WHITE, 1);
        #else
        real tmp[] = { -1,-1,-1,-1,1,-1,-1,1,-1,1,1,-1,1,1,-1,1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,1,-1,1,1,-1,1,1,1,1,1,1,1,1,1,-1,1,1,-1,1,-1,-1,1,-1,-1,-1,-1,-1,1,-1,1,-1,-1,1,1,1,1,-1,1,1,1,1,-1,-1,1,-1,1 };
        DRAW_(DIM, LINES, NITEMS(tmp) / 3, tmp, WHITE, 1);
        #endif

        END_FRAME();
    }
}
#undef DIM
#undef vecX



void hello_pencil() {
    // note: this pattern is a bit silly, but is verrrry convenient in practice
    vec2 **line_strips = 0; // ~ std::vector<std::vector<vec2>>

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 3, 'F', BLACK);

        if (KEY_PRESSED['R']) {
            for_(i, arrlen(line_strips)) arrfree(line_strips[i]);
            arrfree(line_strips);
        }

        if (MOUSE_LEFT_PRESSED) {
            arrput(line_strips, (vec2 *) 0);
        }

        if (MOUSE_LEFT_HELD) {
            arrput(line_strips[arrlen(line_strips) - 1], MOUSE_POSITION);
        }

        for_(i, arrlen(line_strips)) {
            DRAW_(2, LINE_STRIP, arrlen(line_strips[i]), line_strips[i], monokai(i));
        }

        END_FRAME();
    }
}
void hello_pencil_another_way() {
    // store all the strokes contiguously
    vec2 *vertices = 0;
    int *vertex_count = 0;

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 3, 'F', BLACK);

        if (KEY_PRESSED['R']) {
            arrfree(vertices);
            arrfree(vertex_count);
        }

        if (MOUSE_LEFT_PRESSED) {
            arrput(vertex_count, 0);
        }

        if (MOUSE_LEFT_HELD) {
            arrput(vertices, MOUSE_POSITION);
            ++(vertex_count[arrlen(vertex_count) - 1]);
        }

        int offset = 0;
        for_(i, arrlen(vertex_count)) {
            DRAW_(2, LINE_STRIP, vertex_count[i], vertices + offset, monokai(i));
            offset += vertex_count[i];
        }

        END_FRAME();
    }
}
#if 0
#include <vector>
void hello_pencil_with_std_vector() {
    // note: i don't personally use std vector anymore, but it is very std
    //       many people think it's great, so please feel free to use it in this class*
    //       *just understand that there really isn't anything particularly exciting about a dynamic array / stretchy buffer:
    #if 0
    template <typename T> struct NaiveDynamicArray {
        T *data;
        int capacity;
        int count;
        T &operator [](int i) {
            return data[i];
        }
        void push_back(T element) {
            if (count == capacity) {
                data = (T *) realloc(data, 2 * capacity * sizeof(T));
            }
            data[count++] = element;
        }
        void clear() {
            free(data);
            capacity = count = 0;
        }
    };
    #endif
    // things to consider (in my opinion):
    // - stuff gets real hard to look at in a debugger
    // - compile errors get real hard to understand
    // - i no longer understand what's going on with the actual memory
    // -- (something something references, move semantics, constructors, destructors, etc.)
    //                         ^ ---------- i basically never use this stuff ----------- ^
    //     std::vector     stb_ds arr
    // -------------------------------
    //      foo.data() <-> foo
    // foo.push_back() <-> arrput(foo)
    //      foo.size() <-> arrlen(foo)

    std::vector<std::vector<vec2>> line_strips;

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 3, 'F', BLACK);

        if (KEY_PRESSED['R']) {
            for_(i, line_strips.size()) 
                line_strips.clear();
        }

        if (MOUSE_LEFT_PRESSED) {
            line_strips.push_back(std::vector<vec2>());
        }

        if (MOUSE_LEFT_HELD) {
            line_strips[line_strips.size() - 1].push_back(MOUSE_POSITION);
        }

        for_(i, line_strips.size()) {
            DRAW_(2, LINE_STRIP, (int) line_strips[i].size(), line_strips[i].data(), monokai(i));
        }

        END_FRAME();
    }
}
#endif



void app_draw() {
    int primitives[] = { POINTS, LINES, TRIANGLES, QUADS, LINE_STRIP, LINE_LOOP, TRIANGLE_FAN }; // note: in modern graphics people basically just use triangles (a line segment is e.g. two skinny triangles) but don't worry about that for now

    int num_points = 12; // note: what happens if you make this bigger than 12? why?
    vec2 *points = (vec2 *) malloc(num_points * sizeof(vec2));
    vec3 *colors = (vec3 *) malloc(num_points * sizeof(vec3));

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 2.5, 'F', BLACK);

        static bool initialized;
        if (!initialized || KEY_PRESSED['R']) {
            initialized = true;
            for (int i = 0; i < num_points; ++i) {
                real theta = double(i) / num_points * 6.18; // note: what happens if you don't cast i to double? why?
                points[i] = { cos(theta), sin(theta) };     // todo: #define PI 3.14159265358979323846264338f
                //       #define TWO_PI (2 * PI)
                colors[i] = AVG(monokai(i / 2), monokai((i + 1) / 2)); // monokai(i) just looks up in { RED, ORANGE, ..., PURPLE }
            }
        }

        static int i;
        i = MODULO(i + (KEY_PRESSED['K'] ? 1 : KEY_PRESSED['J'] ? -1 : 0), NITEMS(primitives));
        // note: this ^ is a fairly silly one-liner (but quite convenient for copying and pasting)
        // more standard (ew) would be e.g.
        //
        //   if (KEY_PRESSED['K']) {
        //       ++i;
        //       if (i > NITEMS(primitives) - 1) {
        //           i = 0;
        //       }
        //   } else if (KEY_PRESSED['J']) {
        //       --i;
        //       if (i < 0) {
        //           i = NITEMS(primitives) - 1;
        //       }
        //   }

        DRAW_(2, primitives[i], num_points, points, colors);
        DRAG_2D_POINTS_(PV, num_points, points, WHITE);

        END_FRAME();
    }
}
void app_draw_plus_plus() {
    vec3 rainbow[] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE };
    int primitives[] = { POINTS, LINES, TRIANGLES, QUADS, LINE_STRIP, LINE_LOOP, TRIANGLE_FAN };

    int num_points = 96;

    vec2 *points = (vec2 *) malloc(num_points * sizeof(vec2));
    vec3 *colors = (vec3 *) malloc(num_points * sizeof(vec3));
    for (int i = 0; i < num_points; ++i) {
        real x = double(i) / num_points;
        points[i] = { x, 0 };
        colors[i] = rainbow[i % NITEMS(rainbow)];
    }

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 2, 'F', BLACK);

        for (int i = 0; i < num_points; ++i) {
            // note: here we extract polar coordinates (r, theta) from our array of data points
            //       another approach would be to store r, theta in arrays as data
            double r = norm(points[i]);
            double theta = atan2(points[i].y, points[i].x);
            theta += .0003 * i;
            points[i] = r * V2(cos(theta), sin(theta));
        }

        static int i;
        i = MODULO(i + (KEY_PRESSED['K'] ? 1 : KEY_PRESSED['J'] ? -1 : 0), NITEMS(primitives));

        DRAW_(2, primitives[i], num_points, points, colors);

        END_FRAME();
    }
}


int main() {
    // hello_c();
    // hello_linalg();

    // hello_triangle();
    // hello_triangle_more_as_i_would_actually_write_it();

    // hello_balls();
    // hello_balls_plus_plus();

    // hello_pencil();
    // hello_pencil_another_way();
    // hello_pencil_with_std_vector();

    // app_draw();
    app_draw_plus_plus();

    return 0;
}

