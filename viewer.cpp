#include "viewer.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <GL/gl.h>
#include <GL/glu.h>

#include "a2.hpp"
#include "algebra.hpp"
#include "game_engine.hpp"
#include "perlin.h"

static int tid = 1;

static int win_size = 600;

/*  Create checkerboard texture  */
#define NOISE_SIZE 256
#define checkImageWidth NOISE_SIZE
#define checkImageHeight NOISE_SIZE
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

static GLuint texName;

void makeCheckImage(void)
{
   int i, j, c;
    
   for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
         c = ((((i&0x8)==0)^((j&0x8))==0))*255;
//         c = (int)(255.0 * PerlinNoise_2D(i,j));
//         double p = 0.1*PerlinNoise_2D(i, j, 0.25, 4) + 0.9*PerlinNoise_2D(i, j, 0.95, 1);
         double p = PerlinNoise_2D(i, j, 0.01, 1);
//         p = cos(p);
         p = fabs(p);
//         p = p*10;
//         cout << "p: " << p << endl;
//         if (p<0.1) p = 0;
//         else p = 1;
         c = int(255*p);
         //glColor3d(112.0/255, 64.0/255, 0.0/255);
         checkImage[i][j][0] = (GLubyte) 100 + int(p*200);
         checkImage[i][j][1] = (GLubyte) 55 + int(p*100);
         checkImage[i][j][2] = (GLubyte) 0;
         checkImage[i][j][3] = (GLubyte) 255;
      }
   }
}

void init(void)
{    
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);
   glEnable(GL_DEPTH_TEST);

   makeCheckImage();
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(1, &texName);
   glBindTexture(GL_TEXTURE_2D, texName);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
                   GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                   GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, 
                checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
                checkImage);
}

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
  
  init();
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
  
  init();
  
  // Just enable depth testing and set the background colour.
  glEnable(GL_DEPTH_TEST);
  
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glClearColor(0.7, 0.7, 1.0, 0.0);
  
  gldrawable->gl_end();
}

void Viewer::drawCube(double x, double y, double z)
{
  glBegin(GL_QUADS);
  if (multi) glColor3d(multi_colours[0][0],multi_colours[0][1], multi_colours[0][2]);
  glVertex3d(x, y, z);
  glVertex3d(x+1, y, z);
  glVertex3d(x+1, y+1, z);
  glVertex3d(x, y+1, z);
  
  if (multi) glColor3d(multi_colours[1][0],multi_colours[1][1], multi_colours[1][2]);  
  glVertex3d(x, y, z+1);
  glVertex3d(x+1, y, z+1);
  glVertex3d(x+1, y+1, z+1);
  glVertex3d(x, y+1, z+1);
  
  if (multi) glColor3d(multi_colours[2][0],multi_colours[2][1], multi_colours[2][2]);
  glVertex3d(x, y+1, z);
  glVertex3d(x+1, y+1, z);
  glVertex3d(x+1, y+1, z+1);
  glVertex3d(x, y+1, z+1);
  
  if (multi) glColor3d(multi_colours[3][0],multi_colours[3][1], multi_colours[3][2]);
  glVertex3d(x, y, z);
  glVertex3d(x+1, y, z);
  glVertex3d(x+1, y, z+1);
  glVertex3d(x, y, z+1);
  
  if (multi) glColor3d(multi_colours[4][0],multi_colours[4][1], multi_colours[4][2]);
  glVertex3d(x, y, z);
  glVertex3d(x, y, z+1);
  glVertex3d(x, y+1, z+1);
  glVertex3d(x, y+1, z);
  
  if (multi) glColor3d(multi_colours[5][0],multi_colours[5][1], multi_colours[5][2]);
  glVertex3d(x+1, y, z);
  glVertex3d(x+1, y, z+1);
  glVertex3d(x+1, y+1, z+1);
  glVertex3d(x+1, y+1, z);
    
  glEnd();
}

void Viewer::drawBorder()
{
  glColor3d(0.3,0.3,0.3);
  for(int y=-1;y<HEIGHT;y+=1) {
    drawCube(-1,y,0);
    drawCube(WIDTH,y,0);
  }
  for(int x=0;x<WIDTH;x+=1) {
    drawCube(x,-1,0);
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
      drawCube(c,r,0);
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

double deg_to_rad(double d)
{
  return M_PI*d/180;
}

double rad_to_deg(double r)
{
  return 180.0/M_PI*r;
}

void drawWorm()
{
  glColor3d(0,1,0);
  GLUquadric *quad = gluNewQuadric();
  glPushMatrix();
  glTranslated(2, 0, 0);
  glScaled(2,2,2);
  //gluDisk(quad, 0,10,10,10);
  Matrix4x4 trans;
  int circle_segments = 3;
  vector<Point3D> ring1;
  ring1.resize(circle_segments);
  for(int t=0; t<circle_segments; t++)
  {
    double deg = double(t)/360;
    double cx = cos(deg_to_rad(deg));
    double cy = sin(deg_to_rad(deg));
    ring1.at(t) = Point3D(cx, cy, 0);
  }
  trans = trans * translation(Vector3D(-0.5, 0, 0)) * rotation(30, 'y') * translation(Vector3D(0.5, 0, 0));
  vector<Point3D> ring2;
  ring2.resize(circle_segments);
  for(int t=0; t<circle_segments; t++)
  {
    double deg = double(t)/360;
    double cx = cos(deg_to_rad(deg));
    double cy = sin(deg_to_rad(deg));
    cout << "before: " << Point3D(cx, cy, 0) << endl;
    ring2.at(t) = trans * Point3D(cx, cy, 0);
    cout << "after: " << ring2.at(t) << endl;
  }
  exit(-1);
#if 0
    glBegin(GL_POLYGON);      
    glVertex3d(0, 0, 0);
    glVertex3d(0, 1, 0);
    glVertex3d(1, 1, 0);
    glVertex3d(1, 0, 0);
    glEnd();
    #endif
  
  for(int i=0; i<circle_segments; i++)
  {
    Point3D pt1a = ring1.at(i);
    Point3D pt1b = ring1.at((i+1)%ring1.size());
    Point3D pt2a = ring2.at(i);
    Point3D pt2b = ring2.at((i+1)%ring2.size());
    
//    cout << "1a: " << pt1a << ", 1b: " << pt1b << ", pt2a: " << pt2a << ", pt2b: " << pt2b << endl;
      
    glBegin(GL_POLYGON);
    glVertex3d(pt1a.x, pt1a.y, pt1a.z);
    glVertex3d(pt2a.x, pt2a.y, pt2a.z);
    glVertex3d(pt2b.x, pt2b.y, pt2b.z);
    glVertex3d(pt1b.x, pt1b.y, pt1b.z);                  
    glEnd();
  }
//  exit(-1);
  glPopMatrix();
}

void drawWorm_old()
{
#if 1
  int T = 10;
  for(int t=0; t<T; t++)
  {
    double tt = double(t)/double(T)*3;
    
  }

#else



  double length = 2;
  int prec = 30;
  
  vector< vector<Point3D> > rings;
  rings.resize(prec);
  
  for(int p=0; p<prec; p++)
  {
    double x1 = double(p)/double(prec)*length;
    double z1 = sin(x1);
    
    double x2 = double(p+1)/double(prec)*length;
    double z2 = sin(x2);
    
    double angle = rad_to_deg( atan2(z2-z1, x2-x1) );
    if (angle < 0) angle = (90+angle);
//    cout << "angle: " << angle << endl;
    
    Matrix4x4 rot = rotation(angle, 'y');
    
    int circle_segments = 3;
    rings.at(p).resize(circle_segments);
    
    for(int c=0; c<circle_segments; c++)
    {
      double d = double(c)/double(circle_segments)*360;
//      cout << "d: " << d << endl;
      double cx = cos(deg_to_rad(d));
      double cy = sin(deg_to_rad(d));
//      cout << "cx : " << cx << ", cy: " << cy << endl;
      Point3D cp(cx, cy, 0);      
      cp = rot*cp;
      cp = cp + (Point3D(x1, 0, z1)-Point3D(0,0,0));
      rings.at(p).at(c) = cp;
    }
  }
//  exit(-1);
  

  glPushMatrix();
  glTranslated(2,0,0);
  glScaled(3,3,3);
  
  for(int i=0; i<rings.size()-1; i++)
  {
    double acc = 0.2;
    vector<Point3D> points = rings.at(i);
    for(int j=0; j<points.size(); j++)
    { 
      glColor3d(0.5, 0.2+acc, 0.2+acc);
      acc += 0.2;
      Point3D pt1a = points.at(j);
      Point3D pt1b = points.at((j+1)%points.size());
      Point3D pt2a = rings.at(i+1).at(j);
      Point3D pt2b = rings.at(i+1).at((j+1)%points.size());
      glBegin(GL_POLYGON);      
      glVertex3d(pt1a.x, pt1a.y, pt1a.z);
      glVertex3d(pt2a.x, pt2a.y, pt2a.z);
      glVertex3d(pt2b.x, pt2b.y, pt2b.z);
      glVertex3d(pt1b.x, pt1b.y, pt1b.z);                  
      glEnd();
      break;
    }
  }
  glPopMatrix();
#endif
}

void drawRing()
{
  int segments = 10;
  double height_scale = 1;
  
  vector< vector<double> > x_vals;
  vector< vector<double> > y_vals;
  
  x_vals.resize(segments+1);
  y_vals.resize(segments+1);
  
  for(int s = 0; s <= segments ; s++)
  {
    x_vals.at(s).resize(2);
    y_vals.at(s).resize(2);
    
    double d = (double(s)/double(segments)) * 360.0;
    double r_outer = 1, r_inner = 0.9;
    double x_outer = r_outer*cos(deg_to_rad(d)), y_outer = r_outer*sin(deg_to_rad(d));
    double x_inner = r_inner*cos(deg_to_rad(d)), y_inner = r_inner*sin(deg_to_rad(d));
    x_vals.at(s)[0] = x_inner;
    y_vals.at(s)[0] = y_inner;
    
    x_vals.at(s)[1] = x_outer;
    y_vals.at(s)[1] = y_outer;
  }
  
  glBegin(GL_QUADS);
  for(int i=0; i<=segments; i++)
  {
    int next = (i+1)%(segments+1);
   
    glColor3d(0.5, 0.5, 0.5);
    glVertex3d(x_vals.at(i)[0], 0, y_vals.at(i)[0]);
    glVertex3d(x_vals.at(i)[1], 0, y_vals.at(i)[1]);
    glVertex3d(x_vals.at(next)[1], 0, y_vals.at(next)[1]);    
    glVertex3d(x_vals.at(next)[0], 0, y_vals.at(next)[0]);

    glColor3d(0.5, 0.5, 0.5);
    glVertex3d(x_vals.at(i)[0], height_scale, y_vals.at(i)[0]);
    glVertex3d(x_vals.at(i)[1], height_scale, y_vals.at(i)[1]);
    glVertex3d(x_vals.at(next)[1], height_scale, y_vals.at(next)[1]);    
    glVertex3d(x_vals.at(next)[0], height_scale, y_vals.at(next)[0]);
    
    glColor3d(0.3, 0.3, 0.3);    
    glVertex3d(x_vals.at(i)[0], 0, y_vals.at(i)[0]);
    glVertex3d(x_vals.at(i)[0], height_scale, y_vals.at(i)[0]);
    glVertex3d(x_vals.at(next)[0], height_scale, y_vals.at(next)[0]);
    glVertex3d(x_vals.at(next)[0], 0, y_vals.at(next)[0]);
    
    glColor3d(0.5, 0.5, 0.5);
    glVertex3d(x_vals.at(i)[1], 0, y_vals.at(i)[1]);
    glVertex3d(x_vals.at(i)[1], height_scale, y_vals.at(i)[1]);
    glVertex3d(x_vals.at(next)[1], height_scale, y_vals.at(next)[1]);
    glVertex3d(x_vals.at(next)[1], 0, y_vals.at(next)[1]);
  }
  glEnd();
  
  GLfloat mat_solid[] = { 0.75, 0.75, 0.0, 1.0 };
  GLfloat mat_zero[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat mat_transparent[] = { 0.0, 0.0, 0.9, 0.0 };
  GLfloat mat_emission[] = { 0.0, 0.3, 0.3, 0.6 };
  
//  glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
//  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_transparent);
  glEnable (GL_BLEND);
  glDepthMask (GL_FALSE);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE);
  
  glColor3d(0.3, 0.3, 0.4);
  glBegin(GL_POLYGON);
  for(int i=0; i<=segments; i++)
  { 
    glVertex3d(x_vals.at(i)[1], 0, y_vals.at(i)[1]);
  }
  glEnd();
  
  glDepthMask (GL_TRUE);
  glDisable (GL_BLEND);
}

void Viewer::drawMG()
{
  GLUquadric *quad = gluNewQuadric();

  double handle_rad = 1.0;
  double handle_length = handle_rad * 10;
  double rivet_scale = 7.0;
  double rivet_rad_scale = 1.2;
  glColor3d(0.1, 0.1, 0.1);
  double g_x = max(0.0, double(min(win_size, mouse_x)))/double(win_size);
  double g_y = (600.0 -double(max(0.0, (double)min(win_size, mouse_y))) )/double(win_size);
  
  g_x -= 0.5;
  g_y -= 0.5;
  
  g_x *= 4*eng.ground.width;
  g_y *= 4*eng.ground.length;
  
  glTranslated(g_x, 5.0, -(g_y-handle_length*3/2));
  glRotated(180, 0,1,0);
  
  gluCylinder(quad, handle_rad, handle_rad, handle_length, 10, 10);
  gluDisk(quad, 0, handle_rad, 10, 10);
  
  glPushMatrix();
  glTranslated(0, 0, handle_length - handle_length/rivet_scale);
  glColor3d(0.6, 0.6, 0.6);
  gluDisk(quad, handle_rad, handle_rad*rivet_rad_scale, 10, 10);
  gluCylinder(quad, handle_rad*rivet_rad_scale, handle_rad*rivet_rad_scale, handle_length/rivet_scale, 10, 10);
  glTranslated(0, 0, handle_length/rivet_scale);
  gluDisk(quad, 0, handle_rad*rivet_rad_scale, 10, 10);
  glPopMatrix();
  
  glTranslated(0, -2*rivet_rad_scale*1.2/2, handle_length*(1.45));
  glScaled(handle_length/2, 2*rivet_rad_scale*1.2, handle_length/2);
  drawRing();  
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
  
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  // Not implemented: set up lighting (if necessary)

  // Not implemented: scale and rotate the scene
  glRotated(x_rot_angle, 1,0,0);
  glRotated(y_rot_angle, 0,1,0);
  glRotated(z_rot_angle, 0,0,1);
  glScaled((double)scale/(double)SCALE,
           (double)scale/(double)SCALE,
           (double)scale/(double)SCALE);

  // look at the ground from above-back
  glTranslated(0.0, -12.0, 0.0);
  
  // draw ground
  //glColor3d(112.0/255, 64.0/255, 0.0/255);
  
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_TEXTURE_2D);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   glBindTexture(GL_TEXTURE_2D, texName);
//    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  
  glBegin(GL_QUADS);
  glTexCoord2d (0.0, 0.0); glVertex3d(-eng.ground.width,0,-eng.ground.length);
  glTexCoord2d (1.0, 0.0); glVertex3d(-eng.ground.width,0,eng.ground.length);
  glTexCoord2d (1.0, 1.0); glVertex3d(eng.ground.width,0,eng.ground.length);  
  glTexCoord2d (0.0, 1.0); glVertex3d(eng.ground.width,0,-eng.ground.length);
  glEnd();
  
  glDisable(GL_TEXTURE_2D);

  // draw base
  glColor3d(0.0, 0.0, 0.0);
  drawCube(eng.base.pos.x,eng.base.pos.y, eng.base.pos.z);
  
  // draw worms
  for(int i=0; i<eng.num_worms; i++) {
    //cout << "worm: " << i << ": " << eng.worms[i].pos << endl;
    glColor3d(0.2, 0.2, 0.2);
    drawCube(eng.worms[i].pos.x, eng.worms[i].pos.y, eng.worms[i].pos.z);
    glPushMatrix();
    glTranslated(eng.worms[i].pos.x, eng.worms[i].pos.y+1, eng.worms[i].pos.z);
//    glScaled(0.1, 0.1, 0.1);
    glColor3d(1.0, 0.0, 0.0);
    
    for (int p=0; p<eng.ps.size; p++)
    {
      double size = 0.1;
      Point3D pos = eng.ps.p[p].pos;
      glColor3d(1.0,
                0.5*pow(( 0.5*pow(2, -10*fabs(pos.x-0.5)) + 
                          pow(1*eng.ps.p[p].frac(),2) +
                          unif()*0.0 ), 5),
                0.0);  
//      cout << "x: " << pos.x << endl;
      glEnable (GL_BLEND);
      glDepthMask (GL_FALSE);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE);
      
      glBegin(GL_QUADS);
      glVertex3d(pos.x, pos.y, pos.z);
      glVertex3d(pos.x, pos.y+size, pos.z);
      glVertex3d(pos.x+size, pos.y+size, pos.z);
      glVertex3d(pos.x+size, pos.y, pos.z);      
      glEnd();
      
      glDepthMask (GL_TRUE);
      glDisable (GL_BLEND);
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
//    exit(-1);
    glPopMatrix();
  }
  
  drawWorm();
  
  drawMG();
  
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
//      cout << mouse_x << ", " << mouse_y << endl;
  }
  mouse_x = event->x;
  mouse_y = event->y;
  invalidate();
  return true;
}






















