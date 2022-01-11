#ifndef GAME_EXCEPTIONS_H
#define GAME_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class Exception : public std::exception
{
	public:
		Exception() = delete;
		Exception(const Exception& other) = default;
		virtual ~Exception() noexcept = default;

		explicit Exception(const std::string& error_type, const std::string& error_message)
		{
		    if (error_message == "")
            {
		        this->error_message = error_type;
            }
		    else
            {
		        this->error_message =  error_type + ": " + error_message;
            }
		}
		
		const char* what() const noexcept override {
			return error_message.c_str();
		}
	private:
		std::string error_type;
		std::string error_message;
};

class AllocationError : public Exception
{
	public:
		explicit AllocationError(const std::string& msg = "") : Exception(msg, "AllocationError") {}

};


class InvalidInput : public Exception
{
	public:
		explicit InvalidInput(const std::string& msg = "") : Exception(msg, "InvalidInput") {}

};

class Failure : public Exception
{
	public:
		explicit Failure(const std::string& msg = "") : Exception(msg, "Failure") {}
};

#endif //GAME_EXCEPTIONS_H
