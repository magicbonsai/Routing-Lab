#include "messages.h"


#if defined(GENERIC)
ostream &RoutingMessage::Print(ostream &os) const
{
  os << "RoutingMessage()";
  return os;
}
#endif


#if defined(LINKSTATE)

ostream &RoutingMessage::Print(ostream &os) const
{
  os << "message :" << this->link;
  return os;
}

RoutingMessage::RoutingMessage(const Link &link, const double time)
{
	this->link = Link(link);
  this->time_stamp = time;
}


RoutingMessage::RoutingMessage(const RoutingMessage &rhs)
{
	this->link = Link(rhs.link);
  this->time_stamp = rhs.time_stamp;
}

#endif


#if defined(DISTANCEVECTOR)

ostream &RoutingMessage::Print(ostream &os) const
{
  os << "massage from " << sender << ":";
  for(map<unsigned, double>::const_iterator it = vector.begin(); it != vector.end(); ++it) {
  	os << " " << it->first << ":" << it->second;
  }
  return os;
}


RoutingMessage::RoutingMessage(map<unsigned, double> v, unsigned s)
{
    this->sender = s;
    this->vector = map<unsigned, double>(v);
}


RoutingMessage::RoutingMessage(const RoutingMessage &rhs)
{
	this->sender = rhs.sender;
	this->vector = map<unsigned, double>(rhs.vector);
}

#endif

