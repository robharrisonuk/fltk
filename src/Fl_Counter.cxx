// Fl_Counter.H

// A numerical value with up/down step buttons.  From Forms.

#include <FL/Fl.H>
#include <FL/Fl_Counter.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Input.H> // for default_font

void Fl_Counter::draw() {
  int i; Fl_Boxtype boxtype[5];

  boxtype[0] = box();
  if (boxtype[0] == FL_UP_BOX) boxtype[0] = Fl_Input::default_box();
  for (i=1; i<5; i++)
    if (mouseobj == i)
      boxtype[i] = down(box());
    else
      boxtype[i] = box();

  int xx[5], ww[5];
  if (type() == FL_NORMAL_COUNTER) {
    int W = w()*15/100;
    xx[1] = x();	 ww[1] = W;
    xx[2] = x()+1*W;     ww[2] = W;
    xx[0] = x()+2*W;     ww[0] = w()-4*W;
    xx[3] = x()+w()-2*W; ww[3] = W;
    xx[4] = x()+w()-1*W; ww[4] = W;
  } else {
    int W = w()*20/100;
    xx[2] = x();	 ww[2] = W;
    xx[0] = x()+W;	 ww[0] = w()-2*W;
    xx[3] = x()+w()-1*W; ww[3] = W;
  }

  draw_box(boxtype[0], xx[0], y(), ww[0], h(), FL_WHITE);
  fl_font(textfont(), textsize(),
	  Fl_Input::default_font(), Fl_Input::default_size());
  fl_color(active_r() ? textcolor() : inactive(textcolor()));
  char str[128]; format(str);
  fl_draw(str, xx[0], y(), ww[0], h(), FL_ALIGN_CENTER);
  if (!(damage()&128)) return; // only need to redraw text

  if (type() == FL_NORMAL_COUNTER) {
    draw_box(boxtype[1], xx[1], y(), ww[1], h(), color());
    fl_draw_symbol("@-4<<", xx[1], y(), ww[1], h(), selection_color());
  }
  draw_box(boxtype[2], xx[2], y(), ww[2], h(), color());
  fl_draw_symbol("@-4<",  xx[2], y(), ww[2], h(), selection_color());
  draw_box(boxtype[3], xx[3], y(), ww[3], h(), color());
  fl_draw_symbol("@-4>",  xx[3], y(), ww[3], h(), selection_color());
  if (type() == FL_NORMAL_COUNTER) {
    draw_box(boxtype[4], xx[4], y(), ww[4], h(), color());
    fl_draw_symbol("@-4>>", xx[4], y(), ww[4], h(), selection_color());
  }
}

void Fl_Counter::increment_cb() {
  if (!mouseobj) return;
  double v = value();
  switch (mouseobj) {
  case 1: v -= lstep_; break;
  case 2: v = increment(v, -1); break;
  case 3: v = increment(v, 1); break;
  case 4: v += lstep_; break;
  }
  handle_drag(clamp(round(v)));
}

#define INITIALREPEAT .5
#define REPEAT .1

void Fl_Counter::repeat_callback(void* v) {
  Fl_Counter* b = (Fl_Counter*)v;
  if (b->mouseobj) {
    Fl::add_timeout(REPEAT, repeat_callback, b);
    b->increment_cb();
  }
}

int Fl_Counter::calc_mouseobj() {
  if (type() == FL_NORMAL_COUNTER) {
    int W = w()*15/100;
    if (Fl::event_inside(x(), y(), W, h())) return 1;
    if (Fl::event_inside(x()+W, y(), W, h())) return 2;
    if (Fl::event_inside(x()+w()-2*W, y(), W, h())) return 3;
    if (Fl::event_inside(x()+w()-W, y(), W, h())) return 4;
  } else {
    int W = w()*20/100;
    if (Fl::event_inside(x(), y(), W, h())) return 2;
    if (Fl::event_inside(x()+w()-W, y(), W, h())) return 3;
  }
  return -1;
}

int Fl_Counter::handle(int event) {
  int i;
  switch (event) {
  case FL_RELEASE:
    if (mouseobj) {
      Fl::remove_timeout(repeat_callback, this);
      mouseobj = 0;
      redraw();
    }
    handle_release();
    return 1;
  case FL_PUSH:
    handle_push();
  case FL_DRAG:
    i = calc_mouseobj();
    if (i != mouseobj) {
      Fl::remove_timeout(repeat_callback, this);
      mouseobj = i;
      if (i) Fl::add_timeout(INITIALREPEAT, repeat_callback, this);
      increment_cb();
      redraw();
    }
    return 1;
  default:
    return 0;
  }
}

Fl_Counter::~Fl_Counter() {
  Fl::remove_timeout(repeat_callback, this);
}

Fl_Counter::Fl_Counter(int x, int y, int w, int h, const char* l)
  : Fl_Valuator(x, y, w, h, l) {
  box(FL_UP_BOX);
  selection_color(FL_INACTIVE_COLOR); // was FL_BLUE
  align(FL_ALIGN_BOTTOM);
  bounds(-1000000.0, 1000000.0);
  Fl_Valuator::step(1, 10);
  lstep_ = 1.0;
  mouseobj = 0;
  textfont_ = FL_HELVETICA;
  textsize_ = FL_NORMAL_SIZE;
  textcolor_ = FL_BLACK;
}
