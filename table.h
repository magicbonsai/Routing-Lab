#ifndef _table
#define _table
#define INF 0x3FFFFF


#include <iostream>
#include <deque>
#include <map>
#include <set>

class Node;

#include "node.h"
using namespace std;



#if defined(GENERIC)
class Table {
  // Students should write this class

 public:
  ostream & Print(ostream &os) const;
};
#endif


#if defined(LINKSTATE)
class Table {
  // Students should write this class

 public:

  map< unsigned, map< unsigned, double> > t;
  
  Table();
  Table(const Table &table);

  ostream & Print(ostream &os) const;
};
#endif

#if defined(DISTANCEVECTOR)



class Table {



 public:
    map< unsigned, map< unsigned, double> > t;
	  map< unsigned, double > cost;
	  set< unsigned> neighbors;

    Table();
    Table(const Table &table);

    ostream & Print(ostream &os) const;
};
#endif

inline ostream & operator<<(ostream &os, const Table &t) { return t.Print(os);}

#endif
