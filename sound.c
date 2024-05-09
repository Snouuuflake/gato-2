#include "header.h"

/**
 * @fn Plays hardmode soundtrack
 * @return void
 */
void startMusic() {
  system("aplay ./MEDIA/hercules.wav");
}


/**
 * @fn Stops all music
 * @return void
 */
void stopMusic() {
  system("killall aplay");
}
