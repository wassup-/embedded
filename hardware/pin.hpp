#ifndef HARDWARE_PIN_HPP_
#define HARDWARE_PIN_HPP_

#include "../memory/observer_ptr.hpp"

#include <memory>
#include <type_traits>

struct analog_tag { };
struct digital_tag { };

enum pin_mode_e
{
  pin_output = 0,
  pin_input,
  pin_pullup,
};

extern pin_mode_e mode(analog_tag, int, pin_mode_e);
extern pin_mode_e mode(digital_tag, int, pin_mode_e);

template<typename /* Tag */, int /* Voltage */, int /* Bits */>
class basic_pin;

template<typename>
struct is_pin : std::false_type { };

template<typename Tag, int Voltage, int Bits>
struct is_pin<basic_pin<Tag, Voltage, Bits>> : std::true_type { };

template<typename Tag, int Voltage, int Bits>
class basic_pin
{
public:
  typedef basic_pin this_type;
  typedef int handle_type;
  typedef Tag tag_type;

public:
  basic_pin(handle_type p)
  : pin_(p)
  { }

  basic_pin(handle_type p, pin_mode_e m)
  : pin_(p)
  { mode(m); }

  handle_type handle() const { return pin_; }

  pin_mode_e mode() const { return mode_; }
  void mode(pin_mode_e m) { mode_ = ::mode(Tag(), pin_, m); }

  int read()
  {
    return ::read(Tag(), pin_);
  }

  int write(int v)
  {
    return ::write(Tag(), pin_, v);
  }

public:
  friend int read(basic_pin &self)
  {
    return self.read();
  }

  friend int write(basic_pin &self, int v)
  {
    return self.write(v);
  }

private:
  handle_type pin_;
  pin_mode_e mode_;
};

class any_pin
{
public:
  template<typename Pin>
  any_pin(Pin &pin)
  : self_(new holder<Pin>(pin))
  { }

  int read()
  {
    return self_->read();
  }

  int write(int v)
  {
    return self_->write(v);
  }

public:
  friend int read(any_pin &self)
  {
    return self.read();
  }

  friend int write(any_pin &self, int v)
  {
    return self.write(v);
  }

private:
  struct impl
  {
    virtual int read() = 0;
    virtual int write(int) = 0;
  };

  template<typename Pin>
  struct holder : impl
  {
  public:
    holder(cxx::observer_ptr<Pin> ptr)
    : self_(ptr)
    { }

    int read() { return ::read(*self_); }
    int write(int v) { return ::write(*self_, v); }

  private:
    cxx::observer_ptr<Pin> self_;
  };

  std::unique_ptr<impl> self_;
};

#endif
