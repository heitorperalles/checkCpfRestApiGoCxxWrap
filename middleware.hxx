//------------------------------------------------------------------------------
// From http://github.com/heitorperalles/checkCpfRestApiGoCxxWrap
//
// Distributed under The MIT License (MIT) <http://opensource.org/licenses/MIT>
//
// Copyright (c) 2020 Heitor Peralles <heitorgp@gmail.com>
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------
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
