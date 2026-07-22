#include "parser.hpp"

#include "expression_parser.hpp"

using namespace neon_compiler;
using namespace neon_compiler::parser;
using namespace neon_compiler::analysis;
using namespace neon_compiler::ast::nodes;

void Parser::run()
{
	parse_and_register_expected_package_declaration();

	while(!reader.end_of_file_reached())
	{
		if(reader.peek().get_type() == TokenType::IMPORT)
		{
			parse_and_register_import_statement();
		}
		else
		{
			const Access access = parse_access(); // `private` if no keyword is present.
			parse_expected_package_member(access);
		}
	}
}

std::shared_ptr<neon_compiler::ast::nodes::Root> Parser::get_root_node() const
{
	return root_node;
}

void Parser::report_token
(
	AnalysisEntryType type,
	AnalysisSeverity severity,
	const Token& token,
	std::optional<std::string> info
)
{
	analysis_reporter->report(AnalysisEntry{file, type, severity, token.get_source_position(), token.get_length(), info});
}

void Parser::append_ast(std::unique_ptr<PackageMember> node, const std::string& identifier)
{
	const std::string full_identifier{package.to_string() + "::" + identifier};

	root_node->file_package_members[std::string{file}].push_back(full_identifier);
	root_node->package_members[full_identifier] = std::move(node);
}

std::optional<neon_compiler::ast::Identifier> Parser::parse_identifier(AnalysisEntryType type, AnalysisSeverity severity)
{
	FuncReportToken func_report_token = [this] (AnalysisEntryType type, AnalysisSeverity severity, const Token& token, std::optional<std::string> info)
	{
		report_token(type, severity, token, info);
	};

	ExpressionParser expression_parser{logger, &reader, &func_report_token, operator_table};
	return expression_parser.parse_identifier(type, severity);
}

void Parser::parse_and_register_expected_package_declaration()
{
	if(reader.peek().get_type() == TokenType::PACKAGE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.peek(),
			std::string{error_messages::MISSING_PACKAGE_DECLARATION});
	}

	std::optional<neon_compiler::ast::Identifier> package_id = parse_identifier(AnalysisEntryType::PACKAGE, AnalysisSeverity::INFO);
	if(!package_id.has_value())
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.peek(),
			std::string{error_messages::MISSING_PACKAGE_DECLARATION});
	}

	if(reader.peek().get_type() == TokenType::END_STATEMENT)
	{
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.peek(),
			std::string{error_messages::MISSING_SEMICOLON});
	}

	package = package_id.value_or(neon_compiler::ast::Identifier{});
}

void Parser::parse_and_register_import_statement()
{
	// At this point, IMPORT should be guaranteed
	report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());

	std::optional<neon_compiler::ast::Identifier> package_member_id = parse_identifier(AnalysisEntryType::REFERENCE, AnalysisSeverity::INFO);

	if(!package_member_id.has_value())
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.peek(),
			std::string{error_messages::INVALID_IMPORT_STATEMENT});
		return;
	}

	imports.push_back(package_member_id.value());
}

Access Parser::parse_access()
{
	Access access{AccessType::PRIVATE};
	if(reader.peek().get_type() == TokenType::ACCESS_PRIVATE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::ACCESS_PROTECTED)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::PROTECTED_PACKAGE_MEMBER});
	}
	else if(reader.peek().get_type() == TokenType::ACCESS_PUBLIC)
	{
		access = Access{AccessType::PUBLIC};
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::ACCESS_EXCLUSIVE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());

		if(reader.peek().get_type() == TokenType::BRACKET_CURLY_OPEN)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
		}
		else
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
				std::string{error_messages::MISSING_PACKAGE_MEMBER_PATTERNS});
		}

		std::vector<PackageMemberPattern> patterns;

		patterns.push_back(parse_package_member_pattern());
		while(reader.peek().get_type() == TokenType::COMMA)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
			patterns.push_back(parse_package_member_pattern());
		}

		access = Access{AccessType::EXCLUSIVE, patterns};

		if(reader.peek().get_type() == TokenType::BRACKET_CURLY_CLOSE)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
		}
		else
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
				std::string{error_messages::INVALID_PACKAGE_MEMBER_PATTERN__EXPECTED_CLOSING_BRACKET});
		}
	}
	return access;
}

PackageMemberPattern Parser::parse_package_member_pattern()
{
	PackageMemberPatternType type = PackageMemberPatternType::PACKAGE_MEMBER;
	if(reader.peek().get_type() == TokenType::SHALLOW)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		type = PackageMemberPatternType::PACKAGE_WITHOUT_SUBPACKAGES;
	}
	else if(reader.peek().get_type() == TokenType::DEEP)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		type = PackageMemberPatternType::PACKAGE_WITH_SUBPACKAGES;
	}

	if(type != PackageMemberPatternType::PACKAGE_MEMBER)
	{
		if(reader.peek().get_type() == TokenType::PACKAGE)
		{
			report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		}
		else
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
				std::string{error_messages::INVALID_PACKAGE_MEMBER_PATTERN__EXPECTED_PKG});
		}
	}

	std::optional<ast::Identifier> package_member_identifier = parse_identifier(AnalysisEntryType::REFERENCE, AnalysisSeverity::INFO);

	if(reader.peek().get_type() != TokenType::INHERITANCE_EXTENDS) { return PackageMemberPattern{type, package_member_identifier}; }
	report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());

	std::optional<ast::Identifier> supertype = parse_identifier(AnalysisEntryType::REFERENCE, AnalysisSeverity::INFO);
	if(supertype.has_value())
	{
		return PackageMemberPattern{package_member_identifier.has_value() ? PackageMemberPatternType::INHERITANCE_ONLY : type,
			package_member_identifier, supertype};
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::MISSING_SECOND_PACKAGE_MEMBER_PATTERN});

		return PackageMemberPattern{type, package_member_identifier};
	}
}

void Parser::parse_expected_package_member(const Access& access)
{
	if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_ENTRYPOINT)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		parse_and_register_expected_entrypoint(access);
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_PURE_FUNCTION_SET)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_OPERATOR_MODULE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		parse_and_register_expected_operator_module(access);
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_COMPILE_FUNCTION)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_CLASS)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::MEMBER_ABSTRACT && reader.peek(1).get_type() == TokenType::PACKAGE_MEMBER_CLASS)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_INTERFACE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_FILE_LEVEL_TOKEN});
	}
}

std::string Parser::parse_expected_declaration_name(AnalysisEntryType analysis_entry_type)
{
	if(reader.peek().get_type() == TokenType::IDENTIFIER)
	{
		std::string name{reader.peek().get_lexeme().value()};
		report_token(analysis_entry_type, AnalysisSeverity::INFO, reader.consume(), name);
		return name;
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_DECLARATION_NAME});

		return std::string{error_recovery::PLACEHOLDER_NAME};
	}
}

void Parser::parse_and_register_expected_entrypoint(const Access& access)
{
	const std::string name = parse_expected_declaration_name(AnalysisEntryType::DECLARATION);

	ParameterDeclarationList parameters = parse_parameter_declarations();

	CodeBlock body = CodeBlock{std::vector<std::unique_ptr<Statement>>{}};

	if(reader.peek().get_type() == TokenType::BRACKET_CURLY_OPEN)
	{
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
		body = parse_code_block_until_end();
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::MISSING_CODE_BLOCK});
	}

	std::unique_ptr<PackageMember> package_member = std::make_unique<Entrypoint>(access, std::move(parameters), std::move(body));

	append_ast(std::move(package_member), name);
}

void Parser::parse_and_register_expected_operator_module(const Access& access)
{
	const std::string name = parse_expected_declaration_name(AnalysisEntryType::DECLARATION);

	if(reader.peek().get_type() == TokenType::BRACKET_CURLY_OPEN)
	{
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::MISSING_CODE_BLOCK});
	}

	std::vector<OperatorDeclaration> operators;
	std::vector<OperatorFunction> functions;

	TokenType token_type = reader.peek().get_type();
	while(!reader.end_of_file_reached() && token_type != TokenType::BRACKET_CURLY_CLOSE)
	{
		if(token_type == TokenType::OPERATOR)
		{
			operators.push_back(parse_expected_operator_declaration());
		}
		else
		{
			functions.push_back(parse_expected_operator_function());
		}
		token_type = reader.peek().get_type();
	}

	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());

	append_ast
	(
		std::make_unique<OperatorModule>(access, std::move(operators), std::move(functions)),
		name
	);
}

OperatorDeclaration Parser::parse_expected_operator_declaration()
{
	// At this point, the `operator` keyword should be guaranteed
	report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());

	std::vector<OperatorSyntaxPatternElement> pattern;
	uint subordination{0};
	OperatorAssociativity associativity{OperatorAssociativity::NONE};

	TokenType token_type = reader.peek().get_type();
	while(!reader.end_of_file_reached() && token_type != TokenType::BRACKET_CURLY_OPEN)
	{
		if(token_type == TokenType::BRACKET_ROUND_OPEN)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());

			if(reader.peek(0).get_type() != TokenType::IDENTIFIER || reader.peek(1).get_type() != TokenType::BRACKET_ROUND_CLOSE)
			{
				report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(), std::string{error_messages::INVALID_OPERATOR_PARAMETER});
			}
			else
			{
				report_token(AnalysisEntryType::DECLARATION, AnalysisSeverity::INFO, reader.peek());
				pattern.push_back(OperatorSyntaxParameter{std::string{reader.consume().get_lexeme().value()}});

				report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
			}
		}
		else
		{
			const Token& token = reader.consume();
			const std::optional<std::string_view>& lexeme = token.get_lexeme();

			if(lexeme.has_value())
			{
				pattern.push_back(TokenPattern{token.get_type(), std::string{lexeme.value()}});
			}
			else
			{
				pattern.push_back(TokenPattern{token.get_type()});
			}
		}
		token_type = reader.peek().get_type();
	}

	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume()); // Consume the `{`

	token_type = reader.peek().get_type();

	while(!reader.end_of_file_reached() && token_type != TokenType::BRACKET_CURLY_CLOSE)
	{
		if(token_type == TokenType::SUBORDINATION)
		{
			report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());

			bool invalid_subord{false};

			if(reader.peek().get_type() == TokenType::LITERAL_NUMBER)
			{
				try
				{
					int subord = std::stoi(std::string{reader.peek().get_lexeme().value()}, nullptr, 0);
					if(subord < 0) { invalid_subord = true; }
					subordination = subord;
				}
				catch(...) { invalid_subord = true; }
			}
			else { invalid_subord = true; }

			if(invalid_subord)
			{
				report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(), std::string{error_messages::INVALID_SUBORDINATION});
			}
			else
			{
				report_token(AnalysisEntryType::LITERAL_NUMBER, AnalysisSeverity::INFO, reader.consume());
			}
		}
		else if(token_type == TokenType::ASSOCIATIVITY)
		{
			report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());

			token_type = reader.peek().get_type();

			if(token_type == TokenType::LEFT)
			{
				report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
				associativity = OperatorAssociativity::LEFT;
			}
			else if(token_type == TokenType::RIGHT)
			{
				report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
				associativity = OperatorAssociativity::RIGHT;
			}
			else
			{
				report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(), std::string{error_messages::INVALID_ASSOCIATIVITY});
			}
		}
		else
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(), std::string{error_messages::INVALID_OPERATOR_PROPERTY});
		}

		if(reader.peek().get_type() != TokenType::END_STATEMENT)
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(), std::string{error_messages::MISSING_SEMICOLON});
		}
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());

		token_type = reader.peek().get_type();
	}

	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume()); // Consume the `}`

	return OperatorDeclaration{std::move(pattern), subordination, associativity, BuiltinOperatorKind::NOT_BUILT_IN};
}

OperatorFunction Parser::parse_expected_operator_function()
{
	std::optional<ReferenceType> return_value = parse_reference_type(MutabilityMode::BORROW);

	if(!return_value.has_value())
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_REFERENCE_TYPE});
	}
	
	std::vector<OperatorFunctionPatternElement> pattern = parse_operator_function_pattern();
	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume()); // Consume the `{`
	CodeBlock body = parse_code_block_until_end();

	return OperatorFunction
	{
		return_value.value_or
		(
			ReferenceType{false, MutabilityMode::BORROW, false, std::string{error_recovery::PLACEHOLDER_TYPE}}
		),
		std::move(pattern),
		std::move(body)
	};
}

std::vector<OperatorFunctionPatternElement> Parser::parse_operator_function_pattern()
{
	std::vector<OperatorFunctionPatternElement> pattern;

	while(!reader.end_of_file_reached() && reader.peek().get_type() != TokenType::BRACKET_CURLY_OPEN)
	{
		if(reader.peek().get_type() == TokenType::BRACKET_ROUND_OPEN)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());

			VariableDeclaration parameter = parse_variable_declaration(MutabilityMode::BORROW)
				.value_or
				(
					VariableDeclaration
					{
						false,
						ReferenceType{false, MutabilityMode::BORROW, false, std::string{error_recovery::PLACEHOLDER_TYPE}},
						std::string{error_recovery::PLACEHOLDER_NAME}
					}
				);

			pattern.push_back(OperatorFunctionParameter{parameter});

			if(reader.peek().get_type() == TokenType::BRACKET_ROUND_CLOSE)
			{
				report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
			}
			else
			{
				report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
					std::string{error_messages::INVALID_PACKAGE_MEMBER_PATTERN_PART__EXPECTED_CLOSING_BRACKET});
			}
		}
		else
		{
			const Token& token = reader.peek();
			std::optional<std::string> lexeme{std::nullopt};

			if(token.get_lexeme().has_value())
			{
				lexeme = std::string{token.get_lexeme().value()};
			}
	
			report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	
			pattern.push_back(TokenPattern{token.get_type(), lexeme});
		}
	}

	return pattern;
}

ParameterDeclarationList Parser::parse_parameter_declarations()
{
	ParameterDeclarationList param_decl_list{};

	if(reader.peek().get_type() == TokenType::BRACKET_ROUND_OPEN)
	{
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
	}
	else
	{
		return param_decl_list;
	}

	bool first{true};

	while(!reader.end_of_file_reached())
	{
		if(reader.peek().get_type() == TokenType::BRACKET_ROUND_CLOSE)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
			break;
		}
		else if(!first)
		{
			if(reader.peek().get_type() == TokenType::COMMA)
			{
				report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
			}
			else
			{
				report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
					std::string{error_messages::INVALID_PARAMETER_DECLARATION});
				continue;
			}
		}

		first = false;

		std::optional<VariableDeclaration> var_decl = parse_variable_declaration(MutabilityMode::BORROW);

		if(!var_decl.has_value())
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
				std::string{error_messages::INVALID_PARAMETER_DECLARATION});
			continue;
		}

		param_decl_list.parameters.push_back(std::move(var_decl.value()));
	}

	return param_decl_list;
}

std::optional<VariableDeclaration> Parser::parse_variable_declaration(MutabilityMode default_mutability_mode)
{
	bool var{reader.peek().get_type() == TokenType::VAR};

	if(var)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}

	std::optional<ReferenceType> ref_type = parse_reference_type(default_mutability_mode);

	std::string ref_name{error_recovery::PLACEHOLDER_NAME};
	if(reader.peek().get_type() == TokenType::IDENTIFIER)
	{
		report_token(AnalysisEntryType::DECLARATION, AnalysisSeverity::INFO, reader.peek());
		ref_name = reader.consume().get_lexeme().value();
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::MISSING_IDENTIFIER});
	}

	if(ref_type.has_value())
	{
		return VariableDeclaration{var, ref_type.value(), ref_name}; // TODO: parse optional expression for initialisation
	}
	else if(var)
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_VARIABLE_DECLARATION});

		ReferenceType valid_ref_type{false, default_mutability_mode, false, std::string{error_recovery::PLACEHOLDER_NAME}};

		return VariableDeclaration{var, valid_ref_type, ref_name};
	}
	else
	{
		return std::nullopt;
	}
}

std::optional<ReferenceType> Parser::parse_reference_type(MutabilityMode default_mutability_mode)
{
	bool opt{reader.peek().get_type() == TokenType::OPTIONAL};

	if(opt)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}

	TokenType tt{reader.peek().get_type()};

	MutabilityMode mm{default_mutability_mode};
	bool implicit_mutability_mode{false};

	switch (tt)
	{
		case TokenType::REF_TYPE_OWN:     mm = MutabilityMode::OWN;         break;
		case TokenType::REF_TYPE_SHARED:  mm = MutabilityMode::SHARED;      break;
		case TokenType::REF_TYPE_BORROW:  mm = MutabilityMode::BORROW;      break;
		default:                          implicit_mutability_mode = true;  break;
	}

	if(!implicit_mutability_mode)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}

	bool mut{reader.peek().get_type() == TokenType::MUT};

	if(mut)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		if(reader.peek().get_type() == TokenType::COLON) { report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume()); }
	}

	if(reader.peek().get_type() == TokenType::IDENTIFIER)
	{
		report_token(AnalysisEntryType::REFERENCE, AnalysisSeverity::INFO, reader.peek());
		std::string type_name{reader.consume().get_lexeme().value()};
		return ReferenceType{opt, mm, mut, type_name};
	}
	else if(opt || mut || !implicit_mutability_mode)
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_REFERENCE_TYPE});

		return ReferenceType{opt, mm, mut, std::string{error_recovery::PLACEHOLDER_NAME}};
	}
	else
	{
		return std::nullopt;
	}
}

CodeBlock Parser::parse_code_block_until_end()
{
	// At this point, a `{` should already be consumed
	std::vector<std::unique_ptr<Statement>> statements;

	while(!reader.end_of_file_reached())
	{
		if(reader.peek().get_type() == TokenType::BRACKET_CURLY_CLOSE)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
			break;
		}
		else if(reader.peek().get_type() == TokenType::STMT_RETURN)
		{
			statements.push_back(std::move(parse_return_statement()));
		}
		else
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume());
		}
	}

	return CodeBlock{std::move(statements)};
}

std::unique_ptr<Statement> Parser::parse_return_statement()
{
	// At this point, STMT_RETURN should be guaranteed
	report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());

	std::unique_ptr<Expression> value = nullptr;
	
	if(reader.peek().get_type() != TokenType::END_STATEMENT)
	{		
		FuncReportToken func_report_token = [this] (AnalysisEntryType type, AnalysisSeverity severity, const Token& token, std::optional<std::string> info)
		{
			report_token(type, severity, token, info);
		};

		ExpressionParser expression_parser{logger, &reader, &func_report_token, operator_table};

		value = expression_parser.parse_expression();
	}

	if(reader.peek().get_type() == TokenType::END_STATEMENT)
	{
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::MISSING_SEMICOLON});
	}

	return std::make_unique<Return>(std::move(value));
}

