#include <stdio.h>


// show help message
void show_help() {
    printf("Usage: battery [OPTION]\n");
    printf("Show battery status in system tray.\n");
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message and exit\n");
    printf("  -n=NUM         Set update interval in seconds (default 3, min 1, max 10)\n");
    printf("  -d=/path       Set battery device path (default /sys/class/power_supply/BAT1)\n");
    printf("\n");
    printf("Report bugs to: https://github.com/ViktorViskov/bbat/\n");
}