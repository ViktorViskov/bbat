#include "src/tray_icon.h"
#include "src/help.h"


int main(int argc, char *argv[]) {
    // Parse arguments
    for (int i = 1; i < argc; i++) {
        // Show help message
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            show_help();
            return 0;
        }
    }

    start(argc, argv);
    return 0;
}