#include <bits/stdc++.h>
using namespace std;

class Term {
public:
    int a, b, c, d; // coefficient, x^b, sin^c(x), cos^d(x)
    
    Term(int a = 0, int b = 0, int c = 0, int d = 0) : a(a), b(b), c(c), d(d) {}
    
    bool operator==(const Term& other) const {
        return b == other.b && c == other.c && d == other.d;
    }
    
    bool operator<(const Term& other) const {
        if (b != other.b) return b > other.b;
        if (c != other.c) return c > other.c;
        return d > other.d;
    }
};

class Poly {
public:
    vector<Term> terms;
    
    Poly() {}
    Poly(const vector<Term>& t) : terms(t) {}
    
    void simplify() {
        // Sort terms by exponents
        sort(terms.begin(), terms.end());
        
        // Combine like terms
        vector<Term> simplified;
        for (const Term& t : terms) {
            if (t.a == 0) continue;
            
            bool found = false;
            for (Term& st : simplified) {
                if (st.b == t.b && st.c == t.c && st.d == t.d) {
                    st.a += t.a;
                    found = true;
                    break;
                }
            }
            if (!found) {
                simplified.push_back(t);
            }
        }
        
        // Remove zero coefficient terms
        terms.clear();
        for (const Term& t : simplified) {
            if (t.a != 0) {
                terms.push_back(t);
            }
        }
    }
    
    Poly operator+(const Poly& other) const {
        Poly result;
        result.terms = terms;
        result.terms.insert(result.terms.end(), other.terms.begin(), other.terms.end());
        result.simplify();
        return result;
    }
    
    Poly operator-(const Poly& other) const {
        Poly result;
        result.terms = terms;
        for (Term t : other.terms) {
            t.a = -t.a;
            result.terms.push_back(t);
        }
        result.simplify();
        return result;
    }
    
    Poly operator*(const Poly& other) const {
        Poly result;
        for (const Term& t1 : terms) {
            for (const Term& t2 : other.terms) {
                Term t(t1.a * t2.a, t1.b + t2.b, t1.c + t2.c, t1.d + t2.d);
                result.terms.push_back(t);
            }
        }
        result.simplify();
        return result;
    }
    
    Poly derivative() const {
        Poly result;
        for (const Term& t : terms) {
            // derivative of a*x^b*sin^c(x)*cos^d(x)
            // Use product rule: derivative = sum of derivatives of each factor
            
            // derivative of x^b term
            if (t.b > 0) {
                Term t1(t.a * t.b, t.b - 1, t.c, t.d);
                result.terms.push_back(t1);
            }
            
            // derivative of sin^c(x) term
            if (t.c > 0) {
                // (sin^c(x))' = c*sin^(c-1)(x)*cos(x)
                Term t2(t.a * t.c, t.b, t.c - 1, t.d + 1);
                result.terms.push_back(t2);
            }
            
            // derivative of cos^d(x) term
            if (t.d > 0) {
                // (cos^d(x))' = -d*sin(x)*cos^(d-1)(x)
                Term t3(-t.a * t.d, t.b, t.c + 1, t.d - 1);
                result.terms.push_back(t3);
            }
        }
        result.simplify();
        return result;
    }
};

class Frac {
public:
    Poly p, q;
    
    Frac() {}
    Frac(const Poly& p, const Poly& q) : p(p), q(q) {}
    Frac(int x) {
        p = Poly({Term(x, 0, 0, 0)});
        q = Poly({Term(1, 0, 0, 0)});
    }
    Frac(const Term& t) {
        p = Poly({t});
        q = Poly({Term(1, 0, 0, 0)});
    }
    
    Frac operator+(const Frac& other) const {
        Frac result;
        result.p = p * other.q + other.p * q;
        result.q = q * other.q;
        return result;
    }
    
    Frac operator-(const Frac& other) const {
        Frac result;
        result.p = p * other.q - other.p * q;
        result.q = q * other.q;
        return result;
    }
    
    Frac operator*(const Frac& other) const {
        Frac result;
        result.p = p * other.p;
        result.q = q * other.q;
        return result;
    }
    
    Frac operator/(const Frac& other) const {
        Frac result;
        result.p = p * other.q;
        result.q = q * other.p;
        return result;
    }
    
    Frac derivative() const {
        // (p/q)' = (p'*q - q'*p) / (q*q)
        Frac result;
        result.p = p.derivative() * q - q.derivative() * p;
        result.q = q * q;
        return result;
    }
};

string termToString(const Term& t) {
    string result;
    
    // Handle coefficient
    if (t.b == 0 && t.c == 0 && t.d == 0) {
        // Constant term
        return to_string(t.a);
    }
    
    // Handle sign and coefficient
    if (t.a == -1) {
        result += "-";
    } else if (t.a != 1) {
        result += to_string(t.a);
    }
    
    // Handle x^b
    if (t.b > 0) {
        result += "x";
        if (t.b > 1) {
            result += "^" + to_string(t.b);
        }
    }
    
    // Handle sin^c(x)
    if (t.c > 0) {
        result += "sin";
        if (t.c > 1) {
            result += "^" + to_string(t.c);
        }
        result += "x";
    }
    
    // Handle cos^d(x)
    if (t.d > 0) {
        result += "cos";
        if (t.d > 1) {
            result += "^" + to_string(t.d);
        }
        result += "x";
    }
    
    return result;
}

string polyToString(const Poly& poly) {
    if (poly.terms.empty()) {
        return "0";
    }
    
    string result;
    for (size_t i = 0; i < poly.terms.size(); i++) {
        const Term& t = poly.terms[i];
        
        if (i == 0) {
            // First term - no leading + if positive
            if (t.a < 0) {
                result += termToString(t);
            } else {
                result += termToString(t);
            }
        } else {
            // Subsequent terms
            if (t.a < 0) {
                result += termToString(t);
            } else {
                result += "+" + termToString(t);
            }
        }
    }
    
    return result;
}

string fracToString(const Frac& frac) {
    string numerator = polyToString(frac.p);
    string denominator = polyToString(frac.q);
    
    // Special cases
    if (numerator == "0") {
        return "0";
    }
    if (denominator == "1") {
        return numerator;
    }
    
    // Check if we need parentheses
    bool needParenN = (frac.p.terms.size() > 1);
    bool needParenD = (frac.q.terms.size() > 1);
    
    string result;
    if (needParenN) result += "(";
    result += numerator;
    if (needParenN) result += ")";
    
    result += "/";
    
    if (needParenD) result += "(";
    result += denominator;
    if (needParenD) result += ")";
    
    return result;
}

int getNumber(const string& s, int& pos) {
    int sign = 1;
    if (pos < s.size() && s[pos] == '-') {
        sign = -1;
        pos++;
    }
    
    if (pos >= s.size() || !isdigit(s[pos])) {
        return sign;
    }
    
    int num = 0;
    while (pos < s.size() && isdigit(s[pos])) {
        num = num * 10 + (s[pos] - '0');
        pos++;
    }
    
    return sign * num;
}

Term parseTerm(const string& s, int& pos) {
    int a = getNumber(s, pos);
    int b = 0, c = 0, d = 0;
    
    while (pos < s.size()) {
        if (s[pos] == 'x') {
            pos++;
            b = 1;
            if (pos < s.size() && s[pos] == '^') {
                pos++;
                b = getNumber(s, pos);
            }
        } else if (pos + 2 < s.size() && s.substr(pos, 3) == "sin") {
            pos += 3;
            c = 1;
            if (pos < s.size() && s[pos] == '^') {
                pos++;
                c = getNumber(s, pos);
            }
            if (pos < s.size() && s[pos] == 'x') pos++;
        } else if (pos + 2 < s.size() && s.substr(pos, 3) == "cos") {
            pos += 3;
            d = 1;
            if (pos < s.size() && s[pos] == '^') {
                pos++;
                d = getNumber(s, pos);
            }
            if (pos < s.size() && s[pos] == 'x') pos++;
        } else {
            break;
        }
    }
    
    return Term(a, b, c, d);
}

Poly parsePoly(const string& s, int& pos, int maxPos) {
    vector<Term> terms;
    
    while (pos < maxPos) {
        Term t = parseTerm(s, pos);
        if (t.a != 0) {
            terms.push_back(t);
        }
        
        if (pos < maxPos && (s[pos] == '+' || s[pos] == '-')) {
            if (s[pos] == '+') pos++;
            // For '-', the getNumber() in parseTerm will handle it
        } else {
            break;
        }
    }
    
    Poly result(terms);
    result.simplify();
    return result;
}

Frac parseFactor(const string& expr, int l, int r);
Frac parseExpression(const string& expr, int l, int r);

Frac parseFactor(const string& expr, int l, int r) {
    // Skip spaces
    while (l <= r && expr[l] == ' ') l++;
    while (l <= r && expr[r] == ' ') r--;
    
    if (l > r) return Frac(0);
    
    // Handle parentheses
    if (expr[l] == '(') {
        int balance = 1;
        int pos = l + 1;
        while (pos <= r && balance > 0) {
            if (expr[pos] == '(') balance++;
            else if (expr[pos] == ')') balance--;
            pos++;
        }
        if (balance == 0) {
            return parseExpression(expr, l + 1, pos - 2);
        }
    }
    
    // Parse simple term or polynomial
    string sub = expr.substr(l, r - l + 1);
    bool hasOp = false;
    for (char c : sub) {
        if (c == '+' || c == '-' || c == '*' || c == '/') {
            hasOp = true;
            break;
        }
    }
    
    if (!hasOp) {
        // Simple term
        int pos = l;
        Term t = parseTerm(expr, pos);
        return Frac(t);
    }
    
    // Check for division at top level
    int balance = 0;
    int divPos = -1;
    for (int i = l; i <= r; i++) {
        if (expr[i] == '(') balance++;
        else if (expr[i] == ')') balance--;
        else if (balance == 0 && expr[i] == '/') {
            divPos = i;
            break;
        }
    }
    
    if (divPos != -1) {
        Frac left = parseFactor(expr, l, divPos - 1);
        Frac right = parseFactor(expr, divPos + 1, r);
        return left / right;
    }
    
    // Check for multiplication at top level
    balance = 0;
    int mulPos = -1;
    for (int i = r; i >= l; i--) {
        if (expr[i] == ')') balance++;
        else if (expr[i] == '(') balance--;
        else if (balance == 0 && expr[i] == '*') {
            mulPos = i;
            break;
        }
    }
    
    if (mulPos != -1) {
        Frac left = parseFactor(expr, l, mulPos - 1);
        Frac right = parseFactor(expr, mulPos + 1, r);
        return left * right;
    }
    
    // Parse as polynomial
    int pos = l;
    return Frac(parsePoly(expr, pos, r + 1), Poly({Term(1, 0, 0, 0)}));
}

Frac parseExpression(const string& expr, int l, int r) {
    // Skip spaces
    while (l <= r && expr[l] == ' ') l++;
    while (l <= r && expr[r] == ' ') r--;
    
    if (l > r) return Frac(0);
    
    // Handle parentheses
    if (expr[l] == '(') {
        int balance = 1;
        int pos = l + 1;
        while (pos <= r && balance > 0) {
            if (expr[pos] == '(') balance++;
            else if (expr[pos] == ')') balance--;
            pos++;
        }
        if (balance == 0 && pos > r) {
            return parseExpression(expr, l + 1, r - 1);
        }
    }
    
    // Find top-level + or -
    int balance = 0;
    int opPos = -1;
    char op = '+';
    
    for (int i = l; i <= r; i++) {
        if (expr[i] == '(') balance++;
        else if (expr[i] == ')') balance--;
        else if (balance == 0 && (expr[i] == '+' || expr[i] == '-')) {
            if (i > l) {  // Not unary minus
                opPos = i;
                op = expr[i];
            }
        }
    }
    
    if (opPos != -1) {
        Frac left = parseExpression(expr, l, opPos - 1);
        Frac right = parseExpression(expr, opPos + 1, r);
        if (op == '+') {
            return left + right;
        } else {
            return left - right;
        }
    }
    
    // No top-level + or -, parse as factor
    return parseFactor(expr, l, r);
}

void solve(const string& s, int n) {
    Frac f = parseExpression(s, 0, n - 1);
    Frac df = f.derivative();
    
    cout << fracToString(f) << endl;
    cout << fracToString(df) << endl;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    string expr;
    getline(cin, expr);
    
    solve(expr, expr.length());
    
    return 0;
}