#include "app.cpp"

bool DRAG_2D_POINTS_(mat4 PV, int num_points, void *points_, vec3 color) {
    vec2 *points = (vec2 *) points_;

    static vec2 *selected;

    if (!selected) {
        vec2 *hot = 0; {
            for_(i, num_points) {
                vec2 *point = points + i;
                vec2 point_NDC = transformPoint(PV, *point);
                if (norm(point_NDC - _MOUSE_POSITION_NDC) < .05) {
                    hot = point;
                }
            }
        }
        if (hot) {
            DRAW_(2, GL_POINTS, 1, hot, color);
            if (MOUSE_LEFT_PRESSED) {
                selected = hot;
            }
        }
    } else {
        DRAW_(2, GL_POINTS, 1, selected, color);
    }

    if (MOUSE_LEFT_HELD && selected) {
        *selected = MOUSE_POSITION;
    } else if (MOUSE_LEFT_RELEASED) {
        selected = 0;
    }

    return bool(selected);
}

#define BEGIN_FRAME(PV, H, F, COLOR) POLL_INPUT();CAMERA_2D(PV, H);FULLSCREEN_TOGGLE(F);CLEAR_DRAW_BUFFER(COLOR)
#define END_FRAME SWAP_DRAW_BUFFERS

#define POINTS GL_POINTS
#define LINES GL_LINES
#define LINE_LOOP GL_LINE_LOOP
#define LINE_STRIP GL_LINE_STRIP
#define TRIANGLES GL_TRIANGLES
#define TRIANGLE_STRIP GL_TRIANGLE_STRIP
#define TRIANGLE_FAN GL_TRIANGLE_FAN
#define QUADS _HACK_GL_QUADS

#define RED monokaiRed
#define ORANGE monokaiOrange
#define YELLOW monokaiYellow
#define GREEN monokaiGreen
#define BLUE monokaiBlue
#define PURPLE monokaiPurple
#define WHITE monokaiWhite
#define GRAY monokaiGray
#define BLACK monokaiBlack

////////////////////////////////////////////////////////////////////////////////
// functions for lecture (please don't use)                                   //
////////////////////////////////////////////////////////////////////////////////

void DrawPoint2D(vec2 point, vec3 color) { DRAW_(2, POINTS, 1, &point, color); }
