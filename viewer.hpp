#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <gtkmm.h>
#include <gtkglmm.h>
#include "game.hpp"
#include "game_engine.hpp"

#define SCALE (50)
#define HEIGHT (20)
#define WIDTH (10)

// The "main" OpenGL widget
class Viewer : public Gtk::GL::DrawingArea {
public:
  Viewer();
  virtual ~Viewer();

  // A useful function that forces this widget to rerender. If you
  // want to render a new frame, do not call on_expose_event
  // directly. Instead call this, which will cause an on_expose_event
  // call when the time is right.
  void invalidate();
  void double_buf(int);
  void wireframe();
  void face();
  void multicoloured();
  void reset();
  void reset_game();
  void tick();
  int completed_rows;
  	
protected:

  // Events we implement
  // Note that we could use gtkmm's "signals and slots" mechanism
  // instead, but for many classes there's a convenient member
  // function one just needs to define that'll be called with the
  // event.

  // Called when GL is first initialized
  virtual void on_realize();
  // Called when our window needs to be redrawn
  virtual bool on_expose_event(GdkEventExpose* event);
  // Called when the window is resized
  virtual bool on_configure_event(GdkEventConfigure* event);
  // Called when a mouse button is pressed
  virtual bool on_button_press_event(GdkEventButton* event);
  // Called when a mouse button is released
  virtual bool on_button_release_event(GdkEventButton* event);
  // Called when the mouse moves
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual bool on_key_press_event(GdkEventKey* event);

private:
  void set_config();
  void drawCube(double,double,double);
  void drawBorder();
  bool rot_tick();
  void drawGame();
  void reset_multi_colours();
  int mouse_x, mouse_y, x_rot_angle, y_rot_angle, z_rot_angle;
  unsigned int last_motion_time;
  int last_motion_diff, last_motion_button[3];
  int shift, scale;
  Game game;
  
  sigc::connection rot_tick_conn;
  int rot_tick_conn_valid;
  int multi;
  
  double multi_colours[6][3];
  game_engine eng;
};

#endif
