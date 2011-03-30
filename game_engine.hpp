#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include <gtkmm.h>
#include <time.h>
#include <algorithm>
#include <cstdib>

#include "algebra.hpp"

#define NUM_WORMS (4)
#define TICK_SPEED 100;

static int clock_seconds()
{
  return (int)( clock()/CLOCKS_PER_SEC );
}

struct worm {
  int valid;
  double x, y, z;
};

struct game_engine {
  struct {
    double x,y,z,length,width;
  } ground;
  struct {
    double x,y,z;
  } base;
  
  int num_worms;
  worm worms[NUM_WORMS];
  
  void tick()
  {
    if (clock_seconds() > 2) num_worms = max(num_worms, 1);
    for(int i=0; i<num_worms; i++) {
      if (!worms[i].valid) {
        worms[i].valid = 1;
        worms[i].x = -ground.width + (rand() % (2*ground.width));
        worms[i].z = -ground.length + (rand() % (2*ground.length));
        worms[i].y = 0;
      } else {
            
      }
    }
  }
  
  game_engine() {
    ground.x = ground.y = ground.z = 0;
    ground.length = 10;
    ground.width = 20;
    base.x = base.y = base.x = 0;
    num_worms = 0;
  }
};

#endif // GAME_ENGINE_HPP
