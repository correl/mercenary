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
//#include "mirc.h"
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

	mirc_script_engine(MIRCScriptManager *m) : script() {
		manager = m;
		stage = PARSE;
		current_alias = aliases.end();
		current_variable = vars.end();
	}
	
	void handle_alias_definition(char const* str, char const* end) {
		if (stage != PARSE) return;
		
		string s(str, end);
		aliases.insert(s.c_str(), mirc_alias(true));
		current_alias = aliases.find(s.c_str());
	}
	void handle_alias_definition_local(char const* str, char const* end) {
		if (stage != PARSE) return;
		
		string s(str, end);
		aliases.insert(s.c_str(), mirc_alias(false));
		current_alias = aliases.find(s.c_str());
	}
	void close_alias(char const*, char const*) {
		if (stage != PARSE) return;
		
		if (!aliases.empty() && current_alias != aliases.end()) {
			current_alias = aliases.end();
		}
	}
	void store_code(char const* str, char const* end) {
		if (stage != PARSE) return;
		
		string s(str, end);
		if (!aliases.empty() && current_alias != aliases.end()) {
			current_alias->code.append(s.c_str()).append("\n");
		} else {
			script.code.append(s.c_str()).append("\n");
		}
	}
	void call_alias(char const*, char const*) {
		if (stage != EXECUTE) return;
		
	}
	void return_alias(char const*, char const*) {
	}
	void declare_variable(char const* str, char const* end) {
		if (stage != EXECUTE) return;
		string s(str, end);
		vars.insert(s.c_str(), "");
		current_variable = vars.find(s.c_str());
		stack.push(QStringList());
	}
	void assign_variable(char const* str, char const* end) {
		if (stage != EXECUTE) return;
		
		if (current_variable != vars.end()) {
			string s(str, end);
			*current_variable = (!stack.isEmpty() ? stack.pop().join(" ") : "");
			current_variable = vars.end();
		}
	}
	void fetch_variable(char const*, char const*) {
		if (stage != EXECUTE) return;
		
	}
	void append_expression(char const* str, char const *end) {
		if (stage != EXECUTE) return;
		
		string s(str, end);
		QStringList list;
		if (stack.isEmpty()) {
			list << s.c_str();
		} else {
			list = stack.pop();
			list << s.c_str();
		}
		stack.push(list);
	}
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
			s_action v_def ( bind( &mirc_script_engine::declare_variable, self.actions, _1, _2 ) );
			s_action v_assign ( bind( &mirc_script_engine::assign_variable, self.actions, _1, _2 ) );
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
					//|	(ch_p('\\') >> eol_p)
					|	str_p("$&") >> *blank_p >> eol_p
				)
				;
			identifier
				=	alpha_p >> *alnum_p
				;
			string
				=	(	variable
						| alias_function
						| +(graph_p - ch_p(',') - ch_p('(') - ch_p(')'))
					)[e_append]
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
					/*
					>>	(expression - ch_p(','))
					>>	*(	*space
						>>	ch_p(',') >> *space
						>>	(expression - ch_p(','))
						)
					*/
					>> ch_p(')')
					)
				;
			alias_definition
				=	str_p("alias") >> *space
				//>>	!(str_p("-l") >> *space)
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
