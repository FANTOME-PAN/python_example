#include <iostream>
#include <vector>
#include <random>
//#include "GF(256).h"
#include "GF(65536).h"
#include "shamir.h"

//typedef GF256::byte operand;
//typedef GF256::point point;
//constexpr int MAX_VALUE = 255;
typedef GF65536::operand operand;
typedef GF65536::point point;
constexpr int MAX_VALUE = 65535;
constexpr unsigned short FIRST8_BITS_MASK = 0xff00;
constexpr unsigned short LAST8_BITS_MASK = 0x00ff;
typedef std::vector<std::vector<GF65536::point>> shares;

shamir::scheme::scheme(int members, int threshold) {
	n = members;
	k = threshold;
}

/*Creating of shares shamir (n,k) scheme*/

shares* shamir::scheme::createShares(std::string secret) {
	std::random_device device;
	std::default_random_engine generator(device());
	std::uniform_int_distribution<int> distribution(0, MAX_VALUE);
	shares* allShares = new shares(n);
	int len = secret.length();
	for (int p = 0; p < len; p += 2) {
		unsigned short c1 = secret[p], c2 = secret[p + 1];
		int data = c1 | (c2 << 8);
		operand* coeff = new operand[k];
		coeff[0] = data;
		for (int i = 1; i < k; i++) {
			coeff[i] = distribution(generator);
		}
		point temp;
		operand x, y;
		for (int i = 0; i < n; i++) {
			x = (int)(i + 1);
			y = 0;
			for (int j = 0; j < k; j++) {
				y = y + (coeff[j] * power(x, j));
			}
			temp.x = x;
			temp.y = y;
			(*allShares)[i].push_back(temp);
		}
	}
	return allShares;
}

std::string shamir::scheme::getSecret(shares* Kshares) {
	if ((*Kshares).size() < k) {
		std::cout << "Atleast " << k << " shares are required out of " << n << " shares" << std::endl;
		return "";
	}
	std::string secret = "";
	int secretSize = (*Kshares)[0].size();
	std::vector<point> curr_shares(k);
	for (int di = 0; di < secretSize; di++) {
		for (int i = 0; i < k; i++) {
			curr_shares[i] = (*Kshares)[i][di];
		}
		unsigned short data = interpolate_gf65536(curr_shares);
		char c1 = data & LAST8_BITS_MASK, c2 = (data & FIRST8_BITS_MASK) >> 8;
		secret += c1;
		secret += c2;
	}
	return secret;
}


void shamir::init() {
}
