#ifndef MIRC_PARSER_H
#define MIRC_PARSER_H

/*
	Mercenary
	
	mIRC Scripting Language Parser
*/

//#define BOOST_SPIRIT_DEBUG
#include <string>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/confix.hpp>
#include <boost/spirit/dynamic/if.hpp>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QStack>

class MIRCScriptManager;

using namespace std;
using namespace boost;
using namespace boost::spirit;

typedef QMap<QString, QString> mirc_variables;

struct mirc_alias {
	mirc_alias(bool _global = true) {
		global = _global;
	}
	bool global;
	QString code;
};

typedef QMap<QString, mirc_alias> mirc_aliases;

enum mirc_engine_stage {
	PARSE,
	EXECUTE
};

class mirc_script_engine : public QObject {
	Q_OBJECT
private:
	MIRCScriptManager *manager;
	mirc_aliases::iterator current_alias;
	mirc_variables::iterator current_variable;
	QStack<QStringList> stack;
public:
	mirc_engine_stage stage;
	mirc_alias script;
	mirc_aliases aliases;
	mirc_variables vars;

	mirc_script_engine(MIRCScriptManager *m);
	
	void handle_alias_definition(char const* str, char const* end);
	void handle_alias_definition_local(char const* str, char const* end);
	void close_alias(char const*, char const*);
	void store_code(char const* str, char const* end);
	void call_alias(char const*, char const*);
	void return_alias(char const*, char const*);
	void declare_variable(char const* str, char const* end);
	void assign_variable(char const* str, char const* end);
	void fetch_variable(char const*, char const*);
	void append_expression(char const* str, char const *end);
};

struct mirc_script : public grammar<mirc_script> {
	mirc_script_engine *actions;
	
	mirc_script( mirc_script_engine *_actions ) {
		actions = _actions;
	}
	
	template <typename ScannerT>
	struct definition {
		rule<ScannerT>	script,
						space,
						identifier,
						string,
						expression,
						expression_group,
						parameters,
						variable,
						assignment,
						alias_action,
						alias_function,
						alias_definition,
						code_line,
						code_block,
						comment;
		
		definition(mirc_script const &self) {
			
			typedef function< void(const char*, const char*) > s_action;
			s_action a_def ( bind( &mirc_script_engine::handle_alias_definition, self.actions, _1, _2 ) );
			s_action l_def ( bind( &mirc_script_engine::handle_alias_definition_local, self.actions, _1, _2 ) );
			s_action a_close ( bind( &mirc_script_engine::close_alias, self.actions, _1, _2 ) );
			s_action a_call ( bind( &mirc_script_engine::call_alias, self.actions, _1, _2 ) );
			s_action a_return ( bind( &mirc_script_engine::return_alias, self.actions, _1, _2 ) );
			s_action v_def ( bind( &mirc_script_engine::declare_variable, self.actions, _1, _2 ) );
			s_action v_assign ( bind( &mirc_script_engine::assign_variable, self.actions, _1, _2 ) );
			s_action v_fetch ( bind( &mirc_script_engine::fetch_variable, self.actions, _1, _2 ) );
			s_action e_append ( bind( &mirc_script_engine::append_expression, self.actions, _1, _2 ) );
			s_action s_code ( bind( &mirc_script_engine::store_code, self.actions, _1, _2 ) );
			
			script
				=	*(
						alias_definition[a_close]
					|	code_block
					) >> end_p
				;
			space
				=	(	blank_p
					|	str_p("$&") >> *blank_p >> eol_p
				)
				;
			identifier
				=	alpha_p >> *alnum_p
				;
			string
				=	(	variable[e_append][v_fetch]
						| alias_function[e_append]
						| (+(graph_p - ch_p(',') - ch_p('(') - ch_p(')')))[e_append]
					)
				;
			expression
				=	string >> *(*space >> string)
				;
			expression_group
				=	expression | expression_group
				;
			parameters
				=	expression >> *(*space >> ch_p(',') >> *space >> expression)
				;
			variable
				=	ch_p('%') >> identifier
				;
			assignment
				=	!(str_p("var") >> *space)
				>>	variable[v_def]
				>>	*space >> ch_p('=') >> *space
				>>	expression[v_assign]
				;
			alias_action
				=	!ch_p('/') >> !ch_p('/')
				>>	identifier >> *space >> parameters
				;
			alias_function
				=	ch_p('$') >> identifier
				>>	!(
						ch_p('(') >> *space
					>>	parameters >> *space
					>> ch_p(')')
					)
				;
			alias_definition
				=	str_p("alias") >> *space
				>>	if_p(str_p("-l") >> *space)[identifier[l_def]].else_p[identifier[a_def]]
				>> *space >> !eol_p
				>>	code_block
				;
			code_line
				=	*space
				>> (	comment
					|	(
							assignment /* Must come first to avoid "var" being caught as an action */
						|	alias_action
						)[s_code]
					)
				>> !eol_p
				;
			code_block
				=	(	*space >> ch_p('{') >> *space >> !eol_p
					>>	(*code_line)
					>>	ch_p('}') >> *space >> !eol_p
					)
				|	code_line
				;
			comment = comment_p(";");
			
			BOOST_SPIRIT_DEBUG_NODE(script);
			BOOST_SPIRIT_DEBUG_NODE(space);
			BOOST_SPIRIT_DEBUG_NODE(identifier);
			BOOST_SPIRIT_DEBUG_NODE(string);
			BOOST_SPIRIT_DEBUG_NODE(expression);
			BOOST_SPIRIT_DEBUG_NODE(variable);
			BOOST_SPIRIT_DEBUG_NODE(assignment);
			BOOST_SPIRIT_DEBUG_NODE(alias_action);
			BOOST_SPIRIT_DEBUG_NODE(alias_function);
			BOOST_SPIRIT_DEBUG_NODE(alias_definition);
			BOOST_SPIRIT_DEBUG_NODE(code_line);
			BOOST_SPIRIT_DEBUG_NODE(code_block);
			BOOST_SPIRIT_DEBUG_NODE(comment);
			BOOST_SPIRIT_DEBUG_NODE(*this);
		}
		rule<ScannerT> const& start() const { return script; }
	};
};

#endif
