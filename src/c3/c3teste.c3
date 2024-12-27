module c3teste;

import std::io;
/*
typedef struct {
    char *items;
    size_t count;
    size_t capacity;
} Nob_String_Builder;
*/

struct Nob_String_Builder {
    ZString items;
    usz count, capacity;
}

struct Lang_t {
    char[6] code;
    ZString name;
    ZString author;
    ZString text_paused;
    ZString menu_file;
    ZString menu_file_open;
    ZString menu_file_quit;
}

fn void foo(Nob_String_Builder *sb) @export("c3_foo") {
    io::printfn("%.*s", sb.count, sb.items);
}

fn void c3_teste(Lang_t *lang) @export("c3_teste")
{
    io::printn("------------------------------------------------------");
	io::printfn("lang.name           = '%s'", lang.name);
	io::printfn("lang.code           = '%s'", (ZString)&lang.code);
	io::printfn("lang.author         = '%s'", lang.author);
	io::printfn("lang.text_paused    = '%s'", lang.text_paused);
	io::printfn("lang.menu_file      = '%s'", lang.menu_file);
	io::printfn("lang.menu_file_open = '%s'", lang.menu_file_open);
	io::printfn("lang.menu_file_quit = '%s'", lang.menu_file_quit);
    io::printn("------------------------------------------------------");
}

// fn void main() {}