//
// header.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HEADER_H
#define HEADER_H

#include <string>

namespace http {
namespace server {

class header
{
  public:
    std::string name;
    std::string value;
};

} // namespace server
} // namespace http

#endif // HEADER_H