#ifndef BIGNUM_HPP
#define BIGNUM_HPP

#include <vector>
#include <string>
#include "Bignum.hpp"
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <random>

class Bignum {
public:
  Bignum(int = 0);
  Bignum(unsigned);
  Bignum(std::string const & s);
  Bignum(Bignum const &);
  Bignum(Bignum &&);

  ~Bignum();

  Bignum & operator=(Bignum const &);
  Bignum & operator=(Bignum &&);

  Bignum & operator+=(Bignum const &);
  Bignum & operator-=(Bignum const &);
  Bignum & operator*=(Bignum const &);
  Bignum & operator/=(Bignum const &);
  Bignum & operator<<=(unsigned int n);
  Bignum & operator>>=(unsigned int n);

  void printHexa() const;
  void resize();


public:
  std::vector<uint32_t> tab;
  bool isPositive;


friend bool operator<(Bignum const &, Bignum const &);
friend bool operator<=(Bignum const &, Bignum const &);
friend bool operator>(Bignum const &, Bignum const &);
friend bool operator>=(Bignum const &, Bignum const &);
friend bool operator==(Bignum const &, Bignum const &);
friend bool operator!=(Bignum const &, Bignum const &);

friend std::ostream & operator<<(std::ostream &, Bignum const &);
friend Bignum operator>>(Bignum const & x, unsigned n) {auto y = x; return y >>= n;};
friend Bignum operator<<(Bignum const & x, unsigned n) {auto y = x; return y <<= n;};

friend Bignum operator+(Bignum const &, Bignum const &);
friend Bignum operator-(Bignum const &, Bignum const &);
friend Bignum operator*(Bignum const &, Bignum const &);
friend Bignum operator/(Bignum const &, Bignum const &);
friend Bignum operator%(Bignum const &, Bignum const &);

friend std::pair<Bignum, Bignum> division(Bignum, Bignum const &);

friend Bignum inverseMod(Bignum const &, Bignum const &);

};


#endif
