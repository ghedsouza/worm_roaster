#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include <gtkmm.h>
#include <time.h>
#include <algorithm>
#include <cstdlib>

#include "algebra.hpp"

using namespace std;

#define NUM_WORMS (4)
#define TICK_SPEED 100

static int clock_seconds()
{
  return (int)( clock()/CLOCKS_PER_SEC );
}

struct worm {
  int valid;
  Point3D pos;
};

struct game_engine {
  struct {
    double x,y,z,length,width;
  } ground;
  struct {
    Point3D pos;
  } base;
  
  int num_worms;
  worm worms[NUM_WORMS];
  
  void tick()
  {
    if (clock_seconds() > 2) num_worms = max(num_worms, 1);
    for(int i=0; i<num_worms; i++) {
      if (!worms[i].valid) {
        worms[i].valid = 1;
        worms[i].pos.x = -ground.width + (rand() % (2*(int)ground.width));
        worms[i].pos.z = -ground.length + (rand() % (2*(int)ground.length));
        worms[i].pos.y = 0;
      } else {
        Vector3D dir = (worms[i].pos-base.pos);
        dir.normalize();
        worms[i].pos = worms[i].pos + 0.1*dir;
      }
    }
  }
  
  game_engine() {
    ground.x = ground.y = ground.z = 0;
    ground.length = 10;
    ground.width = 20;
    base.pos.x = base.pos.y = base.pos.x = 0;
    num_worms = 0;
  }
};

#endif // GAME_ENGINE_HPP
