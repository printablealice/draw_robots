// #define ASSERT_WAITS_FOR_YOU_TO_PRESS_ENTER_BEFORE_CRASHING
// #define MAC_I_CANT_DRAG_THE_TRIANGLE

#include "draw_robots.cpp"

void hello_c() { // note: step through this app with a debugger
    { // fixed size arrays (stack allocated)
        // additional reading: https://stackoverflow.com/questions/35442414/dynamic-vs-static-array-in-c

        int a[5];                     // uninitialized (filled with garbage)
        int b[5] = {};                // zero initialized (filled with zeros)
        int c[5] = { 0, 1, 2, 3, 4 }; // fully initialized
        int d[5] = { 0, 1, 2, };      // padded with zeros
        int e[]  = { 0, 1, 2, };      // fully initialized (size implied)

        for (int i = 0; i < 5; ++i) {
            a[i] = i;
            b[i] = 2 * i;
        }
    }

    { // heap allocated arrays
        int N = 24;
        int *a = (int *) malloc(N * sizeof(int));
        int *b = (int *) calloc(N, sizeof(int));
        for (int i = 0; i < N; ++i) {
            a[i] = i;
            b[i] = 2 * i;
        }
    }

    { // pointers + pointer arithmetic
        char foo[] = { 'a', 'b', 'c', 'd', 'e' };

        char *tmp = foo;
        tmp += 2;
        char a = *tmp;

        char b = *(foo + 2);

        char c = foo[2];

        printf("%c %c %c\n", a, b, c); 

        for (char *ptr = foo; ptr < foo + 5; ++ptr) { // totally ok (but maybe a bit odd) to use a ptr as the variable in a for loop
            printf("%c ", *ptr); 
        }
        printf("\n");
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


    printf("press Enter to quit\n");
    getchar();
}

void hello_linalg() {
    {
        double d = 3.14;
        d = CLAMP(d, 0, 1); // macros that work on double's: CLAMP, LERP, AVG, MIN, MAX, MID, ABS, etc.
    }

    {
        vec2 a = { 1., 2. };           // a vec2 is just a plain old data struct

        // a.x; a[0]; a.data[0];     // some equivalent ways of accessing a.data
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
    // - draws stuff reasonably fast (not actually fast, just not ludicrously slow)
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

    // ASSERT(2 + 2 == 5); // use this to confirm ASSERT is behaving as you want it to

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
            DRAW_(2, LINE_LOOP, 3, points, PURPLE); // draw a 2D line loop using these 3 points in purple
            DRAG_2D_POINTS_(PV, 3, points, YELLOW); // drag these three points (and annotate in yellow)
        }

        SWAP_DRAW_BUFFERS();      // swap the buffers (display what we just drew) -- END_FRAME() is an alias for this
    }
}

void BASIC_DRAG_POINTS_2D(int num_points, vec2 *points) {
    static vec2 *dragPoint;

    for (int i = 0; i < num_points; ++i) {
        if (norm(points[i] - MOUSE_POSITION) < .1) {
            DRAW_(2, POINTS, 1, points + i, WHITE, 10);
            if (MOUSE_LEFT_PRESSED) {
                dragPoint = points + i;
            }
        }
    }

    if (dragPoint && MOUSE_LEFT_HELD) {
        *dragPoint = MOUSE_POSITION;
    }

    if (MOUSE_LEFT_RELEASED) {
        dragPoint = 0;
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


// balls apps we wrote in class
#define NUM_BALLS 5000
struct Ball {
    vec2 position;
    vec2 velocity;
    vec3 color;

    void draw() {
        DrawPoint2D(position, color);
    }

    void update() {
        position += .033 * velocity;
        if (ABS(position.x) > 1) {
            velocity.x *= -1;
        }
        if (ABS(position.y) > 1) {
            velocity.y *= -1;
        }
    }
};
void object_oriented_balls() {
    Ball *balls = (Ball *) calloc(NUM_BALLS, sizeof(Ball));
    for (int i = 0; i < NUM_BALLS; ++i) {
        balls[i].position = { RAND(-1, 1), RAND(-1, 1) };
        balls[i].velocity = { RAND(-1, 1), RAND(-1, 1) };
        balls[i].color = { RAND(0, 1), RAND(0, 1), RAND(0, 1) };
    }

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 2.5, 'F', AVG(RED, BLACK));

        for (int i = 0; i < NUM_BALLS; ++i) {
            balls[i].update();
            balls[i].draw();
        }

        END_FRAME();
    }
}
void data_oriented_balls() {
    vec2 *positions = (vec2 *) calloc(NUM_BALLS, sizeof(vec2));
    vec2 *velocities = (vec2 *) calloc(NUM_BALLS, sizeof(vec2));
    vec3 *colors = (vec3 *) calloc(NUM_BALLS, sizeof(vec3));
    for (int i = 0; i < NUM_BALLS; ++i) {
        positions[i] = { RAND(-1, 1), RAND(-1, 1) };
        velocities[i] = { RAND(-1, 1), RAND(-1, 1) };
        colors[i] = { RAND(0, 1), RAND(0, 1), RAND(0, 1) };
    }

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 2.5, 'F', AVG(GREEN, BLACK));

        // update
        for (int i = 0; i < NUM_BALLS; ++i) {
            positions[i] += .033 * velocities[i];
            if (ABS(positions[i].x) > 1) {
                velocities[i].x *= -1;
            }
            if (ABS(positions[i].y) > 1) {
                velocities[i].y *= -1;
            }
        }

        // draw
        DRAW_(2, POINTS, NUM_BALLS, positions, colors);

        END_FRAME();
    }
}

#define DIM 1
void XD_data_oriented_balls() {
    double *positions = (double *) calloc(NUM_BALLS * DIM, sizeof(double));
    double *velocities = (double *) calloc(NUM_BALLS * DIM, sizeof(double));
    double *colors = (double *) calloc(NUM_BALLS * 3, sizeof(double));
    for (int i = 0; i < NUM_BALLS; ++i) {
        for (int d = 0; d < DIM; ++d) {
            positions[DIM * i + d] = RAND(-1, 1);
            velocities[DIM * i + d] = RAND(-1, 1);
        }
        for (int d = 0; d < 3; ++d) {
            colors[DIM * i + d] = RAND(0, 1);
        }
    }
    while (!KEY_PRESSED['Q']) {
        POLL_INPUT();
        CAMERA_3D(PV, 2.5, false, 'C');
        FULLSCREEN_TOGGLE('F');
        CLEAR_DRAW_BUFFER(BLACK);
        for (int i = 0; i < NUM_BALLS; ++i) {
            for (int d = 0; d < DIM; ++d) {
                positions[DIM * i + d] += .01 * velocities[DIM * i + d];
                if (ABS(positions[DIM * i + d]) > 1) {
                    velocities[DIM * i + d] *= -1;
                }
            }
        }
        DRAW_(DIM, POINTS, NUM_BALLS, positions, colors, 2);
        END_FRAME();
    }
}
#undef DIM
#undef vecX

#undef NUM_BALLS





void hello_balls() {
    const int NUM_BALLS = 20; // alternative: #define NUM_BALLS 20

    vec2 *s = (vec2 *) calloc(NUM_BALLS, sizeof(vec2));
    vec2 *v = (vec2 *) calloc(NUM_BALLS, sizeof(vec2));

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 2.5, 'F', BLACK);

        static bool initialized;
        if (!initialized || KEY_PRESSED['R']) {
            initialized = true;
            for (int i = 0; i < NUM_BALLS; ++i) {
                s[i] = {};
                for (int d = 0; d < 2; ++d) {
                    v[i][d] = .01 * RAND(-1, 1);
                }
            }
        }

        if (!KEY_TOGGLE['P'] || KEY_PRESSED['.']) {
            for (int i = 0; i < NUM_BALLS; ++i) {
                s[i] += v[i];
                for (int d = 0; d < 2; ++d) {
                    if (ABS(s[i][d]) > 1) {
                        s[i][d] = SGN(s[i][d]) * (1 - (ABS(s[i][d]) - 1));
                        v[i][d] *= -1;
                    }
                }
            }
        }

        KELLY_DRAW_(2, POINTS, NUM_BALLS, s, 9);
        double tmp[] = { -1, -1, 1, -1, 1, 1, -1, 1 };
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
        FULLSCREEN_TOGGLE('F');

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
        double tmp[] = { -1, -1, 1, -1, 1, 1, -1, 1 };
        DRAW_(DIM, LINE_LOOP, NITEMS(tmp) / 2, tmp, WHITE, 1);
        #else
        double tmp[] = { -1,-1,-1,-1,1,-1,-1,1,-1,1,1,-1,1,1,-1,1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,1,-1,1,1,-1,1,1,1,1,1,1,1,1,1,-1,1,1,-1,1,-1,-1,1,-1,-1,-1,-1,-1,1,-1,1,-1,-1,1,1,1,1,-1,1,1,1,1,-1,-1,1,-1,1 };
        DRAW_(DIM, LINES, NITEMS(tmp) / 3, tmp, WHITE, 1);
        #endif

        END_FRAME();
    }
}
#undef vecX
#undef DIM





void hello_pencil() {
    // note: this pattern (dynamic array of dynamic arrays) is a bit silly (memory all over the place), but is very convenient in practice
    vec2 **line_strips = 0; // ~ std::vector<std::vector<vec2>>

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 3, 'F', BLACK);

        if (KEY_PRESSED['R']) {
            for (int i = 0; i < arrlen(line_strips); ++i) arrfree(line_strips[i]);
            arrfree(line_strips);
        }

        if (MOUSE_LEFT_PRESSED) {
            arrput(line_strips, (vec2 *) 0);
        }

        if (MOUSE_LEFT_HELD) {
            arrput(line_strips[arrlen(line_strips) - 1], MOUSE_POSITION);
        }

        for (int i = 0; i < arrlen(line_strips); ++i) {
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
        for (int i = 0; i < arrlen(vertex_count); ++i) {
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
    // - stuff gets double hard to look at in a debugger
    // - compile errors get double hard to understand
    // - i no longer understand what's going on with the actual memory
    // -- (something something references, move semantics, constructors, destructors, etc.)
    //                         ^ ---------- i try to avoid most of this stuff ---------- ^
    //     std::vector     stb_ds arr
    // -------------------------------
    //      foo.data() <-> foo
    // foo.push_back() <-> arrput(foo)
    //      foo.size() <-> arrlen(foo)

    std::vector<std::vector<vec2>> line_strips;

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 3, 'F', BLACK);

        if (KEY_PRESSED['R']) {
            for (int i = 0; i < line_strips.size(); ++i) {
                line_strips.clear();
            }
        }

        if (MOUSE_LEFT_PRESSED) {
            line_strips.push_back(std::vector<vec2>());
        }

        if (MOUSE_LEFT_HELD) {
            line_strips[line_strips.size() - 1].push_back(MOUSE_POSITION);
        }

        for (int i = 0; i < line_strips.size(); ++i) {
            DRAW_(2, LINE_STRIP, (int) line_strips[i].size(), line_strips[i].data(), monokai(i));
        }

        END_FRAME();
    }
}
#endif





void app_draw() {
    int primitives[] = { POINTS, LINES, TRIANGLES, QUADS, LINE_STRIP, LINE_LOOP, TRIANGLE_FAN }; // note: in modern graphics people basically just use triangles (a line segment is e.g. two skinny triangles) but don't worry about that for now

    int num_points = 5;
    vec2 *points = (vec2 *) malloc(num_points * sizeof(vec2));
    vec3 *colors = (vec3 *) malloc(num_points * sizeof(vec3));

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 2.5, 'F', BLACK);

        static bool initialized;
        if (!initialized || KEY_PRESSED['R']) {
            initialized = true;
            for (int i = 0; i < num_points; ++i) {
                double theta = double(i) / num_points * 6.18; // note: what happens if you don't cast i to double? why?
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
        double x = double(i) / num_points;
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



void app_fk_2d() {
    const int NUM_LINKS = 4;
    double L[] = { 1, 2, 3, 1 };
    double theta[] = { 0, 0, 0, 0 };
    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 15, 'F', BLACK);
        static vec2 drag_points[NUM_LINKS];
        KELLY_DRAW_(2, POINTS, NUM_LINKS, drag_points);
        DRAG_2D_POINTS_(PV, NUM_LINKS, drag_points, WHITE);
        for_(i, NUM_LINKS) {
            drag_points[i].x = -3 - i;
            theta[i] = drag_points[i].y;
        }
        vec2 joint_positions[NUM_LINKS + 1] = {};
        double cumm_angle = 0;
        for_(i, NUM_LINKS) {
            cumm_angle += theta[i];
            joint_positions[i + 1] = joint_positions[i] + L[i] * V2(cos(cumm_angle), sin(cumm_angle));
        }
        DRAW_(2, LINE_STRIP, NUM_LINKS + 1, joint_positions, GRAY);
        KELLY_DRAW_(2, POINTS, NUM_LINKS, joint_positions);
        END_FRAME();
    }
}
void app_fk_3d() {
    // a .1x1x.1 axis-aligned box with bottommost face centered at the origin
    vec3 link_vertices[]={{-.1,1,.1},{-.1,0,.1},{-.1,0,-.1},{-.1,1,-.1},{.1,1,.1},{.1,0,.1},{.1,0,-.1},{.1,1,-.1},{.1,0,.1},{-.1,0,.1},{-.1,0,-.1},{.1,0,-.1},{.1,1,.1},{-.1,1,.1},{-.1,1,-.1},{.1,1,-.1},{.1,1,-.1},{-.1,1,-.1},{-.1,0,-.1},{.1,0,-.1},{.1,1,.1},{-.1,1,.1},{-.1,0,.1},{.1,0,.1}};
    vec3 link_colors[] = {YELLOW,YELLOW,YELLOW,YELLOW,PURPLE,PURPLE,PURPLE,PURPLE,RED,RED,RED,RED,GREEN,GREEN,GREEN,GREEN,ORANGE,ORANGE,ORANGE,ORANGE,BLUE,BLUE,BLUE,BLUE};

    const int NUM_LINKS = 5;
    double L[]         = { 1.5, 1, 1, .5, .5 };
    int joint_axes[] = { 1, 2, 0, 2, 1 }; // 0 is a joint that rotates about x axis, 1 - y axis, 2 - z axis
    double theta[]     = { 0, 0, 0, 0, 0 };

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 10, 'F', BLACK);

        static vec2 drag_points[NUM_LINKS];
        KELLY_DRAW_(2, POINTS, NUM_LINKS, drag_points);
        bool __MOUSE_HANDLED = DRAG_2D_POINTS_(PV, NUM_LINKS, drag_points, WHITE);
        for_(i, NUM_LINKS) {
            drag_points[i].x = -3 - i;
            theta[i] = drag_points[i].y;
        }

        vec3 joint_positions[NUM_LINKS + 1] = {};
        mat4 joint_orientations[NUM_LINKS + 1] = {};
        for_(i, NUM_LINKS) {
            mat4 R = (joint_axes[i] == 0) ? RotationX(theta[i]) : (joint_axes[i] == 1) ? RotationY(theta[i]) : RotationZ(theta[i]);
            joint_orientations[i] = (i == 0) ? Identity<4>() : joint_orientations[i - 1];
            joint_orientations[i] = joint_orientations[i] * R;
            joint_positions[i + 1] = joint_positions[i] + transformVector(joint_orientations[i], V3(0, L[i], 0));
        }

        CAMERA_3D(PV_3D, 10, __MOUSE_HANDLED, 'C'); // note: left mouse drag rotates 3D camera iff not already handled (by DRAG_2D_POINTS_)

        { // draw robot
            if (KEY_TOGGLE[GLFW_KEY_TAB]) { // as skeleton
                DRAW_(3, LINE_STRIP, NUM_LINKS + 1, joint_positions, GRAY);
                KELLY_DRAW_(3, POINTS, NUM_LINKS, joint_positions);
            } else { // as boxes
                if (1) { // option 1: same vertices different camera
                    for_(i, NUM_LINKS) {
                        mat4 M = Translation(joint_positions[i]) * joint_orientations[i] * Scaling(1, L[i], 1);
                        SET_DRAW_TRANSFORM(PV_3D * M);
                        DRAW_(3, QUADS, NITEMS(link_vertices), link_vertices, link_colors);
                        glPolygonMode(GL_FRONT_AND_BACK,  GL_LINE);
                        DRAW_(3, QUADS, NITEMS(link_vertices), link_vertices, WHITE, 2);
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    }
                    SET_DRAW_TRANSFORM(PV_3D);
                } else { // option 2: actually move all the vertices (and leave the camera as is)
                    ASSERT(NITEMS(link_vertices) == 24);
                    vec3 *vertices = (vec3 *) malloc(NUM_LINKS * 24 * sizeof(vec3));
                    vec3 *colors = (vec3 *) malloc(NUM_LINKS * 24 * sizeof(vec3));
                    int k = 0;
                    for_(i, NUM_LINKS) {
                        mat4 M = Translation(joint_positions[i]) * joint_orientations[i] * Scaling(1, L[i], 1);
                        for_(j, 24) {
                            vertices[k] = transformPoint(M, link_vertices[j]); // fornow
                            colors[k] = link_colors[j];
                            ++k;
                        }
                    }
                    DRAW_(3, QUADS, NUM_LINKS * 24, vertices, colors);
                    glPolygonMode(GL_FRONT_AND_BACK,  GL_LINE);
                    DRAW_(3, QUADS, NUM_LINKS * 24, vertices, WHITE);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }
        }

        { // draw axes
            vec3 vertices[2 * NUM_LINKS] = {};
            vec3 colors[2 * NUM_LINKS] = {};
            for_(i, NUM_LINKS) {
                vec3 axis = {};
                axis[joint_axes[i]] = .5;
                vertices[2 * i    ] = joint_positions[i];
                vertices[2 * i + 1] = joint_positions[i] + transformVector(joint_orientations[i], axis);
                colors[2 * i] = colors[2 * i + 1] = kelly(i);
            }
            DRAW_(3, LINES, NITEMS(vertices), vertices, colors, 5);
        }

        { // draw ground
            double tmp[] = { 1, 0, 1, 1, 0, -1, -1, 0, -1, -1, 0, 1 };
            DRAW_(3, QUADS, NITEMS(tmp) / 3, tmp, AVG(WHITE, GRAY));
        }

        END_FRAME();
    }
}



void app_ik_2d() {
    #define NUM_LINKS 5
    double L[] = { 1, 2, 3, 1, 1 };
    vec2 target = { 5, 5 };

    auto get_x_of_u = [&] (vec2 *x, double *u) { // fk
        x[0] = {};
        double cumm_angle = 0;
        for_(i, NUM_LINKS) {
            cumm_angle += u[i];
            x[i + 1] = x[i] + L[i] * V2(cos(cumm_angle), sin(cumm_angle));
        }
    };

    auto EVALUATE_AND_ADD_TO = [&] (double *u, double *O, double *O_u) { // ik objective and its gradient (first derivative)
        vec2 x[NUM_LINKS + 1];
        get_x_of_u(x, u);

        if (O) {
            *O += 1e-2 * squaredNorm(x[NUM_LINKS] - target);
            for_(i, NUM_LINKS) { *O += 1e-4 * pow(u[i], 2) / 2; }
        }

        if (O_u) { // TODO analytic gradient

        }

    };

    double u[NUM_LINKS] = {};

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 15, 'F', BLACK);

        if (KEY_PRESSED['R']) {
            memset(u, 0, NUM_LINKS * sizeof(double));
        }

        if (KEY_TOGGLE['S']) { // step of gradient descent with line search
            double O = 0;
            double O_u[NUM_LINKS] = {};
            EVALUATE_AND_ADD_TO(u, &O, O_u);
            if (1) { // finite difference approximation of the gradient
                double d = 1e-5;
                for_(i, NUM_LINKS) {
                    double right = 0;
                    double tmp = u[i];
                    u[i] += d;
                    EVALUATE_AND_ADD_TO(u, &right, 0);
                    u[i] = tmp;

                    O_u[i] += (right - O) / (d);
                }
            }

            // TODO check gradient with finite differences

            double u_guess[NUM_LINKS]; {
                double stepSize = 1;
                int lineSearchIteration = 0;
                while (1) {
                    for_(i, NUM_LINKS) { u_guess[i] = u[i] - stepSize * O_u[i]; } // ~ u_guess = u - stepSize * O_u;

                    double O_guess = 0;
                    EVALUATE_AND_ADD_TO(u_guess, &O_guess, 0);

                    if (O_guess < O || lineSearchIteration++ > 10) {
                        break;
                    }

                    stepSize /= 2;
                }
            }
            memcpy(u, u_guess, NUM_LINKS * sizeof(double)); // ~ u = u_guess;
        }

        { // draw
            { // draw current joint angle graph
                vec2 tmp[NUM_LINKS];
                for_(i, NUM_LINKS) {
                    tmp[i].x = -3 - i;
                    tmp[i].y = u[i];
                }
                KELLY_DRAW_(2, POINTS, NUM_LINKS, tmp);
            }

            { // draw robot at x(u)
                vec2 x[NUM_LINKS + 1] = {};
                get_x_of_u(x, u);
                DRAW_(2, LINE_STRIP, NUM_LINKS + 1, x, GRAY);
                KELLY_DRAW_(2, POINTS, NUM_LINKS, x);
            }

            DRAW_(2, POINTS, 1, &target, GREEN);
            DRAG_2D_POINTS_(PV, 1, &target, WHITE);
        }

        END_FRAME();
    }
    #undef NUM_LINKS
}

void hello_speed_coding() {
    vec2 *s = 0;
    vec2 *v = 0;
    vec3 *c = 0;
    vec2 *particles_s = 0;
    vec2 *particles_v = 0;
    vec3 *particles_c = 0;
    int *particles_age = 0;
    vec3 *particles_c_buffer = 0;
    for_(i, 1024) {
        arrput(s, { RAND(-1, 1), RAND(-1, 1) });
        arrput(v, { RAND(-1, 1), RAND(-1, 1) });
        arrput(c, monokai(i));
    }
    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 5, 'F', AVG(GRAY, BLACK));

        {
            int *indices_to_delete = 0;
            for_(i, arrlen(s)) {
                s[i] += .016 * v[i];
                for_(d, 2) if (ABS(s[i][d]) > 1) v[i][d] *= -1;
                if (norm(MOUSE_POSITION - s[i]) < .1) {
                    arrput(indices_to_delete, i);
                    for____(12) {
                        arrput(particles_s, s[i] + V2(RAND(-.05, .05), RAND(-.05, .05)));
                        arrput(particles_v, v[i] + 2*MOUSE_POSITION_OFFSET + V2(RAND(-1, 1), RAND(0, 1)));
                        arrput(particles_c, c[i] + V3(RAND(-.05, .05), RAND(-.05, .05), RAND(-.05, .05)));
                        arrput(particles_age, 0);
                        arrput(particles_c_buffer, {});
                    }
                }
            }
            for (int i = arrlen(indices_to_delete) - 1; i >= 0; --i) {
                arrdelswap(s, indices_to_delete[i]);
                arrdelswap(v, indices_to_delete[i]);
                arrdelswap(c, indices_to_delete[i]);
            }
        }

        {
            int *indices_to_delete = 0;
            for_(i, arrlen(particles_s)) {
                particles_v[i] *= .99;
                particles_v[i] += { 0, -.05 };
                particles_s[i] += .033 * particles_v[i];
                if (ABS(particles_s[i].y) > 2) particles_v[i].y *= -RAND(.45, .55);
                if (ABS(particles_s[i].x) > 2) particles_v[i].x *= -.5;
                particles_s[i].y = CLAMP(particles_s[i].y, -2, INFINITY);
                particles_c[i];
                ++(particles_age[i]);
                particles_c_buffer[i] = (particles_age[i] < 10) ? WHITE : LERP(CLAMP((particles_age[i] - 30) / 256., 0, 1), particles_c[i], BLACK);
                if (particles_age[i] > 300) arrput(indices_to_delete, i);
            }
            for (int i = arrlen(indices_to_delete) - 1; i >= 0; --i) {
                arrdelswap(particles_s, indices_to_delete[i]);
                arrdelswap(particles_v, indices_to_delete[i]);
                arrdelswap(particles_c, indices_to_delete[i]);
                arrdelswap(particles_c_buffer, indices_to_delete[i]);
                arrdelswap(particles_age, indices_to_delete[i]);
            }
        }

        {
            double tmp[] = { -2, -2, 2, -2, 2, 2, -2, 2 };
            DRAW_(2, QUADS, NITEMS(tmp) / 2, tmp, BLACK, 1);
        }
        {
            double tmp[] = { -1, -1, 1, -1, 1, 1, -1, 1 };
            DRAW_(2, QUADS, NITEMS(tmp) / 2, tmp, AVG(GRAY, BLACK), 1);
        }
        DRAW_(2, POINTS, arrlen(particles_s), particles_s, particles_c_buffer, 3);
        DRAW_(2, POINTS, arrlen(s), s, c, 6);

        END_FRAME();
    }
}

void hello_optimization() {
    // todo line search
    // todo Newton's method

    #define NUM_POINTS 256
    vec2 *x = (vec2 *) calloc(NUM_POINTS, sizeof(vec2)); // { x0 y0 } { x1 y1 } ...
    for (int i = 0; i < NUM_POINTS; ++i) {
        x[i] = { RAND(-1, 1), RAND(-1, 1) };
    }

    while (!KEY_PRESSED['Q']) {
        BEGIN_FRAME(PV, 5, 'F', BLACK);

        // calculate the energy E and the gradient dEdx
        real E = 0; // note: not actually used (we will need to be able to compute energy for line search)
        vec2 *dEdx = (vec2 *) calloc(NUM_POINTS, sizeof(vec2));
        {
            for (int i = 0; i < NUM_POINTS; ++i) {
                for (int j = i + 1; j < NUM_POINTS; ++j) {
                    // points x_i and x_j try should be at least r apart
                    real r = .2;
                    vec2 Delta = x[i] - x[j];
                    if (norm(Delta) < r) {
                        real k = 1e0;
                        E += k * pow((squaredNorm(Delta) - pow(r, 2)), 2);
                        vec2 dEdDelta = k * 2 * (squaredNorm(Delta) - pow(r, 2)) * 2 * Delta;
                        dEdx[i] += dEdDelta;
                        dEdx[j] -= dEdDelta;
                    }
                }

                if (MOUSE_LEFT_HELD) {
                    // points x_i and the mouse should be at least r apart
                    real r = .5;
                    real k = 1e0;
                    vec2 Delta = x[i] - MOUSE_POSITION;
                    if (norm(Delta) < r) {
                        E += k * pow((squaredNorm(Delta) - pow(r, 2)), 2);
                        dEdx[i] += k * 2 * (squaredNorm(Delta) - pow(r, 2)) * 2 * Delta;
                    }
                }

                if (KEY_HELD[GLFW_KEY_SPACE]) {
                    // point x_i should stay inside circle of radius r
                    real k = 1e-1;
                    real r = 1;
                    if (norm(x[i]) > r) {
                        E += k * pow((squaredNorm(x[i]) - pow(r, 2)), 2);
                        dEdx[i] += k * 2 * (squaredNorm(x[i]) - pow(r, 2)) * 2 * x[i];
                    }
                }

                {
                    // regularizer (point x_i shouldn't run super far away from the origin)
                    real k = MOUSE_RIGHT_HELD ? 1e-2 : 1e-4;
                    E += k * squaredNorm(x[i]);
                    dEdx[i] += k * 2 * x[i];
                }
            }
        }

        printf("E(x) = %lf\n", E);

        // take a step of gradient descent (_without_ line search)
        for_(i, NUM_POINTS) {
            x[i] -= dEdx[i];
        }

        free(dEdx);

        KELLY_DRAW_(2, POINTS, NUM_POINTS, x, 5);

        END_FRAME();
    }
    #undef NUM_POINTS
}

int main() {
    // hello_speed_coding();
    // hello_c();
    // hello_linalg();

    // hello_triangle();
    // hello_triangle_more_as_i_would_actually_write_it();

    // object_oriented_balls();
    // data_oriented_balls();
    // XD_data_oriented_balls();

    // hello_balls();
    // hello_balls_plus_plus();

    // hello_pencil();
    // hello_pencil_another_way();
    // hello_pencil_with_std_vector();

    hello_optimization();

    // app_draw();
    // app_draw_plus_plus();

    // app_fk_2d();
    // app_fk_3d();

    // app_ik_2d();

    return 0;
}

