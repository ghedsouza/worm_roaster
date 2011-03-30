#include "appwindow.hpp"
#include <iostream>

AppWindow::AppWindow() :
ag(Gtk::AccelGroup::create()),
m_slow(speed_group, "Slow"),
m_medium(speed_group, "Medium"),
m_fast(speed_group, "Fast"),
m_double("Double Buffer"),
game_started(0),
base_speed(SLOW_SPEED),
update_speed(0)
{
  set_title("488 Tetrominoes on the Wall");

  // A utility class for constructing things that go into menus, which
  // we'll set up next.
  using Gtk::Menu_Helpers::MenuElem;
  
  m_slow.signal_toggled().connect(sigc::mem_fun(*this, &AppWindow::slow_toggle));
  m_medium.signal_toggled().connect(sigc::mem_fun(*this, &AppWindow::medium_toggle));
  m_fast.signal_toggled().connect(sigc::mem_fun(*this, &AppWindow::fast_toggle));
  
  // Set up the application menu
  // The slot we use here just causes AppWindow::hide() on this,
  // which shuts down the application.
  m_menu_app.items().push_back(MenuElem("_New Game", Gtk::AccelKey("n"),
    sigc::mem_fun(*this, &AppWindow::newgame)));
  m_menu_app.items().push_back(MenuElem("_Reset", Gtk::AccelKey("r"),
    sigc::mem_fun(*this, &AppWindow::reset)));
  m_menu_app.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"),
    sigc::mem_fun(*this, &AppWindow::hide)));

  m_menu_draw_mode.items().push_back(MenuElem("_Wire-frame", Gtk::AccelKey("w"),
    sigc::mem_fun(*this, &AppWindow::wireframe)));
  m_menu_draw_mode.items().push_back(MenuElem("_Face", Gtk::AccelKey("f"),
    sigc::mem_fun(*this, &AppWindow::face)));
  m_menu_draw_mode.items().push_back(MenuElem("_Multicoloured", Gtk::AccelKey("m"),
    sigc::mem_fun(*this, &AppWindow::multicoloured)));

  m_menu_speed.items().push_back(m_slow);
  m_menu_speed.items().push_back(m_medium);
  m_menu_speed.items().push_back(m_fast);
  
  //m_menu_buffering.items().push_back(m_double);
  m_menu_buffering.items().push_back(
    Gtk::Menu_Helpers::CheckMenuElem("Double _Buffering",
                  Gtk::AccelKey("b"),
                  sigc::mem_fun(*this, &AppWindow::double_buffer)));  

  this->add_accel_group( ag );
  //m_double.add_accelerator("activate", ag, 'b', Gdk::SHIFT_MASK, Gtk::ACCEL_VISIBLE);


  // Set up the menu bar
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Application", m_menu_app));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Draw Mode", m_menu_draw_mode));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Speed", m_menu_speed));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Buffering", m_menu_buffering));

  // Pack in our widgets
  
  // First add the vertical box as our single "top" widget
  add(m_vbox);

  // Put the menubar on the top, and make it as small as possible
  m_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);

  // Put the viewer below the menubar. pack_start "grows" the widget
  // by default, so it'll take up the rest of the window.
  m_viewer.set_size_request(600, 600);
  m_vbox.pack_start(m_viewer);

  show_all();
  
  Glib::signal_timeout().connect(
    sigc::mem_fun(*this, &AppWindow::tick), 100);
}

bool AppWindow::tick()
{
  m_viewer.tick();
  return true;
  
  if (game_started) {
    m_viewer.tick();
    if (comp_rows < m_viewer.completed_rows || update_speed)
    {
      update_speed = 0;
      comp_rows = m_viewer.completed_rows;
      Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &AppWindow::tick), base_speed-(10*(comp_rows)));
      return false;
    }
  }
  return true;
}

void AppWindow::double_buffer()
{
  m_viewer.double_buf( m_double.property_active() );
}

void AppWindow::newgame()
{
  game_started = 1;
  comp_rows = 0;
  m_viewer.reset_game();
}

void AppWindow::double_buf(int do_double_buffer)
{
}

void AppWindow::slow_toggle()
{
  if (base_speed != SLOW_SPEED)
    update_speed = 1;
  base_speed = SLOW_SPEED;
}

void AppWindow::medium_toggle()
{
  if (base_speed != MEDIUM_SPEED)
    update_speed = 1;
  base_speed = MEDIUM_SPEED;
}

void AppWindow::fast_toggle()
{
  if (base_speed != FAST_SPEED)
    update_speed = 1;
  base_speed = FAST_SPEED;
}

void AppWindow::reset()
{
  m_viewer.reset();
}

void AppWindow::wireframe()
{
  m_viewer.wireframe();
}

void AppWindow::face()
{
  m_viewer.face();
}

void AppWindow::multicoloured()
{
  m_viewer.multicoloured();
}

bool AppWindow::on_key_press_event( GdkEventKey *ev )
{
        // This is a convenient place to handle non-shortcut
        // keys.  You'll want to look at ev->keyval.

	// An example key; delete and replace with the
	// keys you want to process
        if( ev->keyval == 't' ) {
                return true;
        } else {
                return Gtk::Window::on_key_press_event( ev );
        }
}
