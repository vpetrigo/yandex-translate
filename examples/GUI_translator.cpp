// This example uses wrapper classes and functions from http://courses.cse.tamu.edu/keyser/csce121/
// It is the modified files from original Stroustrup's book "Principles and Practice Using C++"
// They were modified by TA&M stuff for matching C++11 standard and have no issues with
// compilation on modern compilators.

#include "Ya_translate.hpp"
#include "GUI.h"
#include "Window.h"
#include "Graph.h"

using namespace Graph_lib;

// CONST VARIABLES
constexpr int window_w = 800;
constexpr int window_h = 600;
const Point xy {100, 100};
const string window_name {"Translator"};

struct Translator : Graph_lib::Window {
    explicit Translator(const Point& xy, int w, int h, const string& title, const string& api_key) : Window{xy, w, h, title},
        transl_btn {Point{x_max() - btn_width, 0}, btn_width, btn_height, "Translate", [] (Address, Address pw) { reference_to<Translator>(pw).translate(); }},
        quit_btn {Point{x_max() - 2 * btn_width, 0}, btn_width, btn_height, "Quit", [] (Address, Address pw) { reference_to<Translator>(pw).quit(); }},
        in_field {Point{box_x_offset, box_y_offset}, box_width, box_height, "Input"},
        out_field {Point{3 * box_x_offset, box_y_offset}, box_width, box_height, "Output"},
        lang_menu {Point{x_max() - btn_width, btn_height}, btn_width, btn_height, Menu::vertical, "Language menu"},
        tr{api_key}
    {
        attach(transl_btn);
        attach(quit_btn);
        attach(in_field);
        attach(out_field);
        lang_menu.attach(new Button{Point{0,0}, 0, 0,"en-ru",cb_enru});
        lang_menu.attach(new Button{Point{0,0}, 0, 0,"ru-en",cb_ruen});
        lang_menu.attach(new Button{Point{0,0}, 0, 0,"ru-gr",cb_rude});
        attach(lang_menu);
        tr.set_language("en", "ru");
    }
    
 private:
    static constexpr int box_width = 200;
    static constexpr int box_height = 50;
    static constexpr int box_x_offset = 150;
    static constexpr int box_y_offset = 200;
    static constexpr int btn_width = 70;
    static constexpr int btn_height = 40;
    
    static void cb_enru(Address, Address w);
    static void cb_ruen(Address, Address w);
    static void cb_rude(Address, Address w);

    void translate();
    void quit();
    
    void enru() {
        tr.set_language("en", "ru");
    }
    
    void ruen() {
        tr.set_language("ru", "en");
    }
    
    void rude() {
        tr.set_language("ru", "de");
    }
    
    Button transl_btn;
    Button quit_btn;
    
    In_box in_field;
    Out_box out_field;
    Menu lang_menu;
    Ya_translate::Ya_tr tr;
};

void Translator::translate() {    
    string s = in_field.get_string();
    
    out_field.put(tr.translate(s));
}

void Translator::quit() {
    hide();
}

void Translator::cb_enru(Address, Address w) {
    reference_to<Translator>(w).enru();
}

void Translator::cb_ruen(Address, Address w) {
    reference_to<Translator>(w).ruen();
}

void Translator::cb_rude(Address, Address w) {
    reference_to<Translator>(w).rude();
}

int main() {
    ifstream ifs {"api_key.txt"};
    
    string a_k;
    
    ifs >> a_k;
    
    Translator w {xy, window_w, window_h, window_name, a_k};
    
    gui_main();
    
    return 0;
}
