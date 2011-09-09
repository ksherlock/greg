#ifndef __NODE_H__
#define __NODE_H__

#include <string>
#include <bitset>
//#include <stack>

class NodeVisitor;


    enum NodeType {
        kUnknown = 0, 
        kRule, 
        kVariable, 
        kName, 
        kDot, 
        kCharacter, 
        kString, 
        kClass, 
        kAction, 
        kPredicate, 
        kAlternate, 
        kSequence, 
        kPeekFor, 
        kPeekNot, 
        kQuery, 
        kStar, 
        kPlus 
    } ;    

class Node {
public:
    


    virtual ~Node();
    
    virtual void visit(NodeVisitor &);
    
    virtual Node *appendAlternate(Node *);
    virtual Node *appendSequence(Node *);
    
    NodeType type() const { return type_; }
    
    static void push(Node *);
    static Node *top();
    static Node *pop();
    

    
    static Node *makeRule(const char *);
    static Node *makeVariable(const char *);
    static Node *makeDot(void);
    //static Node *makeCharacter(int c);
    static Node *makeString(const char *);
    static Node *makeClass(const char *);
    static Node *makeClass(const std::bitset<32> &);
    static Node *makeAction(const char *);
    static Node *makePredicate(const char *);
    static Node *makeAlternate(Node *);
    static Node *makeSequence(Node *);
    static Node *makePeekFor(Node *);
    static Node *makePeekNot(Node *);
    static Node *makeQuery(Node *);
    static Node *makeStar(Node *);
    static Node *makePlus(Node *);
    
    //
    Node *next;
    std::string errorBlock;
    
protected:
    Node(NodeType type = kUnknown): type_(type) {} 
       
private:
    Node& operator=(const Node&);
    
    NodeType type_;
    
};


class Rule : public Node {
public:

    static const NodeType Type = kRule;
    friend class Node;
    virtual void visit(NodeVisitor &);
    
    static Rule *find(const char *);
    Rule *begin();
    void setExpression(Node *expression);


    enum {
        RuleUsed      = 1<<0,
        RuleReached   = 1<<1,
    };


    std::string name;
    Node *variables;
    Node *expression;
    int id;
    int flags;


    private:
    Rule(const char *);
    Rule(const std::string &);
    
};

class Variable : public Node {
public:
    
    static const NodeType Type = kVariable;
    friend class Node;

    virtual void visit(NodeVisitor &);

    static Variable *find(const char *);

    std::string name;
    Node *value;
    int offset;
    
    private:
    Variable(const std::string &);
};

class Name : public Node {
public:

    static const NodeType Type = kName;
    friend class Node;
    virtual void visit(NodeVisitor &);

    Node *rule;
    Node *variable;
};

class Dot : public Node {
public:

    static const NodeType Type = kDot;
    friend class Node;
    Dot() : Node(Type) {} 

    virtual void visit(NodeVisitor &);

};

class Character : public Node {
public:

    static const NodeType Type = kCharacter;
    friend class Node;
    virtual void visit(NodeVisitor &);

    int value;
};

class String : public Node {
public:

    static const NodeType Type = kString;
    friend class Node;
    virtual void visit(NodeVisitor &);

    std::string value;
};

class Class : public Node {
public:

    static const NodeType Type = kClass;
    friend class Node;
    virtual void visit(NodeVisitor &);

    std::bitset<32> value;
    std::string text;
};

class Action : public Node {
public:

    static const NodeType Type = kAction;
    friend class Node;
    virtual void visit(NodeVisitor &);

    std::string text;
    std::string name;
    Node *list;
    Node *rule;
};

class Predicate : public Node {
public:

    static const NodeType Type = kPredicate;
    friend class Node;
    virtual void visit(NodeVisitor &);

    std::string text;
};

class Alternate : public Node {
public:

    static const NodeType Type = kAlternate;
    friend class Node;
    virtual void visit(NodeVisitor &);
    virtual Node *appendAlternate(Node *);

    Node *first;
    Node *last;
};

class Sequence : public Node {
public:

    static const NodeType Type = kSequence;
    friend class Node;
    virtual Node *appendSequence(Node *);
    virtual void visit(NodeVisitor &);

    Node *first;
    Node *last;
};

class PeekFor : public Node {
public:

    static const NodeType Type = kPeekFor;
    friend class Node;
    virtual void visit(NodeVisitor &);
    PeekFor(Node *e) : Node(kPeekFor), element(e) {} 

    Node *element;
};

class PeekNot : public Node {
public:

    static const NodeType Type = kPeekNot;
    friend class Node;
    virtual void visit(NodeVisitor &);
    PeekNot(Node *e) : Node(kPeekNot), element(e) {} 

    Node *element;
};


class Query : public Node {
public:

    static const NodeType Type = kQuery;
    friend class Node;
    virtual void visit(NodeVisitor &);
    Query(Node *e) : Node(kQuery), element(e) {} 

    Node *element;
};

class Star : public Node {
public:

    static const NodeType Type = kStar;
    friend class Node;
    virtual void visit(NodeVisitor &);   
    Star(Node *e) : Node(kStar), element(e) {} 

    Node *element;
};

class Plus : public Node {
public:

    static const NodeType Type = kPlus;
    friend class Node;
    virtual void visit(NodeVisitor &);
    Plus(Node *e) : Node(kPlus), element(e) {} 


    Node *element;
};


class NodeVisitor {
    public:
    
    virtual void visit(Node&); // default if others not implemented by child.
    virtual void visit(Rule &);
    virtual void visit(Variable &);
    virtual void visit(Name &);
    virtual void visit(Dot &);
    virtual void visit(Character &);
    virtual void visit(String &);
    virtual void visit(Class &);
    virtual void visit(Action &);
    virtual void visit(Predicate &);
    virtual void visit(Alternate &);
    virtual void visit(Sequence &);
    virtual void visit(PeekFor &);
    virtual void visit(PeekNot &);
    virtual void visit(Query &);
    virtual void visit(Star &);
    virtual void visit(Plus &);

};


#endif