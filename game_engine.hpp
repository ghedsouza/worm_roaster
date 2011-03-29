#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include <gtkmm.h>
#include <time.h>

#define NUM_WORMS (4)
#define TICK_SPEED 100;

static int clock_seconds()
{
  return (int)( clock()/CLOCKS_PER_SEC );
}

struct worm {
  double x, y;
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
  
  bool tick()
  {
    return true;
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
