// Check a function that can be found in 'librt.a'.  On some systems
// adding the library is not needed, on others it is.
#include <sys/mman.h>

int
main ()
{
  shm_unlink (0);
}
