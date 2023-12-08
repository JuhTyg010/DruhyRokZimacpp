#ifndef EXPRESSIONS1_HPP
#define EXPRESSIONS1_HPP

#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_map>

namespace expressions {

// an abstract base class for all values
// - the fact that it is abstract is deduced from the `= 0` in the declaration of `print` (see below)
// this is equivalent to an interface in Java or C# (as long as we don't add any data members)
class Value {
public:
    using ptr = std::shared_ptr<Value>;
    using const_ptr = std::shared_ptr<const Value>;

    // `virtual` means that this a can be overridden in derived classes

    // virtual destructors are required for polymorphic classes (i.e. classes with virtual methods)
    //   if we don't declare the destructor as virtual, then the destructor of the derived class will not be called
    //   when we delete a pointer to the base class that points to an object of the derived class -> memory leak or worse (UB: undefined behavior)
    virtual ~Value() noexcept = default; // `noexcept` is implicit in destructors

    virtual std::ostream& print(std::ostream& out) const = 0; // `= 0` makes this a pure virtual method (aka. abstract - it has no implementation)
};

class invalid_operand : public std::runtime_error {
public:
    using std::runtime_error::runtime_error; // inherit constructors from `std::runtime_error`
};

// use `final` if you don't want to allow further inheritance (use sparingly, here it is mainly for demonstration purposes)
class Int final : public Value { // public inheritance keeps the access rights to the members of the base class as they are (this is the most common case)
public:
    explicit Int(int value) : value_(value) {} // `explicit` prevents implicit conversions from `int` to `Int`

    int get() const { return value_; }

    // `override` specifies that we require the base class to define a virtual method of the same signature
    //   note that `override` is optional, but it helps the compiler catch errors
    std::ostream& print(std::ostream& out) const override {
        return out << value_;
    }

    operator int() const { return value_; } // implicit conversion to `int`
    operator double() const { return value_; } // implicit conversion to `double`

private:
    int value_;
};

class Double final : public Value {
public:
    explicit Double(double value) : value_(value) {} // `explicit` prevents implicit conversions from `double` to `Double`

    double get() const { return value_; }

    std::ostream& print(std::ostream& out) const override {
        return out << value_;
    }

    operator double() const { return value_; } // implicit conversion to `double`
    explicit operator int() const { return static_cast<int>(value_); } // explicit conversion to `int`

private:
    double value_;
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

private:
    Value::ptr value_;
};

class VariableExpression final : public Expression {
public:
    explicit VariableExpression(std::string value) : value_(std::move(value)) {}

    Value::ptr evaluate(const context& ctx) const override {
        return ctx.at(value_)->evaluate(ctx); // throws `std::out_of_range` if the key is not found
    }

private:
    std::string value_;
};

class AddExpression final : public Expression {
public:
    AddExpression(Expression::ptr lhs, Expression::ptr rhs) : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

    Value::ptr evaluate(const context& ctx) const override {
        Value::ptr lhs = lhs_->evaluate(ctx);
        Value::ptr rhs = rhs_->evaluate(ctx);

        // TODO: implement the addition of two values, use `dynamic_cast` to determine the types of the values (returns `nullptr` if the cast fails)

        if(auto lhsInt = dynamic_cast<Int*>(lhs.get())){
            if(auto rhsInt = dynamic_cast<Int*>(rhs.get())){
                return std::make_shared<Int>(lhsInt->get() + rhsInt->get());
            } else if(auto rhsDouble = dynamic_cast<Double*>(rhs.get())){
                return std::make_shared<Double>(lhsInt->get() + rhsDouble->get());
            } else return nullptr;
        } else if(auto lhsDouble = dynamic_cast<Double*>(lhs.get())){
            if(auto rhsInt = dynamic_cast<Int*>(rhs.get())){
                return std::make_shared<Double>(lhsDouble->get() + rhsInt->get());
            } else if(auto rhsDouble = dynamic_cast<Double*>(rhs.get())){
                return std::make_shared<Double>(lhsDouble->get() + rhsDouble->get());
            } else return nullptr;
        } else return nullptr;


        throw invalid_operand("received an invalid operand");
    }

private:
    Expression::ptr lhs_;
    Expression::ptr rhs_;
};

} // namespace expressions

#endif // EXPRESSIONS1_HPP
