#include "table.h"
#include <map>
#include <set>

#if defined(GENERIC)
ostream & Table::Print(ostream &os) const
{
  // WRITE THIS
  os << "Table()";
  return os;
}
#endif

#if defined(LINKSTATE)
Table::Table(){
  t = map< unsigned, map< unsigned, double> >();
}

Table::Table(const Table &table){
  t = map< unsigned, map< unsigned, double> >(table.t);
}

ostream & Table::Print(ostream &os) const
{
  // t
  os << "\n\tTable size: " << t.size();
  for (map<unsigned, map<unsigned, double> >::const_iterator it = t.begin(); it != t.end(); ++it ){
      os << "\n";
      os << "\t\t" << it->first << ":";
      for( map<unsigned, double>::const_iterator vt = it->second.begin(); vt != it->second.end(); ++vt){
        os << " " << vt->first << ":" << vt->second;
      }
  }

  return os;
}

#endif

#if defined(DISTANCEVECTOR)

Table::Table() {
    t = map< unsigned, map< unsigned, double> >();
    cost = map < unsigned, double > ();
    neighbors = set < unsigned> ();
}

Table::Table(const Table &table){
    t = map< unsigned, map< unsigned, double> >(table.t);
    cost = map < unsigned, double > (table.cost);
    neighbors = set < unsigned> (table.neighbors);
}



ostream & Table::Print(ostream &os) const
{
  os << "\n\tTable:\t";
  // neighbors
  os << "Neighbors_size: " << neighbors.size() << ":";
  for (set<unsigned>::iterator it = neighbors.begin(); it != neighbors.end(); ++it) {
    os << " " << *it;
  }
  os << "||";
  // cost
  os << "Cost_size: " << cost.size() << " ";
  for (map<unsigned, double>::const_iterator it = cost.begin(); it != cost.end(); ++it) {
    os << " " << it->first << ":" << it->second;
  }
  os << "||";
  // t
  os << "table_size: " << t.size();
  for (map<unsigned, map<unsigned, double> >::const_iterator it = t.begin(); it != t.end(); ++it ){
      os << "\n";
      os << "\t\t" << it->first << ":";
      for( map<unsigned, double>::const_iterator vt = it->second.begin(); vt != it->second.end(); ++vt){
        os << " " << vt->first << ":" << vt->second;
      }
  }

  return os;
}

#endif
