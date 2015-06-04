#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "film.h"
#include "optionparser.h"

using std::string;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;
using std::make_shared;
using std::shared_ptr;

enum OptionNames{UNKNOWN, LINEAR, RSSF};

option::ArgStatus optional_numeric(const option::Option& opt, bool msg)
{
  if (!opt.arg)
    return option::ARG_OK;
  char* endptr;
  strtof(opt.arg, &endptr);
  if (endptr == opt.arg)
    return option::ARG_IGNORE;
  return option::ARG_OK;
}

const option::Descriptor usage[] =
{
  {UNKNOWN, 0, "", "", option::Arg::None, "USAGE: tonemap [--linear/--rssf]\n\n"},
  {LINEAR, 0, "l", "linear", option::Arg::None, ""},
  {RSSF, 0, "r", "rssf", optional_numeric, "x"},
  {UNKNOWN, 0, 0, 0, 0, 0}
};

int main(int argc, char** argv)
{
  argc -= (argc > 0);
  argv += (argc > 0);
  option::Stats stats(usage, argc, argv);
  option::Option options[stats.options_max], buffer[stats.buffer_max];

  option::Parser parse(usage, argc, argv, options, buffer);

  assert(parse.nonOptionsCount() > 0);
  string filename = parse.nonOption(0);
  cerr << "Reading from " << filename << endl;

  ifstream f(filename, std::ios::in | std::ios::binary);
  if (!f)
  {
    cerr << "Could not read from " << filename << endl;
    exit(1);
  }

  Film film(f);

  shared_ptr<ToneMapper> tonemap = make_shared<LinearToneMapper>();
  if (options[LINEAR])
  {
    cerr << "Using linear tonemapper" << endl;
    tonemap = make_shared<LinearToneMapper>();
  }
  else if (options[RSSF])
  {
    scalar mid = 0.18;
    if (options[RSSF].arg)
      mid = atof(options[RSSF].arg);
    //tonemap = make_shared<RSSFToneMapper>(mid);
    tonemap = shared_ptr<ToneMapper>(new CompositeToneMapper({make_shared<RSSFToneMapper>(mid), 
      make_shared<LinearToneMapper>()}));
    cerr << "Using RSSF tonemapper with midscale of " << mid << endl;
  }
  else
  {
    cerr << "default to linear tonemapper." << endl;
  }

  film.render_to_ppm(cout, tonemap);
}


// int main()
// {
//   spectrum s{2.0, 3.0, 1.0};

//   cout << "color: " << s << endl;
//   cout << "old luminance: " << s.luminance() << endl;
//   spectrum n = s.rescale_luminance(1.0);
//   cout << "new spectrum: " << n << endl;
//   cout << "new luminance: " << n.luminance() << endl;
// }
