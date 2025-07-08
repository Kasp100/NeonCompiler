#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include "ast_visitor.hpp"

namespace neon_compiler::ast
{

class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor &visitor) = 0;
};

}

#endif // AST_NODE_HPP