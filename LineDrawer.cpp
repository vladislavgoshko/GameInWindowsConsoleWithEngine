#include "FrameCreator.h"
#include <algorithm>
#include "LineDrawer.h"

LineDrawer::LineDrawer(FrameCreator* frameCreator) : frame(frameCreator) {

}

void LineDrawer::DrawLine(COORD start, COORD end, SHORT color) {
    int x1 = start.X;
    int y1 = start.Y;
    int x2 = end.X;
    int y2 = end.Y;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        frame->DrawPixel(x1, y1, color);

        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}