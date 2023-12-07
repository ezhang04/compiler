#include "val.h"

// numeric overloaded add this to op
Value Value::operator+(const Value& op) const
{
    if (IsReal() && op.IsReal())
    {
        return Value(GetReal() + op.GetReal());
    }
    else if (IsInt() && op.IsInt())
    {
        return Value(GetInt() + op.GetInt());
    }
    else if (IsInt() && op.IsReal())
    {
        return Value(GetInt() + op.GetReal());
    }
    else if (IsReal() && op.IsInt())
    {
        return Value(GetReal() + op.GetInt());
    }
    else
    {
        return Value();
    }
}

// numeric overloaded subtract op from this
Value Value::operator-(const Value& op) const
{
    if (IsReal() && op.IsReal())
    {
        return Value(GetReal() - op.GetReal());
    }
    else if (IsInt() && op.IsInt())
    {
        return Value(GetInt() - op.GetInt());
    }
    else if (IsInt() && op.IsReal())
    {
        return Value(GetInt() - op.GetReal());
    }
    else if (IsReal() && op.IsInt())
    {
        return Value(GetReal() - op.GetInt());
    }
    else
    {
        return Value();
    }
}

// numeric overloaded multiply this by op
Value Value::operator*(const Value& op) const
{
    if (IsReal() && op.IsReal())
    {
        return Value(GetReal() * op.GetReal());
    }
    else if (IsInt() && op.IsInt())
    {
        return Value(GetInt() * op.GetInt());
    }
    else if (IsInt() && op.IsReal())
    {
        return Value(GetInt() * op.GetReal());
    }
    else if (IsReal() && op.IsInt())
    {
        return Value(GetReal() * op.GetInt());
    }
    else
    {
        return Value();
    }
}

// numeric overloaded divide this by oper
Value Value::operator/(const Value& op) const
{
    if (op.GetInt() == 0 || op.GetReal() == 0){
        return Value();
    }
    if (IsReal() && op.IsReal())
    {
        return Value(GetReal() / op.GetReal());
    }
    else if (IsInt() && op.IsInt())
    {
        return Value(GetInt() / op.GetInt());
    }
    else if (IsInt() && op.IsReal())
    {
        return Value(GetInt() / op.GetReal());
    }
    else if (IsReal() && op.IsInt())
    {
        return Value(GetReal() / op.GetInt());
    }
    else
    {
        return Value();
    }
}

// numeric overloaded modulus of this by oper
Value Value::operator%(const Value& op) const
{
    if (IsInt() && op.IsInt()) {
        return Value(GetInt() % op.GetInt());
    } else {
        return Value();
    }
}

//numeric integer division this by oper
Value Value::div(const Value& op) const
{
    if (op.GetInt() == 0 || op.GetReal() == 0){
        return Value();
    }
    if (IsReal() && op.IsReal())
    {
        return Value(int(GetReal() / op.GetReal()));
    }
    else if (IsInt() && op.IsInt())
    {
        return Value(int(GetInt() / op.GetInt()));
    }
    else if (IsInt() && op.IsReal())
    {
        return Value(int(GetInt() / op.GetReal()));
    }
    else if (IsReal() && op.IsInt())
    {
        return Value(int(GetReal() / op.GetInt()));
    }
    else
    {
        return Value();
    }
}


//overloaded equality operator of this with op
Value Value::operator==(const Value& op) const
{
    switch (GetType()) {
        case VINT:
            if (op.IsInt())
            {
                return Value(GetInt() == op.GetInt());
            }
            else if (op.IsReal())
            {
                return Value(GetInt() == op.GetReal());
            }
            return Value ();
        case VREAL:
            if (op.IsInt())
            {
                return Value(GetReal() == op.GetInt());
            }
            else if (op.IsReal())
            {
                return Value(GetReal() == op.GetReal());
            }
            return Value ();
        case VSTRING:
            if (!op.IsString())
            {
                return Value();
            }
            return Value(GetString() == op.GetString());
        case VBOOL:
            if (!op.IsBool())
            {
                return Value();
            }
            return Value(GetBool() == op.GetBool());
        default:
            return Value();
    }
}
//overloaded greater than operator of this with op
Value Value::operator>(const Value& op) const
{
    if (IsReal() && op.IsReal())
    {
        bool b = GetReal() > op.GetReal();
        return Value(b);
    }
    else if (IsInt() && op.IsInt())
    {
        return Value((GetInt() > op.GetInt()));
    }
    else if (IsInt() && op.IsReal())
    {
        return Value((GetInt() > op.GetReal()));
    }
    else if (IsReal() && op.IsInt())
    {
        return Value((GetReal() > op.GetInt()));
    }
    else if (IsBool() && op.IsBool())
    {
        return Value (GetBool() > op.GetBool());
    }
    else if (IsString() && op.IsString())
    {
        return Value (GetString() > op.GetString());
    }
    else
    {
        return Value();
    }
}
//overloaded less than operator of this with op
Value Value::operator<(const Value& op) const
{
    if (IsReal() && op.IsReal())
    {
        return Value((GetReal() < op.GetReal()));
    }
    else if (IsInt() && op.IsInt())
    {
        return Value((GetInt() < op.GetInt()));
    }
    else if (IsInt() && op.IsReal())
    {
        return Value((GetInt() < op.GetReal()));
    }
    else if (IsReal() && op.IsInt())
    {
        return Value((GetReal() < op.GetInt()));
    }
    else if (IsBool() && op.IsBool())
    {
        return Value (GetBool() < op.GetBool());
    }
    else if (IsString() && op.IsString())
    {
        return Value (GetString() < op.GetString());
    }
    else
    {
        return Value();
    }
}

//integer divide operator of this by op
Value Value::idiv(const Value& op) const
{
    if (op.GetInt() == 0 || op.GetReal() == 0){
        return Value();
    }
    if (IsReal() && op.IsReal())
    {
        return Value(int(GetReal() / op.GetReal()));
    }
    else if (IsInt() && op.IsInt())
    {
        return Value(int(GetInt() / op.GetInt()));
    }
    else if (IsInt() && op.IsReal())
    {
        return Value(int(GetInt() / op.GetReal()));
    }
    else if (IsReal() && op.IsInt())
    {
        return Value(int(GetReal() / op.GetInt()));
    }
    else
    {
        return Value();
    }
}

//Logic operations
Value Value::operator&&(const Value& op) const
{
    if (IsBool() && op.IsBool()) {
        return Value(GetBool() && op.GetBool());
    } else {
        return Value();
    }
}
Value Value::operator||(const Value& op) const
{
    if (IsBool() && op.IsBool()) {
        return Value(GetBool() || op.GetBool());
    } else if (IsInt() && op.IsInt()){
        return Value (GetInt() || op.GetInt());
    } else if (IsReal() && op.IsReal()){
        return Value (GetReal() || op.GetReal());
    } else if (IsInt() && op.IsInt()){
        return Value (GetInt() || op.GetInt());
    }
    else {
        return Value();
    }
}
Value Value::operator!() const
{
    if (IsBool())
    {
        return Value(!GetBool());
    }
    else
    {
        return Value();
    }
}