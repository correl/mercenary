#include <QTextStream>
#include "parser.h"
#include "mirc.h"

mirc_script_engine::mirc_script_engine(MIRCScriptManager *m) : script() {
	manager = m;
	stage = PARSE;
	current_alias = aliases.end();
	current_variable = vars.end();
}

void mirc_script_engine::handle_alias_definition(char const* str, char const* end) {
	if (stage != PARSE) return;
	
	string s(str, end);
	QString alias(s.c_str());
	this->aliases.insert(alias, mirc_alias(true));
	current_alias = this->aliases.find(alias);
}
void mirc_script_engine::handle_alias_definition_local(char const* str, char const* end) {
	if (stage != PARSE) return;
	
	string s(str, end);
	aliases.insert(s.c_str(), mirc_alias(false));
	current_alias = aliases.find(s.c_str());
}
void mirc_script_engine::close_alias(char const*, char const*) {
	if (stage != PARSE) return;
	
	if (!aliases.empty() && current_alias != aliases.end()) {
		current_alias = aliases.end();
	}
}
void mirc_script_engine::store_code(char const* str, char const* end) {
	if (stage != PARSE) return;
	
	string s(str, end);
	if (!aliases.empty() && current_alias != aliases.end()) {
		current_alias->code.append(s.c_str()).append("\n");
	} else {
		script.code.append(s.c_str()).append("\n");
	}
}
void mirc_script_engine::call_alias(char const* str, char const* end) {
	if (stage != EXECUTE) return;
	string s(str, end);
	QStringList params = QString(s.c_str()).split(" ");
	QString alias = params.takeFirst();
	manager->call_alias(alias, stack.top());
}
void mirc_script_engine::return_alias(char const*, char const*) {
}
void mirc_script_engine::declare_variable(char const* str, char const* end) {
	if (stage != EXECUTE) return;
	string s(str, end);
	vars.insert(s.c_str(), "");
	current_variable = vars.find(s.c_str());
	stack.push(QStringList());
}
void mirc_script_engine::assign_variable(char const* str, char const* end) {
	if (stage != EXECUTE) return;
	
	if (current_variable != vars.end()) {
		string s(str, end);
		*current_variable = (!stack.isEmpty() ? stack.pop().join(" ") : "");
		manager->variable(current_variable.key(), current_variable.value());
		current_variable = vars.end();
	}
}
void mirc_script_engine::fetch_variable(char const*, char const*) {
	if (stage != EXECUTE) return;
	
	if (!stack.isEmpty()) {
		QStringList values = stack.pop();
		if (!values.isEmpty()) {
			QString var = values.last();
			values.removeLast();
			values << (manager->hasVariable(var) ? manager->variable(var) : vars[var]);
		}
		stack.push(values);
	}
}
void mirc_script_engine::append_expression(char const* str, char const *end) {
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

void mirc_script_engine::clear_stack(char const*, char const*) {
	stack.clear();
}
