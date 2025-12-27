// Choco Programming Language Interpreter
// Combines Go, Rust, and Ruby-like syntax
// File extension: .choco

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cmath>
#include <stdexcept>

// Token types
enum TokenType {
    TOKEN_EOF, TOKEN_NUMBER, TOKEN_STRING, TOKEN_IDENTIFIER,
    TOKEN_LET, TOKEN_FN, TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_FOR, TOKEN_IN,
    TOKEN_RETURN, TOKEN_PUTS, TOKEN_TRUE, TOKEN_FALSE,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_PERCENT,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL, TOKEN_BANG_EQUAL,
    TOKEN_LESS, TOKEN_GREATER, TOKEN_LESS_EQUAL, TOKEN_GREATER_EQUAL,
    TOKEN_AND, TOKEN_OR, TOKEN_BANG,
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_LBRACKET, TOKEN_RBRACKET,
    TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_ARROW, TOKEN_DOT, TOKEN_DOTDOT, TOKEN_COLON
};

struct Token {
    TokenType type;
    std::string value;
    int line;
};

// Lexer
class Lexer {
    std::string source;
    size_t pos = 0;
    int line = 1;

public:
    Lexer(const std::string& src) : source(src) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (pos < source.length()) {
            skipWhitespace();
            if (pos >= source.length()) break;

            if (source[pos] == '/' && pos + 1 < source.length() && source[pos + 1] == '/') {
                skipComment();
                continue;
            }

            Token tok = nextToken();
            tokens.push_back(tok);
        }
        tokens.push_back({TOKEN_EOF, "", line});
        return tokens;
    }

private:
    void skipWhitespace() {
        while (pos < source.length() && std::isspace(source[pos])) {
            if (source[pos] == '\n') line++;
            pos++;
        }
    }

    void skipComment() {
        while (pos < source.length() && source[pos] != '\n') pos++;
    }

    Token nextToken() {
        char c = source[pos];

        if (std::isdigit(c)) return number();
        if (std::isalpha(c) || c == '_') return identifier();
        if (c == '"') return string();

        pos++;
        switch (c) {
            case '+': return {TOKEN_PLUS, "+", line};
            case '*': return {TOKEN_STAR, "*", line};
            case '/': return {TOKEN_SLASH, "/", line};
            case '%': return {TOKEN_PERCENT, "%", line};
            case '(': return {TOKEN_LPAREN, "(", line};
            case ')': return {TOKEN_RPAREN, ")", line};
            case '{': return {TOKEN_LBRACE, "{", line};
            case '}': return {TOKEN_RBRACE, "}", line};
            case '[': return {TOKEN_LBRACKET, "[", line};
            case ']': return {TOKEN_RBRACKET, "]", line};
            case ',': return {TOKEN_COMMA, ",", line};
            case ';': return {TOKEN_SEMICOLON, ";", line};
            case ':': return {TOKEN_COLON, ":", line};
            case '.':
                if (pos < source.length() && source[pos] == '.') {
                    pos++;
                    return {TOKEN_DOTDOT, "..", line};
                }
                return {TOKEN_DOT, ".", line};
            case '-':
                if (pos < source.length() && source[pos] == '>') {
                    pos++;
                    return {TOKEN_ARROW, "->", line};
                }
                return {TOKEN_MINUS, "-", line};
            case '=':
                if (pos < source.length() && source[pos] == '=') {
                    pos++;
                    return {TOKEN_EQUAL_EQUAL, "==", line};
                }
                return {TOKEN_EQUAL, "=", line};
            case '!':
                if (pos < source.length() && source[pos] == '=') {
                    pos++;
                    return {TOKEN_BANG_EQUAL, "!=", line};
                }
                return {TOKEN_BANG, "!", line};
            case '<':
                if (pos < source.length() && source[pos] == '=') {
                    pos++;
                    return {TOKEN_LESS_EQUAL, "<=", line};
                }
                return {TOKEN_LESS, "<", line};
            case '>':
                if (pos < source.length() && source[pos] == '=') {
                    pos++;
                    return {TOKEN_GREATER_EQUAL, ">=", line};
                }
                return {TOKEN_GREATER, ">", line};
            case '&':
                if (pos < source.length() && source[pos] == '&') {
                    pos++;
                    return {TOKEN_AND, "&&", line};
                }
                break;
            case '|':
                if (pos < source.length() && source[pos] == '|') {
                    pos++;
                    return {TOKEN_OR, "||", line};
                }
                break;
        }
        return {TOKEN_EOF, "", line};
    }

    Token number() {
        std::string num;
        bool hasDot = false;
        
        while (pos < source.length()) {
            if (std::isdigit(source[pos])) {
                num += source[pos++];
            } else if (source[pos] == '.' && !hasDot) {
                // Check if next character is also a dot (range operator)
                if (pos + 1 < source.length() && source[pos + 1] == '.') {
                    // This is the range operator, stop here
                    break;
                }
                // Check if next character is a digit
                if (pos + 1 < source.length() && std::isdigit(source[pos + 1])) {
                    hasDot = true;
                    num += source[pos++];
                } else {
                    // Not part of the number
                    break;
                }
            } else {
                break;
            }
        }
        
        return {TOKEN_NUMBER, num, line};
    }

    Token identifier() {
        std::string id;
        while (pos < source.length() && (std::isalnum(source[pos]) || source[pos] == '_')) {
            id += source[pos++];
        }

        if (id == "let") return {TOKEN_LET, id, line};
        if (id == "fn") return {TOKEN_FN, id, line};
        if (id == "if") return {TOKEN_IF, id, line};
        if (id == "else") return {TOKEN_ELSE, id, line};
        if (id == "while") return {TOKEN_WHILE, id, line};
        if (id == "for") return {TOKEN_FOR, id, line};
        if (id == "in") return {TOKEN_IN, id, line};
        if (id == "return") return {TOKEN_RETURN, id, line};
        if (id == "puts") return {TOKEN_PUTS, id, line};
        if (id == "true") return {TOKEN_TRUE, id, line};
        if (id == "false") return {TOKEN_FALSE, id, line};

        return {TOKEN_IDENTIFIER, id, line};
    }

    Token string() {
        pos++; // skip opening "
        std::string str;
        while (pos < source.length() && source[pos] != '"') {
            if (source[pos] == '\\' && pos + 1 < source.length()) {
                pos++;
                switch (source[pos]) {
                    case 'n': str += '\n'; break;
                    case 't': str += '\t'; break;
                    case '\\': str += '\\'; break;
                    case '"': str += '"'; break;
                    default: str += source[pos];
                }
                pos++;
            } else if (source[pos] == '#' && pos + 1 < source.length() && source[pos + 1] == '{') {
                // String interpolation placeholder
                str += "#{";
                pos += 2;
            } else {
                str += source[pos++];
            }
        }
        pos++; // skip closing "
        return {TOKEN_STRING, str, line};
    }
};

// Value types
struct Value {
    enum Type { NUMBER, STRING, BOOL, ARRAY, NIL } type;
    double num;
    std::string str;
    bool boolean;
    std::vector<Value> array;

    Value() : type(NIL), num(0), boolean(false) {}
    Value(double n) : type(NUMBER), num(n), boolean(false) {}
    Value(const std::string& s) : type(STRING), str(s), num(0), boolean(false) {}
    Value(bool b) : type(BOOL), num(0), boolean(b) {}
    Value(const std::vector<Value>& arr) : type(ARRAY), num(0), boolean(false), array(arr) {}

    std::string toString() const {
        switch (type) {
            case NUMBER: {
                // Format number nicely (no trailing zeros)
                if (num == static_cast<int>(num)) {
                    return std::to_string(static_cast<int>(num));
                }
                std::string s = std::to_string(num);
                s.erase(s.find_last_not_of('0') + 1, std::string::npos);
                if (s.back() == '.') s.pop_back();
                return s;
            }
            case STRING: return str;
            case BOOL: return boolean ? "true" : "false";
            case ARRAY: {
                std::string result = "[";
                for (size_t i = 0; i < array.size(); i++) {
                    result += array[i].toString();
                    if (i < array.size() - 1) result += ", ";
                }
                result += "]";
                return result;
            }
            case NIL: return "nil";
        }
        return "";
    }
};

// Function definition
struct Function {
    std::vector<std::string> params;
    size_t bodyStart;
    size_t bodyEnd;
};

// Interpreter
class Interpreter {
    std::map<std::string, Value> globalVars;
    std::vector<std::map<std::string, Value>> scopes;
    std::map<std::string, Function> functions;
    std::vector<Token> tokens;
    size_t current = 0;
    bool inFunction = false;
    bool hasReturned = false;
    Value returnValue;

public:
    Interpreter(const std::vector<Token>& toks) : tokens(toks) {
        scopes.push_back(std::map<std::string, Value>()); // Global scope
    }

    void execute() {
        while (!isAtEnd()) {
            statement();
        }
    }

private:
    bool isAtEnd() { return tokens[current].type == TOKEN_EOF; }
    Token peek() { return tokens[current]; }
    Token advance() { return tokens[current++]; }
    bool match(TokenType type) {
        if (peek().type == type) {
            advance();
            return true;
        }
        return false;
    }

    void setVariable(const std::string& name, const Value& val) {
        // Check local scopes first (from innermost to outermost)
        for (int i = scopes.size() - 1; i >= 0; i--) {
            if (scopes[i].find(name) != scopes[i].end()) {
                scopes[i][name] = val;
                return;
            }
        }
        // If not found in any scope, set in current scope
        scopes.back()[name] = val;
    }

    Value getVariable(const std::string& name) {
        // Check local scopes first (from innermost to outermost)
        for (int i = scopes.size() - 1; i >= 0; i--) {
            if (scopes[i].find(name) != scopes[i].end()) {
                return scopes[i][name];
            }
        }
        // Check global variables
        if (globalVars.find(name) != globalVars.end()) {
            return globalVars[name];
        }
        return Value();
    }

    void statement() {
        if (hasReturned) return;

        if (match(TOKEN_LET)) {
            letStatement();
        } else if (match(TOKEN_FN)) {
            functionDeclaration();
        } else if (match(TOKEN_PUTS)) {
            putsStatement();
        } else if (match(TOKEN_IF)) {
            ifStatement();
        } else if (match(TOKEN_WHILE)) {
            whileStatement();
        } else if (match(TOKEN_FOR)) {
            forStatement();
        } else if (match(TOKEN_RETURN)) {
            returnStatement();
        } else if (peek().type == TOKEN_IDENTIFIER && current + 1 < tokens.size() && tokens[current + 1].type == TOKEN_EQUAL) {
            Token name = advance();
            match(TOKEN_EQUAL);
            Value val = expression();
            setVariable(name.value, val);
            match(TOKEN_SEMICOLON);
        } else {
            expression();
            match(TOKEN_SEMICOLON);
        }
    }

    void letStatement() {
        Token name = advance();
        match(TOKEN_EQUAL);
        Value val = expression();
        setVariable(name.value, val);
        match(TOKEN_SEMICOLON);
    }

    void functionDeclaration() {
        Token name = advance();
        match(TOKEN_LPAREN);
        
        std::vector<std::string> params;
        while (!match(TOKEN_RPAREN)) {
            Token param = advance();
            params.push_back(param.value);
            if (!match(TOKEN_COMMA)) {
                match(TOKEN_RPAREN);
                break;
            }
        }

        match(TOKEN_LBRACE);
        size_t bodyStart = current;
        
        int braceCount = 1;
        while (braceCount > 0 && !isAtEnd()) {
            if (tokens[current].type == TOKEN_LBRACE) braceCount++;
            if (tokens[current].type == TOKEN_RBRACE) braceCount--;
            current++;
        }
        size_t bodyEnd = current - 1;

        functions[name.value] = {params, bodyStart, bodyEnd};
    }

    void returnStatement() {
        returnValue = expression();
        hasReturned = true;
        match(TOKEN_SEMICOLON);
    }

    void putsStatement() {
        Value val = expression();
        std::cout << val.toString() << std::endl;
        match(TOKEN_SEMICOLON);
    }

    void ifStatement() {
        Value condition = expression();
        
        if (!match(TOKEN_LBRACE)) {
            std::cerr << "Error: Expected '{' after if condition" << std::endl;
            return;
        }
        
        size_t thenStart = current;
        
        // Find matching closing brace for then block
        int braceCount = 1;
        size_t thenEnd = current;
        
        while (braceCount > 0 && thenEnd < tokens.size()) {
            if (tokens[thenEnd].type == TOKEN_LBRACE) {
                braceCount++;
            } else if (tokens[thenEnd].type == TOKEN_RBRACE) {
                braceCount--;
            }
            if (braceCount > 0) {
                thenEnd++;
            }
        }

        size_t elseStart = 0, elseEnd = 0;
        bool hasElse = false;
        
        // Check for else
        size_t afterThen = thenEnd + 1;
        if (afterThen < tokens.size() && tokens[afterThen].type == TOKEN_ELSE) {
            hasElse = true;
            current = afterThen;
            match(TOKEN_ELSE);
            match(TOKEN_LBRACE);
            elseStart = current;
            
            braceCount = 1;
            elseEnd = current;
            while (braceCount > 0 && elseEnd < tokens.size()) {
                if (tokens[elseEnd].type == TOKEN_LBRACE) {
                    braceCount++;
                } else if (tokens[elseEnd].type == TOKEN_RBRACE) {
                    braceCount--;
                }
                if (braceCount > 0) {
                    elseEnd++;
                }
            }
        }

        // Evaluate condition as boolean
        bool shouldExecute = false;
        if (condition.type == Value::BOOL) {
            shouldExecute = condition.boolean;
        } else if (condition.type == Value::NUMBER) {
            shouldExecute = condition.num != 0;
        } else if (condition.type == Value::STRING) {
            shouldExecute = !condition.str.empty();
        }

        if (shouldExecute) {
            current = thenStart;
            while (current < thenEnd && !isAtEnd() && !hasReturned) {
                statement();
            }
        } else if (hasElse) {
            current = elseStart;
            while (current < elseEnd && !isAtEnd() && !hasReturned) {
                statement();
            }
        }
        
        // Move past the entire if-else statement
        current = hasElse ? (elseEnd + 1) : (thenEnd + 1);
    }

    void whileStatement() {
        size_t conditionStart = current;
        
        Value condition = expression();
        match(TOKEN_LBRACE);
        size_t bodyStart = current;
        
        int braceCount = 1;
        size_t bodyEnd = bodyStart;
        while (braceCount > 0 && bodyEnd < tokens.size()) {
            if (tokens[bodyEnd].type == TOKEN_LBRACE) braceCount++;
            if (tokens[bodyEnd].type == TOKEN_RBRACE) braceCount--;
            if (braceCount > 0) bodyEnd++;
        }
        
        while (condition.type == Value::BOOL && condition.boolean && !hasReturned) {
            current = bodyStart;
            while (current < bodyEnd && !isAtEnd() && !hasReturned) {
                statement();
            }
            
            current = conditionStart;
            condition = expression();
            match(TOKEN_LBRACE);
        }
        
        current = bodyEnd + 1;
    }

    void forStatement() {
        Token iterVar = advance();
        
        if (!match(TOKEN_IN)) {
            std::cerr << "Error: Expected 'in' in for loop" << std::endl;
            return;
        }
        
        Value start = expression();
        
        if (!match(TOKEN_DOTDOT)) {
            std::cerr << "Error: Expected '..' in for loop" << std::endl;
            return;
        }
        
        Value end = expression();
        
        if (!match(TOKEN_LBRACE)) {
            std::cerr << "Error: Expected '{' after for range" << std::endl;
            return;
        }
        
        size_t loopBodyStart = current;
        
        // Find matching closing brace
        int depth = 1;
        size_t loopBodyEnd = current;
        
        while (depth > 0 && loopBodyEnd < tokens.size()) {
            if (tokens[loopBodyEnd].type == TOKEN_LBRACE) {
                depth++;
            } else if (tokens[loopBodyEnd].type == TOKEN_RBRACE) {
                depth--;
            }
            if (depth > 0) {
                loopBodyEnd++;
            }
        }
        
        // Execute loop if we have valid numbers
        if (start.type == Value::NUMBER && end.type == Value::NUMBER) {
            int iStart = static_cast<int>(start.num);
            int iEnd = static_cast<int>(end.num);
            
            for (int i = iStart; i < iEnd; i++) {
                if (hasReturned) break;
                
                // Set loop variable
                setVariable(iterVar.value, Value(static_cast<double>(i)));
                
                // Reset to start of body
                size_t savedCurrent = current;
                current = loopBodyStart;
                
                // Execute body statements
                while (current < loopBodyEnd) {
                    if (hasReturned || isAtEnd()) break;
                    statement();
                }
                
                // Restore current (not needed but for safety)
                current = savedCurrent;
            }
        }
        
        // Move past the closing brace
        current = loopBodyEnd + 1;
    }

    Value expression() {
        return logicalOr();
    }

    Value logicalOr() {
        Value left = logicalAnd();
        
        while (match(TOKEN_OR)) {
            Value right = logicalAnd();
            
            // Convert to boolean
            bool leftBool = false;
            if (left.type == Value::BOOL) leftBool = left.boolean;
            else if (left.type == Value::NUMBER) leftBool = left.num != 0;
            
            bool rightBool = false;
            if (right.type == Value::BOOL) rightBool = right.boolean;
            else if (right.type == Value::NUMBER) rightBool = right.num != 0;
            
            left = Value(leftBool || rightBool);
        }
        return left;
    }

    Value logicalAnd() {
        Value left = comparison();
        
        while (match(TOKEN_AND)) {
            Value right = comparison();
            
            // Convert to boolean
            bool leftBool = false;
            if (left.type == Value::BOOL) leftBool = left.boolean;
            else if (left.type == Value::NUMBER) leftBool = left.num != 0;
            
            bool rightBool = false;
            if (right.type == Value::BOOL) rightBool = right.boolean;
            else if (right.type == Value::NUMBER) rightBool = right.num != 0;
            
            left = Value(leftBool && rightBool);
        }
        return left;
    }

    Value comparison() {
        Value left = term();
        
        while (peek().type == TOKEN_EQUAL_EQUAL || peek().type == TOKEN_BANG_EQUAL ||
               peek().type == TOKEN_LESS || peek().type == TOKEN_GREATER ||
               peek().type == TOKEN_LESS_EQUAL || peek().type == TOKEN_GREATER_EQUAL) {
            TokenType op = advance().type;
            Value right = term();
            
            bool result = false;
            if (left.type == Value::NUMBER && right.type == Value::NUMBER) {
                if (op == TOKEN_EQUAL_EQUAL) result = left.num == right.num;
                else if (op == TOKEN_BANG_EQUAL) result = left.num != right.num;
                else if (op == TOKEN_LESS) result = left.num < right.num;
                else if (op == TOKEN_GREATER) result = left.num > right.num;
                else if (op == TOKEN_LESS_EQUAL) result = left.num <= right.num;
                else if (op == TOKEN_GREATER_EQUAL) result = left.num >= right.num;
            } else if (left.type == Value::BOOL && right.type == Value::BOOL) {
                if (op == TOKEN_EQUAL_EQUAL) result = left.boolean == right.boolean;
                else if (op == TOKEN_BANG_EQUAL) result = left.boolean != right.boolean;
            } else if (left.type == Value::STRING && right.type == Value::STRING) {
                if (op == TOKEN_EQUAL_EQUAL) result = left.str == right.str;
                else if (op == TOKEN_BANG_EQUAL) result = left.str != right.str;
            }
            left = Value(result);
        }
        return left;
    }

    Value term() {
        Value left = factor();
        
        while (match(TOKEN_PLUS) || match(TOKEN_MINUS)) {
            TokenType op = tokens[current - 1].type;
            Value right = factor();
            
            if (left.type == Value::NUMBER && right.type == Value::NUMBER) {
                if (op == TOKEN_PLUS) left.num += right.num;
                else left.num -= right.num;
            } else if (left.type == Value::STRING && right.type == Value::STRING && op == TOKEN_PLUS) {
                left.str += right.str;
            }
        }
        return left;
    }

    Value factor() {
        Value left = unary();
        
        while (match(TOKEN_STAR) || match(TOKEN_SLASH) || match(TOKEN_PERCENT)) {
            TokenType op = tokens[current - 1].type;
            Value right = unary();
            
            if (left.type == Value::NUMBER && right.type == Value::NUMBER) {
                if (op == TOKEN_STAR) left.num *= right.num;
                else if (op == TOKEN_SLASH && right.num != 0) left.num /= right.num;
                else if (op == TOKEN_PERCENT && right.num != 0) {
                    left.num = fmod(left.num, right.num);
                }
            }
        }
        return left;
    }

    Value unary() {
        if (match(TOKEN_BANG)) {
            Value val = unary();
            if (val.type == Value::BOOL) {
                return Value(!val.boolean);
            }
            return Value(false);
        }
        if (match(TOKEN_MINUS)) {
            Value val = unary();
            if (val.type == Value::NUMBER) {
                val.num = -val.num;
            }
            return val;
        }
        return call();
    }

    Value call() {
        Value val = primary();
        
        while (match(TOKEN_LPAREN)) {
            if (val.type == Value::STRING) {
                std::string funcName = val.str;
                if (functions.find(funcName) != functions.end()) {
                    std::vector<Value> args;
                    while (!match(TOKEN_RPAREN)) {
                        args.push_back(expression());
                        if (!match(TOKEN_COMMA)) {
                            match(TOKEN_RPAREN);
                            break;
                        }
                    }
                    val = callFunction(funcName, args);
                }
            }
        }
        
        // Array indexing
        if (match(TOKEN_LBRACKET)) {
            Value index = expression();
            match(TOKEN_RBRACKET);
            if (val.type == Value::ARRAY && index.type == Value::NUMBER) {
                int idx = static_cast<int>(index.num);
                if (idx >= 0 && idx < static_cast<int>(val.array.size())) {
                    return val.array[idx];
                }
            }
        }
        
        return val;
    }

    Value callFunction(const std::string& name, const std::vector<Value>& args) {
        if (functions.find(name) == functions.end()) {
            return Value();
        }

        Function& func = functions[name];
        
        // Create new scope for function
        scopes.push_back(std::map<std::string, Value>());
        
        // Bind parameters
        for (size_t i = 0; i < func.params.size() && i < args.size(); i++) {
            scopes.back()[func.params[i]] = args[i];
        }

        // Execute function body
        size_t savedCurrent = current;
        current = func.bodyStart;
        hasReturned = false;
        returnValue = Value();

        while (current < func.bodyEnd && !isAtEnd() && !hasReturned) {
            statement();
        }

        Value result = returnValue;
        hasReturned = false;
        
        // Pop function scope
        scopes.pop_back();
        
        current = savedCurrent;
        return result;
    }

    Value primary() {
        if (match(TOKEN_NUMBER)) {
            return Value(std::stod(tokens[current - 1].value));
        }
        if (match(TOKEN_STRING)) {
            std::string str = tokens[current - 1].value;
            
            // Handle string interpolation
            size_t pos = 0;
            while ((pos = str.find("#{", pos)) != std::string::npos) {
                size_t end = str.find("}", pos);
                if (end != std::string::npos) {
                    std::string varName = str.substr(pos + 2, end - pos - 2);
                    Value val = getVariable(varName);
                    str.replace(pos, end - pos + 1, val.toString());
                }
                pos++;
            }
            
            return Value(str);
        }
        if (match(TOKEN_TRUE)) return Value(true);
        if (match(TOKEN_FALSE)) return Value(false);
        if (match(TOKEN_LBRACKET)) {
            std::vector<Value> arr;
            while (!match(TOKEN_RBRACKET)) {
                arr.push_back(expression());
                if (!match(TOKEN_COMMA)) {
                    match(TOKEN_RBRACKET);
                    break;
                }
            }
            return Value(arr);
        }
        if (match(TOKEN_IDENTIFIER)) {
            std::string name = tokens[current - 1].value;
            
            // Check if it's a function name
            if (functions.find(name) != functions.end()) {
                return Value(name); // Return function name as string for later call
            }
            
            return getVariable(name);
        }
        if (match(TOKEN_LPAREN)) {
            Value val = expression();
            match(TOKEN_RPAREN);
            return val;
        }
        return Value();
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.choco>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Interpreter interpreter(tokens);
    interpreter.execute();

    return 0;
}