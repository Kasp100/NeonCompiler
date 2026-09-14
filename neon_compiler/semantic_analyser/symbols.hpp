#include <cstdint>
#include <string>
#include "../ast/nodes/nodes.hpp"

namespace neon_compiler::semantic_analyser
{

using MutabilityMode = neon_compiler::ast::nodes::MutabilityMode;

struct VariableID
{
    uint32_t value;
};

struct FunctionID
{
    uint32_t value;
};

struct TypeID
{
    uint32_t value;
};

struct Symbol
{
    std::string name;

	virtual ~Symbol() = default;
};

struct TypeSymbol : Symbol
{
    bool supports_mutable_state;
};

struct VariableSymbol : Symbol
{
    bool mutating_permission;
    MutabilityMode mutability_mode;
    TypeID type;
};

struct FunctionSymbol : Symbol
{
    bool return_mutating_permission;
    MutabilityMode return_mutability_mode;
	TypeID return_type;
    std::vector<VariableID> parameters;
};

}
