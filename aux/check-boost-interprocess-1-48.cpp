#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/version.hpp>
#include <iostream>

// See 'misc/interprocess-test.cpp' for explanation.
#if BOOST_VERSION < 104800
#  error need at least Boost 1.48
#endif

int
main ()
{
  // See http://llvm.org/bugs/show_bug.cgi?id=12961 and
  // http://gcc.gnu.org/bugzilla/show_bug.cgi?id=53499.  Whatever is
  // correct, Boost Interprocess offset_ptr doesn't work for us on
  // Clang 3.1 at the moment.  Check for this particular error and
  // disable Interprocess tests if they will fail anyway: it's not an
  // MCT problem.
  boost::interprocess::offset_ptr <int>  a (0), b (0);
  static_cast <void> (a - b);
}
