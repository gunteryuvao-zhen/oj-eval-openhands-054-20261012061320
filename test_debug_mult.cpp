#include <bits/stdc++.h>
using namespace std;

// Copy the essential classes and functions from solution.cpp
class Term {
public:
    int a, b, c, d;
    
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
        sort(terms.begin(), terms.end());
        
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
    
    void print() const {
        cout << "Poly: ";
        for (const Term& t : terms) {
            cout << "(" << t.a << "," << t.b << "," << t.c << "," << t.d << ") ";
        }
        cout << endl;
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
    
    void print() const {
        cout << "Frac: ";
        p.print();
        cout << "----- ";
        q.print();
    }
};

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
    while (l <= r && expr[l] == ' ') l++;
    while (l <= r && expr[r] == ' ') r--;
    
    if (l > r) return Frac(0);
    
    cout << "parseFactor: " << expr.substr(l, r-l+1) << endl;
    
    if (expr[l] == '(') {
        int balance = 1;
        int pos = l + 1;
        while (pos <= r && balance > 0) {
            if (expr[pos] == '(') balance++;
            else if (expr[pos] == ')') balance--;
            pos++;
        }
        if (balance == 0 && pos == r + 1) {
            // Entire expression is enclosed in parentheses
            cout << "Entire expression in parentheses, parsing " << expr.substr(l + 1, r - l - 1) << endl;
            return parseExpression(expr, l + 1, r - 1);
        }
    }
    
    string sub = expr.substr(l, r - l + 1);
    bool hasOp = false;
    for (char c : sub) {
        if (c == '+' || c == '-' || c == '*' || c == '/') {
            hasOp = true;
            break;
        }
    }
    
    if (!hasOp) {
        int pos = l;
        Term t = parseTerm(expr, pos);
        return Frac(t);
    }
    
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
    
    cout << "Looking for multiplication, found at position " << mulPos << endl;
    
    if (mulPos != -1) {
        cout << "Found multiplication at " << mulPos << endl;
        Frac left = parseFactor(expr, l, mulPos - 1);
        cout << "Left factor: "; left.print();
        Frac right = parseFactor(expr, mulPos + 1, r);
        cout << "Right factor: "; right.print();
        Frac result = left * right;
        cout << "Result: "; result.print();
        return result;
    }
    
    int pos = l;
    return Frac(parsePoly(expr, pos, r + 1), Poly({Term(1, 0, 0, 0)}));
}

Frac parseExpression(const string& expr, int l, int r) {
    while (l <= r && expr[l] == ' ') l++;
    while (l <= r && expr[r] == ' ') r--;
    
    if (l > r) return Frac(0);
    
    cout << "parseExpression: " << expr.substr(l, r-l+1) << endl;
    
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
    
    int balance = 0;
    int opPos = -1;
    char op = '+';
    
    for (int i = l; i <= r; i++) {
        if (expr[i] == '(') balance++;
        else if (expr[i] == ')') balance--;
        else if (balance == 0 && (expr[i] == '+' || expr[i] == '-')) {
            if (i > l) {
                cout << "Found operator " << expr[i] << " at position " << i << " with balance " << balance << endl;
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
    
    return parseFactor(expr, l, r);
}

int main() {
    string expr = "(x+1)*(x-1)";
    
    Frac f = parseExpression(expr, 0, expr.length() - 1);
    f.print();
    
    return 0;
}