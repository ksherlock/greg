#include "Node.h"

#include <stack>
#include <vector>
#include <algorithm>

#include <cassert>
    
    // static 
static std::stack<Node *> stack;
static std::vector<Rule *> rules;

static Node *actions = NULL;
static Rule *thisRule = NULL;
static Rule *startRule = NULL;

static int actionCount = 0;
static int ruleCount= 0;
static int lastToken= -1;    

void NodeVisitor::visit(Node &n)
{
}

void NodeVisitor::visit(Rule &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(Variable &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(Name &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(Dot &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(Character &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(String &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(Class &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(Action &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(Predicate &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(Alternate &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(Sequence &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(PeekFor &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(PeekNot &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(Query &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(Star &n)
{
    visit(dynamic_cast<Node &>(n));
}

void NodeVisitor::visit(Plus &n)
{
    visit(dynamic_cast<Node &>(n));
}

// double dispatch so the correct NodeVisitor::visit will be called.
#undef MakeVisitor
#define MakeVisitor(CLASS)\
void CLASS::visit(NodeVisitor& nv){ nv.visit(*this); }


MakeVisitor(Node)
MakeVisitor(Rule)
MakeVisitor(Variable)
MakeVisitor(Name)
MakeVisitor(Dot)
MakeVisitor(Character)
MakeVisitor(String)
MakeVisitor(Class)
MakeVisitor(Action)
MakeVisitor(Predicate)
MakeVisitor(Alternate)
MakeVisitor(Sequence)
MakeVisitor(PeekFor)
MakeVisitor(PeekNot)
MakeVisitor(Query)
MakeVisitor(Star)
MakeVisitor(Plus)

#pragma mark - Node

Node::~Node()
{
}

Node *Node::makeDot()
{
    return new Dot();
}

Node *Node::makeRule(const char *cp)
{
    std::string tmp(cp);
    return new Rule(tmp);
}

Node *Node::makeVariable(const char *cp)
{
    return Variable::find(cp);
}

Node *Node::makePeekFor(Node *e)
{
    return new PeekFor(e);
}

Node *Node::makePeekNot(Node *e)
{
    return new PeekNot(e);
}

Node *Node::makeQuery(Node *e)
{
    return new Query(e);
}

Node *Node::makeStar(Node *e)
{
    return new Star(e);
}

Node *Node::makePlus(Node *e)
{
    return new Plus(e);
}


Node *Node::appendAlternate(Node *node)
{
    // this is not an alternate, so create a new alternate
    // and insert this and node into i.
    Alternate *n;
    
    assert(node);
    assert(node->next == NULL);
    assert(next == NULL);
    
    n = new Alternate();
    n->first = this;
    n->last = node;
    next = node;
    
    return n;
}

Node *Node::appendSequence(Node *node)
{
    // this is not a sequence, so create a new sequence
    // and insert this and node into i.
    Sequence *n;
    
    assert(node);
    assert(node->next == NULL);
    assert(next == NULL);
    
    n = new Sequence();
    n->first = this;
    n->last = node;
    next = node;
    
    return n;
}



void Node::push(Node *node)
{
    assert(node);
    stack.push(node);
}

Node *Node::top()
{
    return stack.top();
}

Node *Node::pop()
{
    Node *tmp = top();
    stack.pop();
    return tmp;
}


Node *Alternate::appendAlternate(Node *node)
{
    // this is an alternate, so just append the node.
    
    assert(node);
    assert(node->next == NULL);
    assert(last);

    last->next = node;
    last = node;
    
    return this;
}

Node *Sequence::appendSequence(Node *node)
{
    // this is a sequence, so just append the node.
    
    assert(node);
    assert(node->next == NULL);
    assert(last);
    
    last->next = node;
    last = node;
    
    return this;
}


#pragma mark - Rule

Rule::Rule(const char *cp) : 
    Node(kRule), 
    name(cp), 
    variables(NULL), 
    expression(NULL), 
    id(++ruleCount), 
    flags(0)
{
    std::replace(name.begin(), name.end(), '-', '_');

    rules.push_back(this);
}

Rule::Rule(const std::string &s) : 
    Node(kRule), 
    name(s), 
    variables(NULL), 
    expression(NULL), 
    id(++ruleCount), 
    flags(0)
{
    std::replace(name.begin(), name.end(), '-', '_');

    rules.push_back(this);
}


Rule *Rule::find(const char *cp)
{
    Rule *r;
    std::string name(cp);
    std::vector<Rule *>::iterator iter;

    std::replace(name.begin(), name.end(), '-', '_');
    
    for (iter = rules.begin(); iter != rules.end(); ++iter)
    {
        r = *iter;
        if (name == r->name) return r;
    }
    
    // make rule
    return new Rule(name);
}

Rule *Rule::begin()
{
    actionCount = 0;
    thisRule = this;
    
    return this;
}

void Rule::setExpression(Node *e)
{
    expression = e;
    if (!startRule || name == "start")
        startRule = this;
}


#pragma mark - Variable

Variable::Variable(const std::string &s) :
    Node(kVariable),
    name(s),
    value(NULL),
    offset(0)
{
}

Variable *Variable::find(const char *cp)
{
    std::string name(cp);
    Variable *v;
    
    for (Node *n = thisRule->variables; n; n = n->next)
    {
        Variable *v = (Variable *)n;
        
        if (v->name == name) return v;
    }
    
    v = new Variable(name);
    v->next = thisRule->variables;
    thisRule->variables = v;
    
    return v;
    
}