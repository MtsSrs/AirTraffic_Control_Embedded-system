#ifndef CONTROLLER_H
#define CONTROLLER_H

enum RunwayStatus { kRed, kGreen };

void handle_embarque();
void handle_desembarque();
void setup_state();

#endif // CONTROLLER_H

