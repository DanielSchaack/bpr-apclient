file(READ "${INPUT}" hex HEX)
string(LENGTH "${hex}" hexlen)
math(EXPR size "${hexlen} / 2")

string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," bytes "${hex}")
# line-wrap every 16 bytes
string(REGEX REPLACE "((0x[0-9a-f][0-9a-f],){16})" "\\1\n" bytes "${bytes}")

file(WRITE "${OUTPUT}"
"#include <cstddef>
namespace Embedded {
extern const unsigned char ${SYMBOL}[] = {
${bytes}
};
extern const std::size_t ${SYMBOL}Size = ${size};
}
")
