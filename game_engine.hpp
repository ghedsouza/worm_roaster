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
  double T, t;
  
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
  
  double frac()
  {
    return t/T;
  }
};

struct particle_system
{   
  double t; // time
  int size;
  particle_t *p;
  
  void renew(int i)
  {
    double scale = 1+unif();
    scale *= 2;
    p[i].pos = Point3D(unif(), 0, unif());
    p[i].vel = Vector3D(-0.01+(unif()*0.02), scale*2*unif()*0.01, -0.01+(unif()*0.02));
    p[i].acc = Vector3D(0,0,0);
    p[i].T = p[i].t = (50 + unif()*50)/scale;
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
  double wiggle_speed_factor;
  double life;
  const double L;
  double frac() {
    return life/L;
  }
  int burning;
  double temp_t;
  worm() : valid(0), L(30) {}
};

struct game_engine {
  struct {
    double x,y,z,length,width;
  } ground;
  struct {
    Point3D pos;
    int hit;
    int H;
  } base;
  
  int num_worms;
  worm worms[NUM_WORMS];
  particle_system ps;
  int temp_t;
  
  Point3D MG_pos;
  
  void burnt(int index, double temp_t)
  {
    if (!worms[index].burning) {
      worms[index].burning = 1;
      worms[index].temp_t = temp_t;
      worms[index].life = worms[index].L;
    }
  }
  
  void tick()
  {
    temp_t += 1;
    //cout << "tick: sec: " << clock_seconds() << endl;
    //if (clock_seconds() > 2) num_worms = max(num_worms, 1);
    
    ps.update();
    base.hit--;
    if (base.hit < 0) base.hit = 0;
    
    for(int i=0; i<num_worms; i++) {
      worms[i].life--;
      if (worms[i].burning) 
      {
        if (worms[i].life < 0) {
          worms[i].valid = 0;
        }
      }
      
      if (!worms[i].valid) {
        worms[i].valid = 1;
        worms[i].burning = 0;
        worms[i].wiggle_speed_factor = 2 + unif()*8;
        double w = ( (unif()*2.0)-1.0 ) * 0.5*ground.width;
        worms[i].pos.x = base.pos.x + w + (w < 0 ? -1 : 1)*0.5*ground.width;
        
        double ll = ( (unif()*2.0)-1.0 ) * 0.5*ground.length;
        worms[i].pos.z = base.pos.z + ll + (ll < 0 ? -1 : 1)*0.5*ground.length;
        
                
//        worms[i].pos.x = 8;
//        worms[i].pos.z = -4;
        
        worms[i].pos.y = 0;
//        cout << "new worm: " << worms[i].pos << endl;
      } else {
      //continue; // TEMP, pause worm
        if (!worms[i].burning)
        {
          double thresh = 2.5;
          Vector3D dir = (base.pos-worms[i].pos);
          dir.normalize();
          if ( !( (base.pos-worms[i].pos).length() < thresh) )
            worms[i].pos = worms[i].pos + 0.2*dir;
            
          if ((base.pos-worms[i].pos).length() < thresh) {
//            worms[i].valid = 0;
            base.hit = base.H = 10;
          }
        }
      }
    }
  }
  
  game_engine() : ps(500) {
    temp_t = 0;
    ground.x = ground.y = ground.z = 0;
    ground.length = 10;
    ground.width = 20;
    base.pos.x = base.pos.y = base.pos.x = 0;
    base.hit = 0;
    num_worms = 4;    
  }
};

#endif // GAME_ENGINE_HPP
