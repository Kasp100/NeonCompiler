#ifndef NODES_HPP
#define NODES_HPP

#include <string>
#include <unordered_map>
#include "../ast_node.hpp"

namespace neon_compiler::ast::nodes
{

struct Root : ASTNode
{
    std::unordered_map<std::string, PackageMember> members;

    void accept(ASTVisitor& visitor) override
    {
        visitor.visit(*this);
    }
};

struct PackageMember : ASTNode {};

struct Type : PackageMember
{
    void accept(ASTVisitor& visitor) override
    {
        visitor.visit(*this);
    }
};

struct PureFunctionSet : PackageMember
{
    void accept(ASTVisitor& visitor) override
    {
        visitor.visit(*this);
    }
};

struct GrammarSet : PackageMember
{
    void accept(ASTVisitor& visitor) override
    {
        visitor.visit(*this);
    }
};

struct CompileFunction : PackageMember
{
    void accept(ASTVisitor& visitor) override
    {
        visitor.visit(*this);
    }
};

}

#endif // NODES_HPP