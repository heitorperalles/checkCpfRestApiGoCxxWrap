#ifndef MIDDLEWARE_HXX
#define MIDDLEWARE_HXX

#include <string>
#include <curl/curl.h>

// Class to handle CPF validation
//
// Manages CPF verifications and HTTP requests to SERPRO
class Middleware {

public:

		// Method to validate CPF
		//
		// Param: Cpf
		// Return:
		//		200 (CPF OK)
		//		400 (Invalid CPF format)
		//		403 (CPF not regular or not existant)
		//		500 (Communication problem)
    static int validateCpf(const char * cpf);

		// Curl callback to write the received data.
		// References to CURLOPT_WRITEFUNCTION from Lib Curl documentation.
		//
		// Param: ptr received data
		// Param: size block length
		// Param: nmemb amount of blocks
		// Param: userdata set by CURLOPT_WRITEDATA
		//
		// Return: amount of treated bytes.
		static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

private:

		// Global initialization flag
		static bool global;

		// Curl handler for the instance
		static CURL *curl;

		// Function to convert HTTP code of SERPRO response
		//
		// Param: code
		// Return:
		//		200 (Existant CPF)
		//		400 (Invalid CPF format)
		//		403 (CPF not regular or not existant)
		//		500 (Communication problem)
		static int convertHttpCode(int code);

    // CPF pre-processing method
    //
    // Param: Cpf
    // Return:
    //		Treated CPF (empty if invalid)
    static std::string treatCpf(std::string cpf);

    // Method to treat received JSON on SERPRO response
    //
    // Param: body
    // Return:
    //		200 (CPF OK)
    //		403 (CPF not regular or not existant)
    //		500 (Communication problem)
    static int treatResponseData(std::string& body);
};

#endif /* MIDDLEWARE_HXX */
