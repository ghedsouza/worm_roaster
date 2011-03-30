#include "viewer.hpp"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <GL/gl.h>
#include <GL/glu.h>

#include "game_engine.hpp"

Viewer::Viewer() :
completed_rows(0),
mouse_x(0),
mouse_y(0),
x_rot_angle(55),
y_rot_angle(0),
z_rot_angle(0),
shift(0),
scale(SCALE),
game(WIDTH, HEIGHT),
rot_tick_conn_valid(0),
multi(0)
{
  set_config();
  
  // Register the fact that we want to receive these events
  add_events(Gdk::BUTTON1_MOTION_MASK    |
             Gdk::BUTTON2_MOTION_MASK    |
             Gdk::BUTTON3_MOTION_MASK    |
             Gdk::BUTTON_PRESS_MASK      | 
             Gdk::BUTTON_RELEASE_MASK    |
             Gdk::POINTER_MOTION_MASK    |
             Gdk::KEY_PRESS_MASK         |
             Gdk::VISIBILITY_NOTIFY_MASK);
             
  set_flags(Gtk::CAN_FOCUS);
  reset_multi_colours();
}

void Viewer::reset_multi_colours()
{
  // choose new colours for multi mode
  for(int i=0; i<6; i+=1)
    for(int j=0; j<3; j+=1)
    {
      multi_colours[i][j] = ( (double)(rand() % 20) )/( (double)20 );
    }
}

void Viewer::set_config()
{
  Glib::RefPtr<Gdk::GL::Config> glconfig;

  // Ask for an OpenGL Setup with
  //  - red, green and blue component colour
  //  - a depth buffer to avoid things overlapping wrongly
  //  - double-buffered rendering to avoid tearing/flickering
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
                                     Gdk::GL::MODE_DEPTH |
                                     Gdk::GL::MODE_DOUBLE);
    
  if (glconfig == 0) {
    // If we can't get this configuration, die
    std::cerr << "Unable to setup OpenGL Configuration!" << std::endl;
    abort();
  }

  // Accept the configuration
  set_gl_capability(glconfig);
}

Viewer::~Viewer()
{
  // Nothing to do here right now.
}

void Viewer::invalidate()
{
  //Force a rerender
  Gtk::Allocation allocation = get_allocation();
  get_window()->invalidate_rect( allocation, false);
  
}

void Viewer::on_realize()
{
  // Do some OpenGL setup.
  // First, let the base class do whatever it needs to
  Gtk::GL::DrawingArea::on_realize();
  
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();
  
  if (!gldrawable)
    return;

  if (!gldrawable->gl_begin(get_gl_context()))
    return;
  
  // Just enable depth testing and set the background colour.
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.7, 0.7, 1.0, 0.0);

  gldrawable->gl_end();
}

void Viewer::drawCube(int x, int y)
{
  glBegin(GL_QUADS);
  if (multi) glColor3d(multi_colours[0][0],multi_colours[0][1], multi_colours[0][2]);
  glVertex3d(x, y, 0);
  glVertex3d(x+1, y, 0);
  glVertex3d(x+1, y+1, 0);
  glVertex3d(x, y+1, 0);
  
  if (multi) glColor3d(multi_colours[1][0],multi_colours[1][1], multi_colours[1][2]);  
  glVertex3d(x, y, 1);
  glVertex3d(x+1, y, 1);
  glVertex3d(x+1, y+1, 1);
  glVertex3d(x, y+1, 1);
  
  if (multi) glColor3d(multi_colours[2][0],multi_colours[2][1], multi_colours[2][2]);
  glVertex3d(x, y+1, 0);
  glVertex3d(x+1, y+1, 0);
  glVertex3d(x+1, y+1, 1);
  glVertex3d(x, y+1, 1);
  
  if (multi) glColor3d(multi_colours[3][0],multi_colours[3][1], multi_colours[3][2]);
  glVertex3d(x, y, 0);
  glVertex3d(x+1, y, 0);
  glVertex3d(x+1, y, 1);
  glVertex3d(x, y, 1);
  
  if (multi) glColor3d(multi_colours[4][0],multi_colours[4][1], multi_colours[4][2]);
  glVertex3d(x, y, 0);
  glVertex3d(x, y, 1);
  glVertex3d(x, y+1, 1);
  glVertex3d(x, y+1, 0);
  
  if (multi) glColor3d(multi_colours[5][0],multi_colours[5][1], multi_colours[5][2]);
  glVertex3d(x+1, y, 0);
  glVertex3d(x+1, y, 1);
  glVertex3d(x+1, y+1, 1);
  glVertex3d(x+1, y+1, 0);
    
  glEnd();
}

void Viewer::drawBorder()
{
  glColor3d(0.3,0.3,0.3);
  for(int y=-1;y<HEIGHT;y+=1) {
    drawCube(-1,y);
    drawCube(WIDTH,y);
  }
  for(int x=0;x<WIDTH;x+=1) {
    drawCube(x,-1);
  }
}

void Viewer::tick()
{
  eng.tick();
  int res = game.tick();
  if (res > 0) {
    completed_rows += res;
  }
  invalidate();
}

void Viewer::drawGame()
{
  for(int r=0;r<game.getHeight()+4;r+=1) {
    for(int c=0;c<game.getWidth();c+=1) {
      int cell = game.get(r,c);
      switch(cell) {
      case -1:
        glColor3d(0.5, 0.5, 0.5);
        break;
      case 0:
        glColor3d(1.0, 0.0, 0.0);
        break;
      case 1:
        glColor3d(0.0, 1.0, 0.0);
        break;
      case 2:
        glColor3d(0.0, 0.0, 1.0);
        break;
      case 3:
        glColor3d(1.0, 0.0, 1.0);
        break;
      case 4:
        glColor3d(1.0, 1.0, 0.0);
        break;
      case 5:
        glColor3d(0, 1.0, 1.0);
        break;
      case 6:
        glColor3d(0.5, 0.5, 1.0);
        break;
      }
      if(cell >= 0)
      drawCube(c,r);
    }
  }
}
        

void Viewer::wireframe()
{
  multi = 0;
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  invalidate();
}

void Viewer::face()
{
  multi = 0;
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  invalidate();
}

void Viewer::multicoloured()
{
  multi = 1;
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  reset_multi_colours();
  invalidate();
}

void Viewer::reset()
{
  x_rot_angle = y_rot_angle = z_rot_angle = 0;
  scale = SCALE;
  if (rot_tick_conn_valid) {
    rot_tick_conn.disconnect();
    rot_tick_conn_valid = 0;
  }
  invalidate();
}

void Viewer::reset_game()
{
  game.reset();
  reset_multi_colours();
  completed_rows = 0;
  invalidate();
}

void Viewer::double_buf(int double_buffer)
{
}

bool Viewer::on_expose_event(GdkEventExpose* event)
{    
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();
    
  if (!gldrawable) return false;

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  
  // Clear the screen    
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Modify the current projection matrix so that we move the 
  // camera away from the origin.  We'll draw the game at the
  // origin, and we need to back up to see it.

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glTranslated(0.0, 0.0, -40.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Not implemented: set up lighting (if necessary)

  // Not implemented: scale and rotate the scene
  glRotated(x_rot_angle, 1,0,0);
  glRotated(y_rot_angle, 0,1,0);
  glRotated(z_rot_angle, 0,0,1);
  glScaled((double)scale/(double)SCALE,
           (double)scale/(double)SCALE,
           (double)scale/(double)SCALE);

  // You'll be drawing unit cubes, so the game will have width
  // 10 and height 24 (game = 20, stripe = 4).  Let's translate
  // the game so that we can draw it starting at (0,0) but have
  // it appear centered in the window.
  glTranslated(0.0, -12.0, 0.0);
  
  
  // draw ground
  glColor3d(112.0/255, 64.0/255, 0.0/255);
  glBegin(GL_QUADS);
  glVertex3d(-eng.ground.width,0,-eng.ground.length);
  glVertex3d(-eng.ground.width,0,eng.ground.length);
  glVertex3d(eng.ground.width,0,eng.ground.length);  
  glVertex3d(eng.ground.width,0,-eng.ground.length);
  glEnd();
  
  
  // Not implemented: actually draw the current game state.
  // Here's some test code that draws red triangles at the
  // corners of the game board.
  glColor3d(0.0, 0.0, 0.0);
  drawCube(eng.base.x,eng.base.y);
  
//  drawBorder();
//  drawGame();

  // We pushed a matrix onto the PROJECTION stack earlier, we 
  // need to pop it.

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  // Swap the contents of the front and back buffers so we see what we
  // just drew. This should only be done if double buffering is enabled.
  gldrawable->swap_buffers();
  gldrawable->gl_end();
   
  return true;
}

bool Viewer::on_configure_event(GdkEventConfigure* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;
  
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  // Set up perspective projection, using current size and aspect
  // ratio of display

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, event->width, event->height);
  gluPerspective(40.0, (GLfloat)event->width/(GLfloat)event->height, 0.1, 1000.0);

  // Reset to modelview matrix mode
  
  glMatrixMode(GL_MODELVIEW);

  gldrawable->gl_end();

  return true;
}

bool Viewer::on_button_press_event(GdkEventButton* event)
{
  if (rot_tick_conn_valid) {
    rot_tick_conn.disconnect();
    rot_tick_conn_valid = 0;
  }	
  mouse_x = event->x;
  mouse_y = event->y;
  return true;
}

bool Viewer::on_key_press_event(GdkEventKey* event)
{
  bool update = true;
  if (event->keyval == 65361)
  {
    game.moveLeft();
  }
  else if (event->keyval == 65362)
  {
    game.rotateCCW();
  }
  else if (event->keyval == 65363)
  {
    game.moveRight();
  }
  else if (event->keyval == 65364)
  {
    game.rotateCCW();
  }
  else if (event->keyval == 32)
  {
    game.drop();
  }
  else
  {
    update = false;
  }
  if (update)
    invalidate();
  return true;
}

bool Viewer::on_button_release_event(GdkEventButton* event)
{
  if (1) {
    if ( last_motion_time == event->time )
    {
      if (rot_tick_conn_valid) {
        rot_tick_conn.disconnect();
        rot_tick_conn_valid = 0;
      }
      rot_tick_conn = Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &Viewer::rot_tick), 100);
      rot_tick_conn_valid = 1;
    }
  }
  return true;
}

bool Viewer::rot_tick()
{
  if (last_motion_button[0])
    x_rot_angle = (x_rot_angle+last_motion_diff)%360;
    
  if (last_motion_button[1])
    y_rot_angle = (y_rot_angle+last_motion_diff)%360;
    
  if (last_motion_button[2])
    z_rot_angle = (z_rot_angle+last_motion_diff)%360;
    
  invalidate();
  return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event)
{
  if ((event->state & GDK_BUTTON1_MASK) ||
      (event->state & GDK_BUTTON2_MASK) ||
      (event->state & GDK_BUTTON3_MASK))
    {
      int diff = (event->x - mouse_x) == 0 ? 0 :
                 (event->x - mouse_x) >  0 ? 1 : -1;
      if (event->state & GDK_SHIFT_MASK)
      {
        scale = std::max((int)( (double)0.5*(double)SCALE ),
                         std::min(scale+diff, (int)( (double)1.3*SCALE )));
      }
      else
      {
        if (event->state & GDK_BUTTON1_MASK)
          x_rot_angle = (x_rot_angle+diff)%360;
          
        if (event->state & GDK_BUTTON2_MASK)
          y_rot_angle = (y_rot_angle+diff)%360;
          
        if (event->state & GDK_BUTTON3_MASK)
          z_rot_angle = (z_rot_angle+diff)%360;
          
        last_motion_diff = diff;
        last_motion_time = event->time;
        last_motion_button[0] = (event->state & GDK_BUTTON1_MASK);
        last_motion_button[1] = (event->state & GDK_BUTTON2_MASK);
        last_motion_button[2] = (event->state & GDK_BUTTON3_MASK);
      }
      mouse_x = event->x;
      mouse_y = event->y;
  }
  invalidate();
  return true;
}






















