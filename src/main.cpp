#include <pybind11/pybind11.h>
#include <vector>
#include <iostream>
#include <string>
#include "GF(65536).h"
#include "shamir.h"
#include <pybind11/stl.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

int add(int i, int j) {
    return i + j;
}

namespace py = pybind11;

typedef GF65536::point point;
typedef GF65536::operand operand;

#define TO_SHORT(char1, char2) ( ((unsigned short)(unsigned char)char1) | ((unsigned short)(unsigned char)char2 << 8) )
#define GET_C1(value) ( (unsigned char)(value & 0xff) )
#define GET_C2(value) ( (unsigned char)((value & 0xff00) >> 8) )


std::vector<py::bytes> pack(shamir::shares* shares) {
	std::vector<py::bytes> ret(shares->size());
	int i = 0;
	int j;
	char c1, c2, c3, c4;
	for (auto& share : (*shares)) {
		std::string str(share.size() << 2, ' ');
		j = 0;
		for (auto& p : share) {
			c1 = GET_C1(p.x.num);
			c2 = GET_C2(p.x.num);
			c3 = GET_C1(p.y.num);
			c4 = GET_C2(p.y.num);
			str[j] = c1;
			str[j + 1] = c2;
			str[j + 2] = c3;
			str[j + 3] = c4;
			j += 4;
		}
		ret[i++] = py::bytes(str);
	}
	return ret;
}


shamir::shares* unpack(std::vector<py::bytes> shares) {
	shamir::shares* ret = new shamir::shares(shares.size());
	int i = 0;
	for (auto& share : shares) {
		std::string str(share);
		int len = str.length();
		for (int j = 0; j < len; j += 4) {
			point temp;
			operand x, y;
			x = TO_SHORT(str[j], str[j + 1]);
			y = TO_SHORT(str[j + 2], str[j + 3]);
			temp.x = x;
			temp.y = y;
			(*ret)[i].push_back(temp);
		}
		i++;
	}
	return ret;
}


std::vector<py::bytes> create_shares(std::string secret, int threshold, int num_shares) {
	shamir::scheme scheme1(num_shares, threshold);
	shamir::shares* shares = scheme1.createShares(secret);
	return pack(shares);
}


py::bytes combine_shares(std::vector<py::bytes> shares) {
	int k = shares.size();
	shamir::scheme scheme1(k, k);
	shamir::shares* s = unpack(shares);
	return scheme1.getSecret(s);
}


PYBIND11_MODULE(flwr_crypto_cpp, m) {
	m.doc() = "shamir's secret sharing algorithm in C++.";
	m.def("create_shares", &create_shares, "create shares of a given secret",
		py::arg("secret"), py::arg("threshold"), py::arg("num_shares"));
	m.def("combine_shares", &combine_shares, "combine shares to reconstruct a secret",
		py::arg("shares"));

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
