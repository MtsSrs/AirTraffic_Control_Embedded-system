#ifndef WIFI_H
#define WIFI_H

void wifi_start();
#ifdef EXAMPLE_POWER_SAVE_MODE
void wifi_power_save();
#endif // EXAMPLE_POWER_SAVE_MODE

#endif
