#include "node.h"
#include "context.h"
#include "error.h"

template <typename Map>
bool map_compare(Map const &lhs, Map const &rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}


Node::Node(const unsigned n, SimulationContext *c, double b, double l) :
    number(n), context(c), bw(b), lat(l)
{

#if defined(DISTANCEVECTOR)
    table = new Table();
    table->cost[this->number] = 0;

    table->t[this->number] = map<unsigned, double>();
    table->t[this->number][this->number] = 0;
#endif

#if defined(LINKSTATE)
    table = new Table();
    message_stack = map<unsigned, map<unsigned, double> >();
#endif


}

Node::Node()
{ throw GeneralException(); }

Node::Node(const Node &rhs) :
  number(rhs.number), context(rhs.context), bw(rhs.bw), lat(rhs.lat) {}

Node & Node::operator=(const Node &rhs)
{
  return *(new(this)Node(rhs));
}

void Node::SetNumber(const unsigned n)
{ number=n;}

unsigned Node::GetNumber() const
{ return number;}

void Node::SetLatency(const double l)
{ lat=l;}

double Node::GetLatency() const
{ return lat;}

void Node::SetBW(const double b)
{ bw=b;}

double Node::GetBW() const
{ return bw;}

Node::~Node()
{}


// Implement these functions  to post an event to the event queue in the event simulator
// so that the corresponding node can recieve the ROUTING_MESSAGE_ARRIVAL event at the proper
void Node::SendToNeighbors(const RoutingMessage *m)
{
    context->SendToNeighbors(this, m);
}

void Node::SendToNeighbor(const Node *n, const RoutingMessage *m)
{
    context->SendToNeighbor(this, n, m);
}

deque<Node*> *Node::GetNeighbors()
{
  return context->GetNeighbors(this);
}


void Node::SetTimeOut(const double timefromnow)
{
  context->TimeOut(this,timefromnow);
}


bool Node::Matches(const Node &rhs) const
{
  return number==rhs.number;
}


#if defined(GENERIC)
void Node::LinkHasBeenUpdated(const Link *l)
{
  cerr << *this << " got a link update: "<<*l<<endl;
  //Do Something generic:
  SendToNeighbors(new RoutingMessage);
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  cerr << *this << " got a routing messagee: "<<*m<<" Ignored "<<endl;
}


void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(const Node *destination) const
{
  return 0;
}

Table *Node::GetRoutingTable() const
{
  return new Table;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")";
  return os;
}

#endif

#if defined(LINKSTATE)


void Node::LinkHasBeenUpdated(const Link *l)
{
  if (table->t.count(l->GetSrc()) == 0){
    table->t[l->GetSrc()] = map < unsigned, double>();
    table->t[l->GetSrc()][l->GetDest()] = l->GetLatency();
    SendToNeighbors(new RoutingMessage(*l, context->GetTime()));

    if (l->GetSrc() == number) {
      message_stack[number][l->GetDest()] = context->GetTime(); 
      cerr << *this<<"\n\tLink Add: "<<*l<<endl;
    }

  } else {
    if (table->t[l->GetSrc()].count(l->GetDest()) == 0 
        || table->t[l->GetSrc()][l->GetDest()] != l->GetLatency()){

      table->t[l->GetSrc()][l->GetDest()] = l->GetLatency();
      SendToNeighbors(new RoutingMessage(*l, context->GetTime()));

      if (l->GetSrc() == number) {
        message_stack[number][l->GetDest()] = context->GetTime(); 
        cerr << *this<<"\n\tLink Update: "<<*l<<endl;
      }
    }
  }

}

void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{

  

  if( message_stack.count(m->link.GetSrc()) == 1 &&
      message_stack[m->link.GetSrc()].count(m->link.GetDest()) == 1 &&
      m->time_stamp <= message_stack[m->link.GetSrc()][m->link.GetDest()]
      ) return;

  message_stack[m->link.GetSrc()][m->link.GetDest()] = m->time_stamp;  
  this->LinkHasBeenUpdated(&(m->link));

  //cerr << *this << " Routing Message: "<<*m;
}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(const Node *destination) const
{
  if (table->t.count(number) == 0) return (Node*)-1;
  map< unsigned, double > D = map< unsigned, double >(table->t[number]);
  
  map< unsigned, double> visited = map< unsigned, double>();
  unsigned current = number;

  map< unsigned, unsigned > pre = map< unsigned, unsigned>();
  for(map< unsigned, double>::iterator it = D.begin(); it != D.end(); ++it) {
    pre[it->first] = current;
  }

  while (D.size() > 0){
    double shortest = INF;
    unsigned next_node = -1;
    for(map< unsigned, double>::iterator it = D.begin(); it != D.end(); ++it) {
      if (it->second < shortest) {
        shortest = it->second;
        next_node = it->first;
      }
    }

    visited[next_node] = D[next_node];

    if (table->t.count(next_node) == 1) {
      for(map< unsigned, double>::iterator it = table->t[next_node].begin(); 
          it != table->t[next_node].end(); ++it) {
        
        if (visited.count(it->first) == 1) continue;

        if (D.count(it->first) == 1 
          && D[it->first] <= D[next_node] + table->t[next_node][it->first]) continue;

        D[it->first] = D[next_node] + table->t[next_node][it->first];
        pre[it->first] = next_node;
      }
    }

    D.erase(next_node);
  }

  if (pre.count(destination->number) == 0) return (Node*)-1;

  unsigned ans = destination->number;
  while (pre[ans] != number) ans = pre[ans];
  return new Node(ans, 0, 0, 0);
}

Table *Node::GetRoutingTable() const
{
  return new Table(*table);
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw;
  os << *table << ")";
  return os;
}
#endif

#if defined(DISTANCEVECTOR)
bool Node::update_table()
{
    set<unsigned> nodes;
    nodes.insert(this->number);

    for(set<unsigned>::iterator i = this->table->neighbors.begin(); i !=  this->table->neighbors.end(); ++i) {
        if (table->cost.count(*i) == 1 ) nodes.insert(*i);
        if (this->table->t.count(*i) == 1) {
            for(map<unsigned, double>::iterator node = this->table->t[*i].begin(); node != this->table->t[*i].end(); ++node){
                nodes.insert(node->first);
            }
        }
    }


    map<unsigned, double > new_vector;

    for(set<unsigned>::iterator y = nodes.begin(); y != nodes.end(); ++y){   // for each y
       unsigned shortest = INF;
       if (table->cost.count(*y) == 1 ) shortest = table->cost[*y];

       for(set<unsigned>::iterator neighbor = table->neighbors.begin(); neighbor !=  table->neighbors.end(); ++neighbor){ // for each v
            if (table->t.count(*neighbor) == 1 && table->t[*neighbor].count(*y) == 1){
                if (shortest > table->cost[*neighbor] + table->t[*neighbor][*y])  shortest = table->cost[*neighbor] + table->t[*neighbor][*y];
            }
       }
       if (shortest > INF) shortest = INF;
       new_vector[*y] = shortest;
    }

    if (map_compare(table->t[this->number], new_vector)) {
        // cout<< "Yes" << endl;
        return false;
    } else {
        table->t[this->number] = new_vector;
        // cout<< "No" << endl;
        return true;
    }
}


void Node::LinkHasBeenUpdated(const Link *l)
{
  // update our table

  table->cost[l->GetDest()] = l->GetLatency();
  table->neighbors.insert(l->GetDest());

  // send out routing mesages
  
  if( update_table()) {
      SendToNeighbors(new RoutingMessage(table->t[this->number], this->number));
  }

  cerr << *this<<"\n\tLink Update: "<<*l<<endl;
}




void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
    table->neighbors.insert(m->sender);
    if ( table->t.count(m->sender) == 0 || map_compare(table->t[m->sender], m->vector) == false ){
        table->t[m->sender] = m->vector;

        if( update_table()) {
            SendToNeighbors(new RoutingMessage(table->t[this->number], this->number));
        }
    }

    // cerr << *this << "\n\tRouting Message: "<<*m <<"\n";
}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}


Node *Node::GetNextHop(const Node *destination) const
{
    unsigned y = destination->number;
    unsigned shortest = INF;
    unsigned nexthop = -1;

    if (table->cost.count(y) == 1 ) {
        shortest = table->cost[y];
        nexthop = y;
    }

    for(set<unsigned>::iterator neighbor = table->neighbors.begin(); neighbor !=  table->neighbors.end(); ++neighbor){ // for each v
        if (table->t.count(*neighbor) == 1 && table->t[*neighbor].count(y) == 1){
            if (shortest > table->cost[*neighbor] + table->t[*neighbor][y]){
                shortest = table->cost[*neighbor] + table->t[*neighbor][y];
                nexthop = *neighbor;
            }

        }
    }

    deque<Node*>* neighbors = context->GetNeighbors(this);

    for (deque<Node*>::iterator it = neighbors->begin(); it != neighbors->end() ; ++it){
        if ((*it)->number == nexthop) return new Node(*(*it));
    }
    return (Node*)-1;
}

Table *Node::GetRoutingTable() const
{
    return new Table(*table);
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw;
  os << *table << ")";
  return os;
}
#endif
