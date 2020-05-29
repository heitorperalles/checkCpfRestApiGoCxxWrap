#include <regex>
#include "middleware.hxx"

// Public URL to request CPF status on SERPRO API
#define SERPRO_URL "https://apigateway.serpro.gov.br/consulta-cpf-df-trial/v1/cpf/"

// Token to be used setting requests on SERPRO API
#define SERPRO_TOKEN "4e1a1858bdd584fdc077fb7d80f39283"

// Prefix of the Bearer to be inserted on the request header
#define AUTHENTICATION_TOKEN_PREFIX "Authorization: Bearer "

// Authentication Header
#define REQUEST_HEADER AUTHENTICATION_TOKEN_PREFIX SERPRO_TOKEN

// Middleware::validateCpf possible return codes
#define CODE_200_CPF_OK						200
#define CODE_400_INVALID_FORMAT		400
#define CODE_403_SUBJECT_REJECTED	403
#define CODE_500_SERVER_PROBLEM		500

// Logging
#define VERBOSE 0
#if (VERBOSE)
#define log(format, ...) fprintf(stderr, __DATE__ " " __TIME__ " " format "\n", ## __VA_ARGS__)
#else
#define log(format, ...) do { } while ((void)0,0)
#endif

// Global initialization flag
bool Middleware::global=false;

// Curl handler for the instance
CURL *Middleware::curl=nullptr;

// CPF pre-processing method
//
// Param: Cpf
// Return:
//		Treated CPF (empty if invalid)
std::string Middleware::treatCpf(std::string cpf) {

	log("Verifying CPF [%s]", cpf.c_str());
	std::string treatedCpf("");
	try {
		// Removing non-numbers...
		treatedCpf = std::regex_replace(cpf, std::regex(R"([^0-9])"), "");
	}
	catch (const std::regex_error& err) {
		log("Problem treating CPF");
	}
	log("Post-processed CPF [%s]", treatedCpf.c_str());
	return treatedCpf;
}

// Method to convert HTTP code of SERPRO response
//
// Param: code
// Return:
//		200 (Existant CPF)
//		400 (Invalid CPF format)
//		403 (CPF not regular or not existant)
//		500 (Communication problem)
int Middleware::convertHttpCode(int code) {
	switch (code) {
		case 200:
				log("[SERPRO] Status code 200: Request has been succeeded");
				return CODE_200_CPF_OK;
		case 206:
				log("[SERPRO] Status code 206: Warning, Partial content returned");
				return CODE_200_CPF_OK;
		case 400:
				log("[SERPRO] Status code 400: Invalid CPF format");
				return CODE_400_INVALID_FORMAT;
		case 401:
				log("[SERPRO] Status code 401: Unauthorized, please review the app TOKEN");
				return CODE_500_SERVER_PROBLEM;
		case 404:
				log("[SERPRO] Status code 404: Not existant CPF");
				return CODE_403_SUBJECT_REJECTED;
		case 500:
				log("[SERPRO] Status code 500: Internal Server error");
				return CODE_500_SERVER_PROBLEM;
		default:
				log("[SERPRO] Unknown Status code [%d]", code);
				return CODE_500_SERVER_PROBLEM;
	}
	return CODE_200_CPF_OK;
}

// Method to treat received JSON on SERPRO response
//
// Param: body
// Return:
//		200 (CPF OK)
//		403 (CPF not regular or not existant)
//		500 (Communication problem)
int Middleware::treatResponseData(std::string& body) {

	// Body example:
	// {"ni":"40442820135","nome":"Nome","situacao":{"codigo":"0","descricao":"Regular"}}

	std::size_t pos = body.find("codigo");
  if (pos == std::string::npos || pos+9 >= body.length()) {
		log("[SERPRO] Problem trying to decode received JSON [%s]", body.c_str());
		return CODE_500_SERVER_PROBLEM;
	}
	log("[SERPRO] CPF Status Code: [%c]", body[pos+9]);
	if (body[pos+9] != '0') {
		return CODE_403_SUBJECT_REJECTED;
	}
	return CODE_200_CPF_OK;
}

// Curl callback to write the received data.
// References to CURLOPT_WRITEFUNCTION from Lib Curl documentation.
//
// Param: ptr received data
// Param: size block length
// Param: nmemb amount of blocks
// Param: userdata set by CURLOPT_WRITEDATA
//
// Return: amount of treated bytes.
size_t Middleware::write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {

	if (userdata != nullptr && ptr != nullptr) {
		((std::string*)userdata)->append((char*)ptr, size * nmemb);
	}
	return size*nmemb;
}

// Method to validate CPF
//
// Param: Cpf
// Return:
//		200 (CPF OK)
//		400 (Invalid CPF format)
//		403 (CPF not regular or not existant)
//		500 (Communication problem)
int Middleware::validateCpf(const char * cpf) {

	std::string treatedCpf = treatCpf(cpf);
	if (treatedCpf.length() == 0) {
		log("Invalid CPF format [%s]", cpf);
		return CODE_400_INVALID_FORMAT;
	}

	log("[SERPRO] Creating Request...");

	// Initialization

	if (!global){
		int res = curl_global_init(CURL_GLOBAL_ALL);
		if (res != 0) {
			log("Problem during curl_global_init: [%d]", res);
			return CODE_500_SERVER_PROBLEM;
		}
		global=true;
	}

	if (curl==nullptr) {
		if (!(curl = curl_easy_init())) {
			log("Problem during curl_easy_init.");
			return CODE_500_SERVER_PROBLEM;
		}
	}
	else {
		curl_easy_reset(curl);
	}

	// Setting Curl properties

	if (curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY ) != CURLE_OK) {
		log("Problem setting CURLOPT_HTTPAUTH.");
	}
	if (curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L) != CURLE_OK) {
		log("Problem setting CURLOPT_FAILONERROR.");
	}
	if (curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L) != CURLE_OK) {
		log("Problem setting CURLOPT_NOSIGNAL.");
	}
	if (curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, -1L) != CURLE_OK) {
		log("Problem setting CURLOPT_DNS_CACHE_TIMEOUT.");
	}

	// Setting Function to get the returned Body

	if (curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &Middleware::write_callback) != CURLE_OK) {
		log("Problem setting CURLOPT_WRITEFUNCTION.");
		return CODE_500_SERVER_PROBLEM;
	}
	std::string curlOutput;
	if (curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlOutput) != CURLE_OK) {
		log("Problem setting CURLOPT_WRITEDATA.");
		return CODE_500_SERVER_PROBLEM;
	}

	// Composing URL

	std::string completeUrl(SERPRO_URL);
	completeUrl+=treatedCpf;
	if (curl_easy_setopt(curl, CURLOPT_URL, completeUrl.c_str()) != CURLE_OK) {
		log("Problem setting CURLOPT_URL.");
		return CODE_500_SERVER_PROBLEM;
	}

	// Setting Authentication TOKEN

	struct curl_slist *headers = NULL;
	if ((headers = curl_slist_append(headers, REQUEST_HEADER)) == nullptr) {
		log("Problem during curl_slist_append.");
	}
	else if (curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers) != CURLE_OK) {
		log("Problem setting CURLOPT_HTTPHEADER.");
	}

	// Actually calling the URL

	CURLcode res;
	res = curl_easy_perform(curl);

	// Cleaning

	if (headers != NULL) {
		curl_slist_free_all(headers);
	}

	// Treating Response HTTP Code

	long http_code = 0;
	if (curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code) != CURLE_OK) {
		log("Problem obtaining CURLINFO_RESPONSE_CODE.");
		return CODE_500_SERVER_PROBLEM;
	}
	if ((http_code = convertHttpCode(http_code)) != CODE_200_CPF_OK) {
		return http_code;
	}

	// Treating Response JSON data

	return treatResponseData(curlOutput);
}
