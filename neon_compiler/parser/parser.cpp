#include "parser.hpp"

using namespace neon_compiler;
using namespace neon_compiler::parser;
using namespace neon_compiler::analysis;
using namespace neon_compiler::ast::nodes;

Parser::Parser
(
	std::shared_ptr<logging::Logger> init_logger,
	std::span<const Token> init_tokens,
	std::shared_ptr<AnalysisReporter> init_analysis_reporter,
	std::shared_ptr<Root> init_root_node,
	std::string_view init_file,
	std::shared_ptr<OperatorMap> init_operator_map
) :
	logger{init_logger},
	reader{init_tokens},
	analysis_reporter{init_analysis_reporter},
	root_node{init_root_node},
	file{init_file},
	operator_map{init_operator_map}
{}

void Parser::run_a()
{
	parse_and_register_package_declaration();

	while(!reader.end_of_file_reached())
	{
		TokenType token_type = reader.peek().get_type();

		if(token_type == TokenType::IMPORT)
		{
			skip_until_statement_end();
			continue;
		}

		if(token_type == TokenType::STMT_USE)
		{
			skip_until_statement_end();
			continue;
		}

		const Access access = parse_access();

		token_type = reader.consume().get_type();

		if(token_type == TokenType::PACKAGE_MEMBER_OPERATOR_MODULE)
		{
			// Token reported in `run_b`
			parse_operator_module_a_and_register_after_keyword(access);
		}
		else
		{
			skip_until_block_start();
			skip_until_block_end();
		}
	}

	reader.reset();
}

void Parser::run_b(std::shared_ptr<OperatorTable> operator_table)
{
	skip_until_statement_end();

	while(!reader.end_of_file_reached())
	{
		const TokenType token_type = reader.peek().get_type();

		if(token_type == TokenType::IMPORT)
		{
			report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
			parse_and_register_import_statement_after_keyword();
			continue;
		}

		if(token_type == TokenType::STMT_USE)
		{
			report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
			operator_table = parse_use_statement_after_keyword_and_create_operator_table(operator_table);
			continue;
		}

		const Access access = parse_access(); // `private` if no keyword is present.

		parse_package_member(access, operator_table);
	}
}

std::shared_ptr<neon_compiler::ast::nodes::Root> Parser::get_root_node() const
{
	return root_node;
}

void Parser::skip_until_statement_end()
{
	while(!reader.end_of_file_reached())
	{
		if(reader.consume().get_type() == TokenType::END_STATEMENT)
		{
			return;
		}
	}
}

void Parser::skip_until_block_start()
{
	while(!reader.end_of_file_reached())
	{
		if(reader.consume().get_type() == TokenType::BRACKET_CURLY_OPEN)
		{
			return;
		}
	}
}

void Parser::skip_until_block_end()
{
	while(!reader.end_of_file_reached())
	{
		const TokenType token_type = reader.consume().get_type();

		if(token_type == TokenType::BRACKET_CURLY_OPEN)
		{
			skip_until_block_end();
		}
		else if(token_type == TokenType::BRACKET_CURLY_CLOSE)
		{
			return;
		}
	}
}

void Parser::report_error_until_member_declaration_end(const std::string& error_message)
{
	TokenType token_type;

	do
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(), error_message);

		token_type = reader.peek().get_type();

		if(token_type == TokenType::END_STATEMENT)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
			return;
		}
	}
	while(!reader.end_of_file_reached() && token_type == TokenType::BRACKET_CURLY_OPEN);

	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());

	uint depth{1};

	while(!reader.end_of_file_reached())
	{
		token_type = reader.peek().get_type();

		if(token_type == TokenType::BRACKET_CURLY_OPEN)
		{
			++depth;
		}
		else if(token_type == TokenType::BRACKET_CURLY_CLOSE)
		{
			--depth;
			if(depth == 0) { break; }
		}

		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(), error_message);
	}

	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
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

std::string Parser::append_ast(std::unique_ptr<PackageMember> node, const std::string& identifier)
{
	std::string full_identifier{package.to_string() + "::" + identifier};

	root_node->file_package_members[std::string{file}].push_back(full_identifier);
	root_node->package_members[full_identifier] = std::move(node);

	logger->info("Appended to AST: " + full_identifier);

	return full_identifier;
}

std::shared_ptr<OperatorTable> Parser::parse_use_statement_after_keyword_and_create_operator_table(std::shared_ptr<OperatorTable> previous)
{
	std::shared_ptr<OperatorTable> new_operator_table = std::make_shared<OperatorTable>();

	const std::vector<std::shared_ptr<const Operator>>* found_operators = parse_use_statement_after_keyword();

	if(!found_operators) { return new_operator_table; }

	for(std::shared_ptr<const Operator> op : *found_operators)
	{
		new_operator_table->add(op);
	}

	new_operator_table->add_all(previous);

	return new_operator_table;
}

const std::vector<std::shared_ptr<const Operator>>* Parser::parse_use_statement_after_keyword()
{
	std::optional<neon_compiler::ast::Identifier> opt_id = parse_identifier(AnalysisEntryType::REFERENCE, AnalysisSeverity::INFO);

	if(!opt_id.has_value())
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_USE_STATEMENT_ARGUMENT});
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

	if(!opt_id.has_value()) { return nullptr; }

	const neon_compiler::ast::Identifier& id = opt_id.value();

	std::string id_str = id.to_string();

	if(id.parts.size() == 1)
	{
		if(imports.contains(id_str))
		{
			id_str = imports[id_str];
		}
		else
		{
			id_str = package.to_string() + "::" + id_str;
		}
	}

	if(!operator_map->contains(id_str))
	{
		logger->info("Could not find operators: " + id_str);
		return nullptr;
	}

	return &(*operator_map)[id_str];
}

std::optional<neon_compiler::ast::Identifier> Parser::parse_identifier(AnalysisEntryType id_type, AnalysisSeverity id_severity)
{
	FuncReportToken func_report_token = [this] (AnalysisEntryType type, AnalysisSeverity severity, const Token& token, std::optional<std::string> info)
	{
		report_token(type, severity, token, info);
	};

	ExpressionParser expression_parser{logger, &reader, &func_report_token, nullptr};

	return expression_parser.parse_identifier(id_type, id_severity);
}

void Parser::parse_and_register_package_declaration()
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

void Parser::parse_and_register_import_statement_after_keyword()
{
	std::optional<neon_compiler::ast::Identifier> opt_id = parse_identifier(AnalysisEntryType::REFERENCE, AnalysisSeverity::INFO);

	if(!opt_id.has_value())
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_IMPORT_STATEMENT});
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

	if(!opt_id.has_value()) { return; }

	const neon_compiler::ast::Identifier& id = opt_id.value();

	imports[id.parts[id.parts.size() - 1]] = id.to_string();
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
			std::move(package_member_identifier), std::move(supertype)};
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::MISSING_SECOND_PACKAGE_MEMBER_PATTERN});

		return PackageMemberPattern{type, std::move(package_member_identifier)};
	}
}

void Parser::parse_package_member(const Access& access, std::shared_ptr<OperatorTable> operator_table)
{
	if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_ENTRYPOINT)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		parse_and_register_entrypoint_after_keyword(access, operator_table);
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_PURE_FUNCTION_SET)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
        parse_and_register_pure_function_set_after_keyword(access, operator_table);
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_OPERATOR_MODULE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		parse_operator_module_b_after_keyword(operator_table);
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_COMPILE_FUNCTION)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_CLASS)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		parse_and_register_type_after_keyword(access, TypeAbstractionLevel::CLASS, operator_table);
	}
	else if(reader.peek().get_type() == TokenType::MEMBER_ABSTRACT && reader.peek(1).get_type() == TokenType::PACKAGE_MEMBER_CLASS)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		parse_and_register_type_after_keyword(access, TypeAbstractionLevel::ABSTRACT_CLASS, operator_table);
	}
	else if(reader.peek().get_type() == TokenType::PACKAGE_MEMBER_INTERFACE)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		parse_and_register_type_after_keyword(access, TypeAbstractionLevel::INTERFACE, operator_table);
	}
	else
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_FILE_LEVEL_TOKEN});
	}
}

std::string Parser::parse_declaration_name(AnalysisEntryType analysis_entry_type)
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

void Parser::parse_and_register_entrypoint_after_keyword(const Access& access, std::shared_ptr<OperatorTable> operator_table)
{
	const std::string name = parse_declaration_name(AnalysisEntryType::DECLARATION);

	ParameterDeclarationList parameters;

	if(reader.peek().get_type() == TokenType::BRACKET_ROUND_OPEN)
	{
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
		parameters = parse_parameter_declarations_after_opening_bracket(operator_table.get());
	}

	bool io{false};

	if(reader.peek().get_type() == TokenType::IO)
	{
		report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
		io = true;
	}

	CodeBlock body{std::vector<std::unique_ptr<Statement>>{}};

	if(reader.peek().get_type() == TokenType::BRACKET_CURLY_OPEN)
	{
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
		body = parse_code_block_after_opening_bracket(operator_table);
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_ENTRYPOINT__MISSING_CODE_BLOCK});
	}

	std::unique_ptr<PackageMember> package_member = std::make_unique<Entrypoint>(access, std::move(parameters), io, std::move(body));

	append_ast(std::move(package_member), name);
}

void Parser::parse_operator_module_a_and_register_after_keyword(const Access& access)
{
	const std::string name = parse_declaration_name(AnalysisEntryType::DECLARATION);

	if(reader.peek().get_type() == TokenType::BRACKET_CURLY_OPEN)
	{
		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
	}
	else
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_OPERATOR_MODULE__MISSING_CURLY_BRACKETS});
	}

	std::vector<OperatorDeclaration> operators;

	while(!reader.end_of_file_reached())
	{
		TokenType token_type = reader.peek().get_type();

		if(token_type == TokenType::OPERATOR)
		{
			report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
			operators.push_back(parse_operator_declaration_after_keyword());
		}
		else if(token_type == TokenType::BRACKET_CURLY_CLOSE)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
			break;
		}
		else
		{
			reader.consume();
			skip_until_block_start();
			skip_until_block_end();
		}
	}

	std::vector<OperatorDeclaration*> operator_declaration_ptrs;

	for(OperatorDeclaration& op : operators)
	{
		operator_declaration_ptrs.push_back(&op);
	}

	std::string full_id = append_ast
	(
		std::make_unique<OperatorModule>(access, std::move(operators), std::vector<OperatorFunction>{}),
		name
	);

	std::vector<std::shared_ptr<const Operator>> operator_list;

	if(operator_map->contains(full_id))
	{
		operator_list = (*operator_map)[full_id];
	}

	for(OperatorDeclaration* op_decl : operator_declaration_ptrs)
	{
		try
		{
			operator_list.push_back(std::make_shared<Operator>(op_decl));
		}
		catch(const std::invalid_argument& e)
		{
			logger->info("Invalid operator: " + std::string{e.what()});
		}
	}

	(*operator_map)[full_id] = std::move(operator_list);
}

void Parser::parse_operator_module_b_after_keyword(std::shared_ptr<OperatorTable> operator_table)
{
	if(reader.peek(0).get_type() != TokenType::IDENTIFIER || reader.peek(1).get_type() != TokenType::BRACKET_CURLY_OPEN)
	{
		logger->info("Skipped invalid operator module.");
		skip_until_block_start();
		skip_until_block_end();
		return;
	}

	// Read and consume the IDENTIFIER token
	const std::string name{reader.consume().get_lexeme().value()};

	reader.consume(); // Consume `{`

	std::vector<OperatorFunction> functions;

	TokenType token_type = reader.peek().get_type();
	while(!reader.end_of_file_reached() && token_type != TokenType::BRACKET_CURLY_CLOSE)
	{
		if(token_type == TokenType::OPERATOR)
		{
			skip_until_block_start();
			skip_until_block_end();
		}
		else
		{
			functions.push_back(parse_operator_function(operator_table));
		}
		token_type = reader.peek().get_type();
	}

	// Consume `}`
	reader.consume();

	const std::string full_identifier{package.to_string() + "::" + name};

	std::unordered_map<std::string, std::unique_ptr<PackageMember>>::iterator it =
		root_node->package_members.find(full_identifier);

	if(it == root_node->package_members.end())
	{
		logger->error("Could not complete operator module \"" + full_identifier + "\"; could not find by identifier.");
		return;
	}

	OperatorModule* operator_module = dynamic_cast<OperatorModule*>(it->second.get());

	if(!operator_module)
	{
		logger->error("Could not complete operator module; dynamic cast returned nullptr.");
		return;
	}

	operator_module->functions = std::move(functions);
}

OperatorDeclaration Parser::parse_operator_declaration_after_keyword()
{
	std::vector<OperatorSyntaxPatternElement> pattern;
	uint subordination{0};
	OperatorAssociativity associativity{OperatorAssociativity::NONE};

	TokenType token_type = reader.peek().get_type();

	while(!reader.end_of_file_reached())
	{
		if(token_type == TokenType::BRACKET_CURLY_OPEN)
		{
			break;
		}
		else if(token_type == TokenType::EMPTY_PARAMETER)
		{
			report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
			pattern.push_back(OperatorSyntaxParameter{});
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

	// Consume `{`
	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());

	token_type = reader.peek().get_type();

	while(!reader.end_of_file_reached())
	{
		if(token_type == TokenType::BRACKET_CURLY_CLOSE)
		{
			break;
		}
		else if(token_type == TokenType::SUBORDINATION)
		{
			report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());

			bool invalid_subord{false};

			if(reader.peek().get_type() == TokenType::LITERAL_NUMBER)
			{
				try
				{
					int subord = std::stoi(std::string{reader.peek().get_lexeme().value()}, nullptr, 0);
					if(subord < 0) { invalid_subord = true; }
					subordination = static_cast<uint>(subord);
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

	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume()); // Consume `}`

	return OperatorDeclaration{std::move(pattern), subordination, associativity, BuiltinOperatorKind::NOT_BUILT_IN};
}

OperatorFunction Parser::parse_operator_function(std::shared_ptr<OperatorTable> operator_table)
{
	std::optional<ReferenceType> opt_return_value = parse_reference_type(MutabilityMode::BORROW);

	if(!opt_return_value.has_value())
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_REFERENCE_TYPE});
	}

	ReferenceType return_value = opt_return_value.value_or
	(
		ReferenceType{false, MutabilityMode::BORROW, false, std::string{error_recovery::PLACEHOLDER_TYPE}}
	);

	std::vector<OperatorFunctionPatternElement> pattern = parse_operator_function_pattern(operator_table.get());

	std::vector<GenericParameter> generic_parameters = parse_generic_parameters();

	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume()); // Consume the `{`

	CodeBlock body = parse_code_block_after_opening_bracket(operator_table);

	return OperatorFunction
	{
		std::move(return_value),
		std::move(generic_parameters),
		std::move(pattern),
		std::move(body)
	};
}

std::vector<OperatorFunctionPatternElement> Parser::parse_operator_function_pattern(OperatorTable* operator_table)
{
	std::vector<OperatorFunctionPatternElement> pattern;

	TokenType token_type = reader.peek().get_type();

	while(!reader.end_of_file_reached())
	{
		if(token_type == TokenType::BRACKET_CURLY_OPEN)
		{
			break;
		}
		else if(token_type == TokenType::BRACKET_ROUND_OPEN)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());

			VariableDeclaration parameter = parse_variable_declaration(MutabilityMode::BORROW, operator_table).value_or
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
			if(token_type == TokenType::SMALLER_THAN)
			{
				// Break if these are the generic parameters
				uint peek_offset{1};
				token_type = reader.peek(peek_offset).get_type();

				while(token_type != TokenType::END_OF_FILE && token_type != TokenType::GREATER_THAN)
				{
					++peek_offset;
					token_type = reader.peek(peek_offset).get_type();
				}

				++peek_offset;
				token_type = reader.peek(peek_offset).get_type();

				if(token_type == TokenType::BRACKET_CURLY_OPEN) { break; }
			}

			const Token& token = reader.peek();
			std::optional<std::string> lexeme{std::nullopt};

			if(token.get_lexeme().has_value())
			{
				lexeme = std::string{token.get_lexeme().value()};
			}
	
			report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
	
			pattern.push_back(TokenPattern{token.get_type(), lexeme});
		}

		token_type = reader.peek().get_type();
	}

	return pattern;
}

std::vector<GenericParameter> Parser::parse_generic_parameters()
{
	std::vector<GenericParameter> params;

	if(reader.peek().get_type() != TokenType::SMALLER_THAN)
	{
		return params;
	}

	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());

	while(!reader.end_of_file_reached())
	{
		std::string generic_param_type{error_recovery::PLACEHOLDER_TYPE};
		std::string generic_param_name{error_recovery::PLACEHOLDER_NAME};

		TokenType tt = reader.peek().get_type();

		if(tt != TokenType::IDENTIFIER)
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
				std::string{error_messages::INVALID_GENERIC_PARAMETER_TYPE});
		}
		else
		{
			const Token& token = reader.consume();

			generic_param_type = token.get_lexeme().value();

			report_token(AnalysisEntryType::REFERENCE, AnalysisSeverity::INFO, token);
		}

		tt = reader.peek().get_type();

		if(tt != TokenType::IDENTIFIER)
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
				std::string{error_messages::MISSING_IDENTIFIER});
		}
		else
		{
			const Token& token = reader.consume();

			generic_param_name = token.get_lexeme().value();

			report_token(AnalysisEntryType::DECLARATION, AnalysisSeverity::INFO, token);
		}

		std::vector<std::string> supertypes = parse_supertype_list();

		tt = reader.peek().get_type();

		params.emplace_back(std::move(generic_param_type), std::move(generic_param_name), std::move(supertypes));

		if(tt == TokenType::GREATER_THAN)
		{
			break;
		}
		else while(tt != TokenType::COMMA && tt != TokenType::END_OF_FILE)
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
				std::string{error_messages::INVALID_GENERIC_PARAMETER_SEPARATOR});

			tt = reader.peek().get_type();
		}

		report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume()); // Consume `,`
	}

	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume()); // Consume `>`

	return params;
}

std::vector<std::string> Parser::parse_supertype_list()
{
	std::vector<std::string> supertypes;

	TokenType tt = reader.peek().get_type();

	if(tt != TokenType::BRACKET_CURLY_OPEN)
	{
		return supertypes;
	}

	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());

	tt = reader.peek().get_type();

	while(!reader.end_of_file_reached())
	{
		if(tt == TokenType::BRACKET_CURLY_CLOSE)
		{
			break;
		}

		std::optional<neon_compiler::ast::Identifier> opt_id = parse_identifier(AnalysisEntryType::REFERENCE, AnalysisSeverity::INFO);

		if(opt_id.has_value())
		{
			supertypes.push_back(opt_id.value().to_string());
		}
		
		while(!reader.end_of_file_reached())
		{
			tt = reader.peek().get_type();

			if(tt == TokenType::BRACKET_CURLY_CLOSE)
			{
				break;
			}
			else if(tt == TokenType::COMMA)
			{
				report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
				break;
			}

			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
				std::string{error_messages::INVALID_SUPERTYPE_LIST_IDENTIFIER});
		}

		tt = reader.peek().get_type();
	}
	
	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume()); // Consume `}`

	return supertypes;
}

ParameterDeclarationList Parser::parse_parameter_declarations_after_opening_bracket(OperatorTable* operator_table)
{
	ParameterDeclarationList param_decl_list;

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

		std::optional<VariableDeclaration> var_decl = parse_variable_declaration(MutabilityMode::BORROW, operator_table);

		if(!var_decl.has_value())
		{
			report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
				std::string{error_messages::INVALID_PARAMETER_DECLARATION});
			continue;
		}

		param_decl_list.push_back(std::move(var_decl.value()));
	}

	return param_decl_list;
}

std::optional<VariableDeclaration> Parser::parse_variable_declaration(MutabilityMode default_mutability_mode, OperatorTable* operator_table)
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
	else if(ref_type.has_value())
	{
		ref_name = std::move(ref_type->inferred_name);
	}

	if(ref_type.has_value())
	{
		if(reader.peek().get_type() == TokenType::ASSIGN)
		{
			report_token(AnalysisEntryType::OPERATOR, AnalysisSeverity::INFO, reader.consume());
			return VariableDeclaration{var, ref_type.value(), ref_name, parse_expression(operator_table)};
		}
		else
		{
			return VariableDeclaration{var, ref_type.value(), ref_name};
		}
	}
	else if(var)
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_VARIABLE_DECLARATION});

		const std::string name{error_recovery::PLACEHOLDER_NAME};
		const ReferenceType valid_ref_type{false, default_mutability_mode, false, name, name};

		return VariableDeclaration{var, std::move(valid_ref_type), std::move(ref_name)};
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

	const std::optional<neon_compiler::ast::Identifier> opt_type_id =
		parse_identifier(AnalysisEntryType::REFERENCE, AnalysisSeverity::INFO);

	if(opt_type_id.has_value())
	{
		const neon_compiler::ast::Identifier& type_id = opt_type_id.value();
		const std::string type_id_str = type_id.to_string();
		const std::string inferred_name = type_id.parts[type_id.parts.size() - 1];

		std::vector<GenericArgument> generic_args = parse_generic_arguments();

		return ReferenceType{opt, mm, mut, std::move(type_id_str), std::move(inferred_name), std::move(generic_args)};
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

std::vector<GenericArgument> Parser::parse_generic_arguments()
{
	FuncReportToken func_report_token = [this] (AnalysisEntryType type, AnalysisSeverity severity, const Token& token, std::optional<std::string> info)
	{
		report_token(type, severity, token, info);
	};

	ExpressionParser expression_parser{logger, &reader, &func_report_token, nullptr};

	return expression_parser.parse_generic_arguments();
}

CodeBlock Parser::parse_code_block_after_opening_bracket(std::shared_ptr<OperatorTable> operator_table)
{
	std::vector<std::unique_ptr<Statement>> statements;

	while(!reader.end_of_file_reached())
	{
		const TokenType token_type = reader.peek().get_type();

		if(token_type == TokenType::BRACKET_CURLY_CLOSE)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
			break;
		}

		std::unique_ptr<Statement> stmt;

		switch(token_type)
		{
			case TokenType::STMT_RETURN:
			{
				report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
				stmt = parse_return_statement_after_keyword(operator_table.get());
				break;
			}
			case TokenType::STMT_USE:
			{
				report_token(AnalysisEntryType::KEYWORD, AnalysisSeverity::INFO, reader.consume());
				operator_table = parse_use_statement_after_keyword_and_create_operator_table(operator_table);
				break;
			}
			default:
			{
				stmt = parse_discard_expression(operator_table.get());
			}
		}

		if(stmt)
		{
			statements.push_back(std::move(stmt));
		}
		else
		{
			logger->info("Parsing encountered an invalid statement.");
		}
	}

	return CodeBlock{std::move(statements)};
}

std::unique_ptr<Expression> Parser::parse_expression(OperatorTable* operator_table)
{
	FuncReportToken func_report_token = [this] (AnalysisEntryType type, AnalysisSeverity severity, const Token& token, std::optional<std::string> info)
	{
		report_token(type, severity, token, info);
	};

	ExpressionParser expression_parser{logger, &reader, &func_report_token, operator_table};

	return expression_parser.parse_expression();
}

void Parser::parse_end_of_statement_after_expression()
{
	while(!reader.end_of_file_reached() && reader.peek().get_type() != TokenType::END_STATEMENT)
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::MISSING_SEMICOLON_OR_FAILED_TO_PARSE_EXPRESSION});
	}

	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());
}

std::unique_ptr<Statement> Parser::parse_return_statement_after_keyword(OperatorTable* operator_table)
{
	std::unique_ptr<Expression> value = nullptr;

	if(reader.peek().get_type() != TokenType::END_STATEMENT)
	{
		value = parse_expression(operator_table);
	}

	parse_end_of_statement_after_expression();

	return std::make_unique<Return>(std::move(value));
}

std::unique_ptr<Statement> Parser::parse_discard_expression(OperatorTable* operator_table)
{
	// TODO: parse local variable declaration
	std::unique_ptr<Expression> expression = parse_expression(operator_table);

	std::unique_ptr<DiscardExpression> result;

	if(expression)
	{
		result = std::make_unique<DiscardExpression>(std::move(expression));
	}

	parse_end_of_statement_after_expression();

	return result;
}

void Parser::parse_and_register_pure_function_set_after_keyword
(
    const Access& access,
    std::shared_ptr<OperatorTable> operator_table
)
{
	const std::string name = parse_declaration_name(AnalysisEntryType::DECLARATION);

    std::vector<ConstantDeclaration> constants;
	std::unordered_map<std::string, std::vector<PureFunction>> functions;

	if(reader.peek().get_type() != TokenType::BRACKET_CURLY_OPEN)
	{
		report_token(AnalysisEntryType::UNKNOWN, AnalysisSeverity::ERROR, reader.consume(),
			std::string{error_messages::INVALID_PURE_FUNCTION_SET__MISSING_CURLY_BRACKETS});
		return;
	}

	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());

	while(!reader.end_of_file_reached())
	{
		if(reader.peek().get_type() == TokenType::BRACKET_CURLY_CLOSE)
		{
			break;
		}

		Access member_access = parse_access();

		std::optional<ReferenceType> opt_ref_type = parse_reference_type(MutabilityMode::BORROW);

		if(!opt_ref_type.has_value())
		{
			report_error_until_member_declaration_end(std::string{error_messages::INVALID_REFERENCE_TYPE});
			continue;
		}

		const ReferenceType& ref_type = opt_ref_type.value();

		std::string member_name;

		if(reader.peek().get_type() == TokenType::IDENTIFIER)
		{
			member_name = reader.consume().get_lexeme().value();
		}
		else if(reader.peek().get_type() == TokenType::ASSIGN)
		{
			member_name = std::move(ref_type.inferred_name);
		}
		else
		{
			report_error_until_member_declaration_end(std::string{error_messages::MISSING_IDENTIFIER});
			continue;
		}

		if(reader.peek().get_type() == TokenType::ASSIGN)
		{
			report_token(AnalysisEntryType::OPERATOR, AnalysisSeverity::INFO, reader.consume());

			std::unique_ptr<Expression> value = parse_expression(operator_table.get());

			constants.push_back
			(
				ConstantDeclaration
				{
					std::move(member_access),
					std::move(ref_type.type),
					std::move(ref_type.generic_arguments),
					std::move(member_name),
					std::move(value)
				}
			);

			parse_end_of_statement_after_expression();

			continue;
		}

		std::vector<GenericParameter> generic_parameters = parse_generic_parameters();

		if(reader.peek().get_type() == TokenType::BRACKET_ROUND_OPEN)
		{
			report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());

			ParameterDeclarationList params = parse_parameter_declarations_after_opening_bracket(operator_table.get());

			if(reader.peek().get_type() != TokenType::BRACKET_CURLY_OPEN)
			{
				report_error_until_member_declaration_end(std::string{error_messages::INVALID_PURE_FUNCTION__MISSING_CODE_BLOCK});
				continue;
			}

			report_token(AnalysisEntryType::OPERATOR, AnalysisSeverity::INFO, reader.consume());

			CodeBlock body = parse_code_block_after_opening_bracket(operator_table);

			if(!functions.contains(member_name))
			{
				functions[member_name] = std::vector<PureFunction>{};
			}

			functions[member_name].push_back
			(
				PureFunction
				{
					std::move(member_access),
					ref_type,
					std::move(generic_parameters),
					std::move(params),
					std::move(body)
				}
			);

			continue;
		}

		report_error_until_member_declaration_end(std::string{error_messages::INVALID_PURE_FUNCTION_SET_MEMBER});
	}

	report_token(AnalysisEntryType::SEPARATOR, AnalysisSeverity::INFO, reader.consume());

	std::unique_ptr<PackageMember> package_member = std::make_unique<PureFunctionSet>
	(
		std::move(access),
		std::move(constants),
		std::move(functions)
	);

	append_ast(std::move(package_member), name);
}

void Parser::parse_and_register_type_after_keyword
(
    const Access& access,
	TypeAbstractionLevel abstraction_level,
    std::shared_ptr<OperatorTable> operator_table
)
{
	const std::string name = parse_declaration_name(AnalysisEntryType::DECLARATION);
}
