#include "map/drawop/test_draw_light.h"
#include "test_database.h"

int main(void) {
    test_draw_light_on_player();
    test_db_open();
    return 0;
}
