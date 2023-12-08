#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <memory>

using namespace std;

class Value {
public:
    using ptr = std::shared_ptr<Value>;
    using const_ptr = std::shared_ptr<const Value>;
    virtual ~Value() noexcept = default;

    virtual std::ostream& print(std::ostream& out) const = 0; // `= 0` makes this a pure virtual method (aka. abstract - it has no implementation)
};

class Expression {
public:
    using ptr = std::shared_ptr<Expression>;
    using const_ptr = std::shared_ptr<const Expression>;
    using context = std::unordered_map<std::string, Expression::ptr>;

    virtual Value::ptr evaluate(const context& ctx) const = 0;
    virtual ~Expression() noexcept = default; // `noexcept` is implicit in destructors
};

class Constant final : public Expression {
public:
    explicit Constant(Value::ptr value) : value_(std::move(value)) {}

    Value::ptr evaluate(const context&) const override {
        return value_;
    }

    shared_ptr<Value> &&value_;
};



std::vector<std::string> split(string s, const std::string& delimiter) {
    s += delimiter; //bug fix
    std::vector<std::string> tokens;
    std::string token;
    size_t pos;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0,pos);
        tokens.push_back(token);
        s = s.substr(pos + delimiter.length(), s.length());
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

string stripSpaces(string& str){
    string result;
    bool first = true;
    bool wasSpace = false;
    for (char c : str) {
        if (c != ' ') {
            result += c;
            wasSpace = false;
        } else if (!wasSpace && !first) {
            result += c;
            wasSpace = true;
        }
        first = false;
    }
    if(result[result.length()-1] == ' '){
        result = result.substr(0, result.length()-1);
    }
    return result;
}
bool contains(string& str, string substr){

    istringstream iss(str);
    string word;
    while(!(iss >>word).eof()){
        if(word == substr){
            return true;
        }
    }
    return false;
}



class Object  {
public:
    Object(string name, int args) : name(std::move(name)), argCount(args) {}
    string name;
    int argCount;
};

template <typename T, typename L> class Rule {
public:
    Rule(T lhs, L rhs) : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    T getLhs() const {
        return lhs;
    }
    L getRhs() const {
        return rhs;
    }

private:
    T lhs;
    L rhs;
};

template <typename T, typename L> class Rules {
public:
    Rules() = default;
    explicit Rules(vector<Rule<T,L>> rules) : rules(std::move(rules)) {}

    void addRule(Rule<T,L> rule){
        rules.push_back(rule);
    }
    void addRule(T lhs, L rhs){
        rules.push_back(Rule<T,L>(lhs, rhs));
    }

    Rule<T,L>& findRule(T expr){
        for(Rule<T,L>& rule : rules){
            if(rule.getLhs() == expr){
                return rule;
            }
        }
    }

    void setRules(T expr, L rhs) {
        this->addRule(expr, rhs);
    }


    void printValue(T expr){
        Rule<T,L> rule = findRule(expr);
        cout << rule.getRhs() << endl;
    }
private:
    vector<Rule<T,L>> rules;
};


bool isObject(string& str, vector<Object>& objects){
    for(Object& o : objects){
        if(o.name == str){
            return true;
        }
    }
    return false;
}

/*
match(lhs, expr):
    if lhs is a variable:
        return {lhs: expr}
    if lhs is an object:
        if lhs.name != expr.name:
            return None
        mapping = {}
        for i, j in in_pairs(lhs.args, expr.args):
            submapping = match(i, j)
            if submapping is None:
                return None
            mapping.update(submapping)
        return mapping
  */



Expression::ptr parse_token(std::string&& token, std::ostream& err) { // `&&` means that we consume the argument (it's potentially moved)

}

Expression::ptr parse_expression(std::istream& in, std::ostream& err) {
    std::string token;

    if (!(in >> token) || token == "=" || (token == "+" && !(in >> token))) {
        err << "Expected expression" << std::endl;
        return nullptr;
    }

    Expression::ptr expr = parse_token(std::move(token), err);


    while (in >> token) {

    }

    return expr;
}


void program_loop(std::istream& in = std::cin, std::ostream& out = std::cout, std::ostream& err = std::cerr) {
    vector<Object> objects;
    Rules<string, string> rules;
    string line;
    string variable;

    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }

        if (line == "quit") {
            break;
        }

        std::istringstream line_stream(line);

        if (line_stream >> variable) {
            if (variable == "#") {
                string name;
                int argCount;
                if (!(line_stream >> name >> argCount)) {
                    err << "Invalid declaration" << std::endl;
                    continue;
                }
                objects.emplace_back(name, argCount);
            } else if(isObject(variable, objects)){
                if (contains(line,  "=")) {  //this is a rule


                } else {                               //this is statement

                }
            } else {
                err << "Invalid variable name: " << variable << std::endl;
                continue;
            }
        } else {
            err << "Expected variable name" << std::endl;
            continue;
        }

    }
}


int main() {
    program_loop();
    return 0;
}
