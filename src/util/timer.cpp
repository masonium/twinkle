#include <sstream>
#include <iomanip>
#include <cmath>
#include "timer.h"

using std::ostringstream;
using std::setw;
using std::string;
using std::setfill;

string format_duration(double seconds)
{
  ostringstream s;
  int milli = (seconds - static_cast<int>(seconds)) * 1000;

  int time_rem = seconds;

  int secs = time_rem % 60;
  time_rem /= 60;

  int mins = time_rem % 60;
  time_rem /= 60;

  int hours = time_rem % 24;
  time_rem /= 24;

  int days = time_rem;

  bool print_rest = false;
  if (days > 0)
  {
    print_rest = true;
    s << days << "d";
  }
  if (hours > 0 || print_rest)
  {
    print_rest = true;
    s << setfill('0') << setw(2) << hours << "h";
  }
  if (mins > 0 || print_rest)
  {
    print_rest = true;
    s << setfill('0') << setw(2) << mins << "m";
  }
  s << setfill('0') << setw(2) << secs << "." << setfill('0') << setw(3) << milli << "s";
  return s.str();
}
