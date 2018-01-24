#include <iostream>

#define S_SAR 0x21534152 // !SAR
#define S_END 0x52415321 // RAS!

// |     2     |       8       |    x    | ... |    y    | 1 |     8     |    2    |
// | signature | header_offset | f1_data | ... | f1_name | 0 | f1_offset | end_tag |

typedef struct {
    string name;
    unsigned long offset;
} FileInfo;

class Archive {
public:
    Archive();
    int create();
    int extract();
private:
    char *buf;
    unsigned int buffsize;

    std::istream f_in;
    std::ostream f_out;
}
