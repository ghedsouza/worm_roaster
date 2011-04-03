#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include <gtkmm.h>
#include <ctime>
#include <algorithm>
#include <cstdlib>

#include "algebra.hpp"

using namespace std;

#define NUM_WORMS (100)
#define TICK_SPEED 100

static int clock_seconds()
{
//  cout << "clock: " << clock() << ", cps: " << CLOCKS_PER_SEC << " sec: " << clock()/CLOCKS_PER_SEC << endl;
  return (int)( clock()/(CLOCKS_PER_SEC/10) );
}

struct particle_t
{
  Point3D pos;
  Vector3D vel;
  Vector3D acc;
  double t;
  
  void update()
  {
    vel = vel + acc;
    pos = pos + vel;
    t -= 1.0;
  }
  
  bool dead()
  {
    return (t < 0);
  }
};

static double unif()
{
  return rand()/double(RAND_MAX);
}

struct particle_system
{   
  double t; // time
  int size;
  particle_t *p;
  
  void renew(int i)
  {
    double scale = 1+unif();
    p[i].pos = Point3D(unif(), 0, unif());
    p[i].vel = Vector3D(-0.01+(unif()*0.02), scale*2*unif()*0.01, -0.01+(unif()*0.02));
    p[i].acc = Vector3D(0,0,0);
    p[i].t = (50 + unif()*50)/scale;
  }
  
  particle_system(int s)
  {
    size = s;
    p = new particle_t[size];
    for(int i=0; i<size; i++)
    {
      renew(i);
    }
    t = 0;
  }
  void update()
  {
    for(int i=0; i<size; i++)
    {
      p[i].update();
      if (p[i].dead())
      {
        renew(i);
      }
    }
  }
};

struct worm {
  int valid;
  Point3D pos;
  worm() : valid(0) {}
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
  particle_system ps;
  
  void tick()
  {
    //cout << "tick: sec: " << clock_seconds() << endl;
    //if (clock_seconds() > 2) num_worms = max(num_worms, 1);
    
    ps.update();
    
    for(int i=0; i<num_worms; i++) {
      if (!worms[i].valid) {
        worms[i].valid = 1;
        worms[i].pos.x = -ground.width + (rand() % (2*(int)ground.width));
        worms[i].pos.z = -ground.length + (rand() % (2*(int)ground.length));
        worms[i].pos.y = 0;
//        cout << "new worm: " << worms[i].pos << endl;
      } else {
        Vector3D dir = (base.pos-worms[i].pos);
        dir.normalize();
//        worms[i].pos = worms[i].pos + 0.2*dir;
        if ((base.pos-worms[i].pos).length() < 1) {
          worms[i].valid = 0;
        }
      }
    }
  }
  
  game_engine() : ps(1000) {
    ground.x = ground.y = ground.z = 0;
    ground.length = 10;
    ground.width = 20;
    base.pos.x = base.pos.y = base.pos.x = 0;
    num_worms = 1;
    
    
  }
};

#endif // GAME_ENGINE_HPP
