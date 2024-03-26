#include "GraphicComponent.h"

GraphicComponent::GraphicComponent() {
    initAllegroRoutines();
    initDisplay();
}

void GraphicComponent::initAllegroRoutines() {

    //Inizializzo tutti i moduli di Allegro 
    al_init();  // inizializza il 

    al_install_keyboard();
    al_init_primitives_addon();
    srand(time(NULL));
}

void GraphicComponent::initDisplay() {

    //Creo il display per il disegno

    //al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE | ALLEGRO_NOFRAME);
    al_get_display_mode(0, &displayMode);
    mainDisplay = al_create_display(DIMW, DIMH);
    //al_set_window_position(mainDisplay, 100, 200);
}


void GraphicComponent::drawScene(int matrix[ROWS][COLS]) {
    al_clear_to_color(al_map_rgb(0, 0, 0)); // sfondo nero
    srand(time(0));

    int dim_x = DIMW / ROWS;
    int dim_y = DIMH / COLS;
    int x = 0, y = 0;

    for (int i = 0; i < DIMW; i += dim_x) {
        for (int j = 0; j < DIMH; j += dim_y) {
            if (matrix[x][y] == 1) {
                al_draw_filled_rectangle(i, j, i + dim_x, j + dim_y, al_map_rgb(255, 255, 0));
            }
            y++;
        }
        x++;
        y = 0;
    }

    al_flip_display();

    al_rest(0.05); // rallenta l'animazione per renderla visibile 
}


// esempio di colori rgb:
// arancione: 230, 159, 0
// verde: 0, 255, 0
// rosso: 255, 0, 0
// blu: 0, 0, 255
// giallo: 255, 255, 0
// viola: 255, 0, 255
// celeste: 0, 255, 255
// bianco: 255, 255, 255