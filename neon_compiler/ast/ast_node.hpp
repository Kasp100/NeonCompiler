#ifndef AST_NODE_HPP
#define AST_NODE_HPP

namespace neon_compiler::ast
{

class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual void accept(class ASTVisitor &visitor) = 0;
};

}

#endif // AST_NODE_HPP