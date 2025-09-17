#pragma once

#include <Windows.h>

class FrameCreator;

class LineDrawer {
public:
    LineDrawer(FrameCreator* frameCreator);

    void DrawLine(COORD start, COORD end, SHORT color);

private:
    FrameCreator* frame;
};
