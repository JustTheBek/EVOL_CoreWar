#include <signal.h>

/* PMARS wants this for X11/curses stepping mode.
   macOS builds do not use it, so we provide a no-op stub. */

void sighandler(int sig)
{
    (void)sig;  // unused
}
