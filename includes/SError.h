#ifndef SERROR_H
#define SERROR_H

#include <sstream>

struct SError : public std::exception
{
    /*
        see: https://stackoverflow.com/questions/37191227/can-i-throw-a-stream

        You cannot copy object containing stream object using default copy
        constructor, but you can write your own copy constructor that will
        copy content of stream.
    */
        template <typename T>
        SError& operator << (T rhs)
        {
            innerStream << rhs;
            return *this;
        }

        SError() = default;

        SError(SError& rhs)
        {
                innerStream << rhs.innerStream.str();
        }

        /*
            Solution of what() is not perfect. If str = innerStream.str() throws then
            std::terminate will be called.
            But if plain copy of a stupid short string throws, then the general
            situation is disastrous enough that the exception finishing
            successfully won't help any.
        */
        virtual const char* what() const noexcept
        {
            str = innerStream.str();  //this can throw
            return str.c_str();
        }

    private:
        std::stringstream innerStream;
        mutable std::string str;
};

#endif
