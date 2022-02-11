#include <assert.h>

int main(int argc, char **argv) {
    int side_effect = -1;
    assert(side_effect++);
    return side_effect;
}
