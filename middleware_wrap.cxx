#include "middleware.hxx"
#include "middleware_wrap.h"

// Wrapped function to validate CPF
//
// Param: Cpf
// Return:
//		200 (CPF OK)
//		400 (Invalid CPF format)
//		403 (CPF not regular or not existant)
//		500 (Communication problem)
int validateCpf(const char * cpf) {
	return Middleware::validateCpf(cpf);
}
