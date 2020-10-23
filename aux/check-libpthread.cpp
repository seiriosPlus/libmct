// Check a function that can be found in 'libpthread.a'.  On some
// systems adding the library is not needed, on others it is.
#include <pthread.h>

int
main ()
{
  pthread_join (0, 0);
}
