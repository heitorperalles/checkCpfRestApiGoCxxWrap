# checkCpfRestApiGoCxxWrap

RESTful API to verify physical person registers status on Brazilian entities.<br>
A GO (Golang) solution manually **wrapped** to C++ code that uses libCurl implementation ([github.com/curl](https://github.com/curl/curl)).

The routing API receives a CPF (Brazilian Physical Person Register) to be checked.<br>
Then the CPF is validated on SERPRO (Brazilian Federal Data Processing Service) that also uses a RESTful API.<br>

The HTTP request implementation is fowarded to **C++ source code** files.<br>
The C++ code links with the pre-installed C/C++ library **libCurl** ([github.com/curl](https://github.com/curl/curl)).<br>
C++ solution is manually reflected to C functions, that GO accesses through  **CGO** ([golang.org/cmd/cgo](https://golang.org/cmd/cgo/)).<br>

The API may also receives more informative data, like Name and RG (Brazilian General Registry), not being validated now.<br>
The API routing implementation uses the library **gorilla/mux** ([github.com/gorilla/mux](https://github.com/gorilla/mux)).

## Quick Start (on Linux)

### Install GO compiler
```bash
sudo apt install gccgo-go
```
This application was tested with GO release `1.12.2`. To check the installed version of GO compiler, run:
```bash
go version
```

### Install Gorilla Mux router library
```bash
go get -u github.com/gorilla/mux
```

### Install lib Curl for Development
```bash
sudo apt-get install libcurl-dev
```
This application was tested with the library `libcurl4-openssl-dev` on release `7.68.0`

### Build and run checkCpfRestApiGoCxxWrap (from the project root folder)
```bash
go build -gccgoflags="-lcurl"
./checkCpfRestApiGoCxxWrap
```
Attention: The `-lcurl` flag is necessary to link with the installed Curl library.


## API description

**URL** : `api/v1/verify`

**Method** : `POST`

**Auth required** : NO

**Permissions required** : None

**Required fields** : `cpf`

**Non-required fields** : `name`, `rg` (`number`, `issued`, `entity`)

**Request example:**

```json
 {
  "name":"Heitor Peralles",
  "cpf":"40442820135",
  "rg": { "number":"209921899", "issued":"2020/5/20", "entity":"DETRAN-RJ" }
 }
```

### Success Response

**Code** : `200 Register OK`

**Response example:**

```json
 {
  "status":"True"
 }
```

### Error Responses

**Code** : `400 Invalid format or registry number`

**Code** : `403 Problem with the registry, NOT OK`

**Code** : `500 Server Error`

**Response example:**

```json
 {
  "status":"False",
  "message":"Invalid CPF Format."
 }
```

## Testing the API

### Unit tests

Unfortunately GO does not support automated testing of applications that use CGO.

### Testing manually

#### A simple way to test the API on Linux:

Install CURL, a recommended library to deal with requests.

``` bash
sudo apt-get install curl
```

Trigger a request to the primary endpoint as follows.

``` bash
curl localhost:8000/api/v1/verify -i -X POST -d '{"cpf":"40442820135"}'
```

Remembering that checkCpfRestApiGoCxxWrap must be running to receive the request.

#### List of testing CPFs provided by SERPRO:

The application uses a service provided by SERPRO, so any test depends on this service being online.<br>
The CPFs provided by SERPRO for testing are:

```
40442820135: Regular (Register OK)
63017285995: Regular (Register OK)
91708635203: Regular (Register OK)
58136053391: Regular (Register OK)
40532176871: Suspended (Problem with the registry)
47123586964: Suspended (Problem with the registry)
07691852312: Regularization Pending (Problem with the registry)
10975384600: Regularization Pending (Problem with the registry)
01648527949: Canceled by Multiplicity (Problem with the registry)
47893062592: Canceled by Multiplicity (Problem with the registry)
98302514705: Null (Problem with the registry)
18025346790: Null (Problem with the registry)
64913872591: Registration Canceled (Problem with the registry)
52389071686: Registration Canceled (Problem with the registry)
05137518743: Deceased Holder (Problem with the registry)
08849979878: Deceased Holder (Problem with the registry)
```

## Implementation

### File structure

The application is composed by a set of source code files:

**app.go** : Main file, with `main()` function.<br>

**api_model.go** : API structures.<br>
**api_routing.go** : API routing implementation using Gorilla Mux.<br>
**api.go** : Code to handle provided API requests and responses.

**middleware.hxx** : C++ header file with definitions of functions and SERPRO structures.<br>
**middleware.cxx** : C++ source file with the core implementation of the REQUEST to SERPRO service.

**middleware_wrap.h** : C header file with C style definitions to be accessed by GO files.<br>
**middleware_wrap.cxx** : C++ source file that forwards C style definitions to C++ source files.

**README.md** : This file, explaining how the application works.<br>
**.gitignore** : List of non-versioned files (such as the compiled binary).

After the compilation process, the non-versioned binary file `checkCpfRestApiGoCxxWrap` will be generated.

### API Credentials

The application is configured with a personal TOKEN, please change it.<br>
Its possible to generate a new TOKEN on SERPRO service page: [servicos.serpro.gov.br](https://servicos.serpro.gov.br/inteligencia-de-negocios-serpro/biblioteca/consulta-cpf/teste.html).

The in use TOKEN is specified on the top of `middleware.cxx` file.<br>
If going to production the SERPRO service URL must also be changed (at the same file).

### Author

Heitor Peralles<br>
[heitorgp@gmail.com](mailto:heitorgp@gmail.com)<br>
[linkedin.com/in/heitorperalles](https://www.linkedin.com/in/heitorperalles)

### Source

[github.com/heitorperalles/checkCpfRestApiGoCxxWrap](https://www.github.com/heitorperalles/checkCpfRestApiGoCxxWrap)

### License

MIT licensed. See the **LICENSE** file for details.
