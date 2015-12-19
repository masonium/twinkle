#pragma once

#include <string>

using std::string;

/*
 * Serves as a base class for all major heap-instantiated objects in twinkle.
 */
class Base
{
  virtual string to_string() const { return "Base"; }
};
