module c3teste;
import std::io;
import std::encoding::csv;
import std::core::cinterop;
/*
typedef struct {
    char *items;
    size_t count;
    size_t capacity;
} Nob_String_Builder;
*/

struct Nob_String_Builder {
    ZString items;
    CULongLong count, capacity;
}

struct Lang_t {
    ZString base;
    CChar[2] code;
    CChar[2] country;
    ZString name;
    ZString author;
    ZString text_paused;
    ZString menu_file;
    ZString menu_file_open;
    ZString menu_file_quit;
}

fn void foo(Nob_String_Builder *sb) {
    io::printfn("%.*s", sb.count, sb.items);
}

fn void c3_teste(Lang_t *lang) @export("c3_teste")
{
    io::printn("------------------------------------------------------");
    if(lang.base != null) {
        io::printfn("lang.base     = \"%s\"", lang.base);
    }
	io::printfn("lang.name           = \"%s\"", lang.name);
	io::printfn("lang.code           = \"%s-%s\"", (String) (lang.code[0..1]), (String) (lang.country[0..1]));
	io::printfn("lang.author         = \"%s\"", lang.author);
	io::printfn("lang.text_paused    = \"%s\"", lang.text_paused);
	io::printfn("lang.menu_file      = \"%s\"", lang.menu_file);
	io::printfn("lang.menu_file_open = \"%s\"", lang.menu_file_open);
	io::printfn("lang.menu_file_quit = \"%s\"", lang.menu_file_quit);
    io::printn("------------------------------------------------------");
}

fn void! main() @if(!$feature(NOMAIN)){
    io::printn("aeHOOOOOOOOOOOOOOOOOOO");
    Lang_t l;
    l.name = "Portuguese (Brazil)";
    l.code = "pt";
    l.country = "BR";
    c3_teste(&l);

    io::printn("Reading CSV");
    String filePath = "../../lang/pt_br.lng";

    if(!file::is_file(filePath)){
        io::printfn("File not found: %s", filePath);
        return;
    } else {
        io::printfn("File found: %s", filePath);
    }
    File! file = file::open(filePath, "r");
    defer (void)file.close();
    
    if (catch excuse = file)
    {
        io::printfn("Excuse found: '%s'", excuse);
        return excuse?; // Returning Excuse using the `?` suffix
    } else {
        io::printn("File opened successfully");
    }


    CsvReader csv;
    csv.init(&file, ",");
    // macro CsvReader.@each_row(self, int rows = int.max; @body(String[] row))
    csv.@each_row(; String[] row)
    {
        if(row.len < 2) {
            io::printfn("Invalid row: %s", row);
        } else {
            io::printf("\"%s\" -> \"%s\"", row[0].trim(), row[1].trim());
            if(row.len > 2) {
                io::printfn(" # %s", row[2].trim());
            } else {
                io::printn("");
            }
        }
    };
}
