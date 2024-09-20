#include "input.h"

void registerInput(nes_t *nes) {
    const controller_t *c = &nes->controller;

    // TODO: proper player input handle...
    uint8_t p1 = 0; // Player 1
    // uint8_t p2 = 0; // Player 2

    /*
    BUTTON_A      -> (1 << 7)
    BUTTON_B      -> (1 << 6)
    BUTTON_SELECT -> (1 << 5)
    BUTTON_START  -> (1 << 4)
    BUTTON_UP     -> (1 << 3)
    BUTTON_DOWN   -> (1 << 2)
    BUTTON_LEFT   -> (1 << 1)
    BUTTON_RIGHT  -> (1 << 0)
    */

    if (IsKeyPressed(c->ButtonUp)) {
        // TODO Controller Input Handle
        p1 |= BUTTON_UP;
    }
    if (IsKeyPressed(c->ButtonDown)) {
        // TODO Controller Input Handle
        p1 |= BUTTON_DOWN;
    }
    if (IsKeyPressed(c->ButtonLeft)) {
        // TODO Controller Input Handle
        p1 |= BUTTON_LEFT;
    }
    if (IsKeyPressed(c->ButtonRight)) {
        // TODO Controller Input Handle
        p1 |= BUTTON_RIGHT;
    }
    if (IsKeyPressed(c->ButtonStart)) {
        // TODO Controller Input Handle
        p1 |= BUTTON_START;
    }
    if (IsKeyPressed(c->ButtonSelect)) {
        // TODO Controller Input Handle
        p1 |= BUTTON_SELECT;
    }
    if (IsKeyPressed(c->ButtonA)) {
        // TODO Controller Input Handle
        p1 |= BUTTON_A;
    }
    if (IsKeyPressed(c->ButtonB)) {
        // TODO Controller Input Handle
        p1 |= BUTTON_B;
    }

    BITFIELD8LOG(p1);
}
