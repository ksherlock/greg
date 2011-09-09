#ifndef __NODE_MATCH_H__
#define __NODE_MATCH_H__

#include <functional>


template<Class Left, Class Right>
void Match(Node *left, Node *right, std::function<void, (Left *, Right *)>& fx)
{
    if (!left || !right) return;
    if (left->type() == Left::type() && right->type() == Right::type())
    {
        fx((Left *)left, (Right *)right);
    }
}


/*
 * something like erlang matching, eg
 * optimize(Class1, Class2) -> class_or(Class1, Class2);
 * optimize(_, _) ->  null.
 *
 *
 * struct {  void operator(Class& n1, Class& n2){ ... } } optimizer;
 * NodeMatch<Class, Class> match(node, node->next, optimizer);
 *
 */

template<Class LeftNode, Class RightNode>
NodeMatch2 : public Visitor {
private:
    LeftNode &lhs;
    typedef function<void, LeftNode&, RightNode&> FX;
    FX &fx;

public:
    NodeMatch2(LeftNode& lhs, Node *r, FX f) : lhs(lhs), fx(f)
    {
        r->visit(*this);
    }
    
    virtual void visit(RightNode &node)
    {
        fx(lhs, node);
    }
    
};


template<Class LeftNode, Class RightNode>
NodeMatch : public Visitor {

private:
    Node *rhs;
    typedef function<void, LeftNode&, RightNode&> FX;
    FX &fx;
        
public:
    NodeMatch(Node *l, Node *r, FX& f) : rhs(r), fx(f)
    {
        if (l && r) l->visit(*this);
    }

    // this will be called when lhs is of type LeftNode
    virtual void visit(LeftNode& node)
    {
        NodeMatch2<LeftNode, RightNode> v(node, fx_);
    }
};

#endif
