#ifndef COMPOSITION_H
#define COMPOSITION_H

#include <map>
#include <boost/shared_ptr.hpp>

namespace cyclus {

typedef int Iso;

// Represents an immutable nuclear material composition
class Composition {
 public:
  typedef boost::shared_ptr<Composition> Ptr;
  typedef std::map<Iso, double> Vect;

  static Ptr CreateFromAtom(Vect v);
  static Ptr CreateFromMass(Vect v);

  int id();
  const Vect& atom_vect();
  const Vect& mass_vect();

  Ptr Decay(int delta);

  /// record in output database (if not done previously).
  void Record();

 protected:
  typedef std::map<int, Composition::Ptr> Chain;
  typedef boost::shared_ptr<Chain> ChainPtr;

  Composition();

  ChainPtr decay_line_;

 private:
  // This constructor allows the creation of decayed versions of
  // compositions while avoiding extra memory allocations.
  Composition(int prev_decay, ChainPtr decay_line);

  Ptr NewDecay(int delta);

  static int nextId_;

  int id_;
  bool recorded_;
  Vect atomv_;
  Vect massv_;
  int prev_decay_;
};

} // namespace cyclus

#endif
