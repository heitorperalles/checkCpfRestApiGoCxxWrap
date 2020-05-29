#ifndef MIDDLEWARE_WRAP_H
#define MIDDLEWARE_WRAP_H

#ifdef __cplusplus
extern "C" {
#endif

//  Wrapped function to validate CPF
//
// Param: Cpf
// Return:
//		200 (CPF OK)
//		400 (Invalid CPF format)
//		403 (CPF not regular or not existant)
//		500 (Communication problem)
int validateCpf(const char * cpf);

#ifdef __cplusplus
}
#endif

#endif /* MIDDLEWARE_WRAP_H */
