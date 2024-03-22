#ifndef GC_H
#define GC_H
#define DIMW 1000
#define DIMH 1000
#define ROWS 100
#define COLS 100

#include <iostream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

class GraphicComponent
{
private:
    ALLEGRO_DISPLAY* mainDisplay;
    ALLEGRO_DISPLAY_MODE displayMode;
    // const int FPS = 10;

    void initAllegroRoutines();
    void initDisplay();


public:
    GraphicComponent();
    void drawScene(int matrix[ROWS][COLS]);
};

#endif