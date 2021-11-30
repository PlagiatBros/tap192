#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/fl_draw.H>

#ifndef _FLATTHEME_H
#define _FLATTHEME_H

#define COLOR_REAR FL_FREE_COLOR
#define COLOR_FRONT (Fl_Boxtype)(FL_FREE_COLOR+1)
#define COLOR_RAISED (Fl_Boxtype)(FL_FREE_COLOR+2)
#define COLOR_BLACK (Fl_Boxtype)(FL_FREE_COLOR+3)
#define COLOR_WHITE (Fl_Boxtype)(FL_FREE_COLOR+4)
#define COLOR_ACCENT (Fl_Boxtype)(FL_FREE_COLOR+5)
#define COLOR_KEYSELECT (Fl_Boxtype)(FL_FREE_COLOR+6)

class Flat_Double_Window : public Fl_Double_Window
{
    public:
        Flat_Double_Window(int W, int H, const char *L = 0) : Fl_Double_Window(W,H,L)
        {

        	Fl::set_color(COLOR_REAR,33,37,43);
        	Fl::set_color(COLOR_FRONT,50,54,62);
            Fl::set_color(COLOR_RAISED,70,74,82);

        	Fl::set_color(FL_BACKGROUND_COLOR,33,37,43);
        	Fl::set_color(FL_BACKGROUND2_COLOR,33,37,43);

        	Fl::set_color(FL_FOREGROUND_COLOR,220,220,220);
        	Fl::set_color(FL_INACTIVE_COLOR,160,160,160);
        	Fl::set_color(FL_SELECTION_COLOR,200,200,200);

            Fl::set_color(COLOR_BLACK,10,10,10);
            Fl::set_color(COLOR_WHITE,220,220,220);
            Fl::set_color(COLOR_ACCENT,117,170,229);
            Fl::set_color(COLOR_KEYSELECT,120,120,120);

            visible_focus(0);
        }
};

class Flat_Window : public Fl_Window
{
    public:
        Flat_Window(int W, int H, const char *L = 0) : Fl_Window(W,H,L)
        {

        	color(COLOR_FRONT);
        }
};

class Flat_Button : public Fl_Button
{
    public:
        Flat_Button(int X, int Y, int W, int H, const char *L = 0) : Fl_Button(X,Y,W,H,L)
        {
            color(COLOR_RAISED);
            box(FL_BORDER_BOX);
            clear_visible_focus();
        }
};

class Flat_Toggle_Button : public Fl_Toggle_Button
{
    public:
        Flat_Toggle_Button(int X, int Y, int W, int H, const char *L = 0) : Fl_Toggle_Button(X,Y,W,H,L)
        {
            color(COLOR_RAISED);
            box(FL_BORDER_BOX);
            clear_visible_focus();
        }
};

class Flat_Value_Slider : public Fl_Group
{
    private:
        Fl_Slider slider;
        Fl_Value_Input input;
    public:
        Flat_Value_Slider(int X, int Y, int W, int H, const char *L = 0) :
            Fl_Group(X,Y,W,H,L),
            slider(X,Y,W,H,""),
            input(X,Y,W,H, "")
        {
            insert(input, 0);
            insert(slider, 1);
            end();

            int x,y;
            measure_label(x,y);
            slider.resize(20+x+40, Y, W-40, H);
            input.resize(20+x, Y, 40, H);

            input.type(FL_FLOAT_INPUT);
            input.step(0.01);

            slider.selection_color(COLOR_RAISED);

            box(FL_NO_BOX);
            slider.box(FL_BORDER_BOX);
            slider.type(FL_HOR_FILL_SLIDER);
            input.box(FL_BORDER_BOX);

            input.callback(Flat_Value_Slider::inputCb, this);
            slider.callback(Flat_Value_Slider::sliderCb, this);

            input.clear_visible_focus();
            slider.clear_visible_focus();
        }
        float value(){return slider.value();};
        void value(float f){slider.value(f);input.value(f);};
        void bounds(float a, float b){slider.bounds(a, b);input.bounds(a, b);};
        void type(uchar t){slider.type(t);};

        static void sliderCb(Fl_Widget* w,void* f){
            Flat_Value_Slider *self = (Flat_Value_Slider*) f;
            self->input.value(self->slider.value());
            self->do_callback();
        };
        static void inputCb(Fl_Widget* w,void* f){
            Flat_Value_Slider *self = (Flat_Value_Slider*) f;
            self->slider.value(self->input.value());
            self->do_callback();
        };
};



class Flat_Counter : public Fl_Counter
{
    public:
        Flat_Counter(int X, int Y, int W, int H, const char *L = 0) : Fl_Counter(X,Y,W,H,L)
        {
            color(COLOR_RAISED);
            box(FL_BORDER_BOX);
            clear_visible_focus();
        }
};

class Flat_Menu_Bar : public Fl_Menu_Bar
{
    public:
        Flat_Menu_Bar(int X, int Y, int W, int H, const char *L = 0) : Fl_Menu_Bar(X,Y,W,H,L)
        {
            box(FL_BORDER_BOX);
            down_box(FL_FLAT_BOX);
            color(COLOR_FRONT);
        }
};


class Flat_Tabs : public Fl_Tabs
{
    public:
        Flat_Tabs(int X, int Y, int W, int H, const char *L = 0) : Fl_Tabs(X,Y,W,H,L)
        {
            box(FL_FLAT_BOX);
            selection_color(COLOR_FRONT);
            color(COLOR_REAR);
            clear_visible_focus();
        }
};

class Flat_Group : public Fl_Group
{
    public:
        Flat_Group(int X, int Y, int W, int H, const char *L = 0) : Fl_Group(X,Y,W,H,L)
        {
            box(FL_FLAT_BOX);
            color(COLOR_FRONT);
        }
};


class Flat_Pack : public Fl_Pack
{
    public:
        Flat_Pack(int X, int Y, int W, int H, const char *L = 0) : Fl_Pack(X,Y,W,H,L)
        {
            box(FL_FLAT_BOX);
            color(COLOR_FRONT);
        }
};


class Flat_Check_Button : public Fl_Check_Button
{
    public:
        Flat_Check_Button(int X, int Y, int W, int H, const char *L = 0) : Fl_Check_Button(X,Y,W,H,L)
        {
            clear_visible_focus();
        }
};



class Flat_Choice : public Fl_Choice
{
    public:
        Flat_Choice(int X, int Y, int W, int H, const char *L = 0) : Fl_Choice(X,Y,W,H,L)
        {
            down_box(FL_BORDER_BOX);
            box(FL_BORDER_BOX);
            color(COLOR_RAISED);
            clear_visible_focus();
        }
        void draw()
        {
            // from NTK @ Jonathan Moore Liles
            int dx = Fl::box_dx(down_box());
            int dy = Fl::box_dy(down_box());
            int H = h() - 2 * dy;
            int W = (H > 20) ? 20 : H;
            int X = x() + w() - W - dx;
            int Y = y() + dy;
            int w1 = (W - 4) / 3; if (w1 < 1) w1 = 1;
            int x1 = X + (W - 2 * w1 - 1) / 2;
            int y1 = Y + (H - w1 - 1) / 2;

            /* if (Fl::scheme()) { */
            draw_box(box(), color());

            fl_color(active_r() ? labelcolor() : fl_inactive(labelcolor()));
              // Show larger up/down arrows...
            fl_polygon(x1, y1 + 3, x1 + w1, y1 + w1 + 3, x1 + 2 * w1, y1 + 3);
            fl_polygon(x1, y1 + 1, x1 + w1, y1 - w1 + 1, x1 + 2 * w1, y1 + 1);

            W += 2 * dx;

            if (mvalue()) {
            Fl_Menu_Item m = *mvalue();
            if (active_r()) m.activate(); else m.deactivate();

            // ERCO
            int xx = x() + dx, yy = y() + dy + 1, ww = w() - W, hh = H - 2;

            fl_push_clip(xx, yy, ww, hh);

            if ( Fl::scheme()) {
              Fl_Label l;
              l.value = m.text;
              l.image = 0;
              l.deimage = 0;
              l.type = m.labeltype_;
              l.font = m.labelsize_ || m.labelfont_ ? m.labelfont_ : textfont();
              l.size = m.labelsize_ ? m.labelsize_ : textsize();
              l.color= m.labelcolor_ ? m.labelcolor_ : textcolor();
              if (!m.active()) l.color = fl_inactive((Fl_Color)l.color);
              fl_draw_shortcut = 2; // hack value to make '&' disappear
              l.draw(xx+3, yy, ww>6 ? ww-6 : 0, hh, FL_ALIGN_LEFT);
              fl_draw_shortcut = 0;
              if ( Fl::focus() == this ) draw_focus(box(), xx, yy, ww, hh);
            }
            else {
              fl_draw_shortcut = 2; // hack value to make '&' disappear
              m.draw(xx, yy, ww, hh, this, Fl::focus() == this);
              fl_draw_shortcut = 0;
            }

            fl_pop_clip();
            }

            draw_label();
        }
};


#endif
