#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <gtkmm.h>
#include "viewer.hpp"

#define SLOW_SPEED (600)
#define MEDIUM_SPEED (300)
#define FAST_SPEED (200)


class AppWindow : public Gtk::Window {
public:
  AppWindow();
  enum SPEED { SLOW, MEDIUM, FAST };
  bool tick();
  
protected:
  virtual bool on_key_press_event( GdkEventKey *ev );
  Gtk::RadioButtonGroup speed_group;

private:
  void double_buf(int);
  void reset();
  void newgame();
  void wireframe();
  void face();
  void multicoloured();
  void speed_slow();
  void speed_medium();
  void speed_fast();
  void slow_toggle();
  void medium_toggle();
  void fast_toggle();
  void double_buffer();
  
  Glib::RefPtr<Gtk::AccelGroup> ag;

  Gtk::RadioMenuItem m_slow, m_medium, m_fast;
  Gtk::CheckMenuItem m_double;

  // A "vertical box" which holds everything in our window
  Gtk::VBox m_vbox;

  // The menubar, with all the menus at the top of the window
  Gtk::MenuBar m_menubar;
  // Each menu itself
  Gtk::Menu m_menu_app;
  Gtk::Menu m_menu_draw_mode;
  Gtk::Menu m_menu_speed;
  Gtk::Menu m_menu_buffering;

  // The main OpenGL area
  Viewer m_viewer;
  
  int game_started;
  int comp_rows;
  int base_speed;
  int update_speed;
};

#endif
