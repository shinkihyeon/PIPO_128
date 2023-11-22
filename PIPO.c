#include "main.h"

void AddRoundKey(byte* input, byte* rk) {
	*(input + 0) ^= *(rk + 0);
	*(input + 1) ^= *(rk + 1);
	*(input + 2) ^= *(rk + 2);
	*(input + 3) ^= *(rk + 3);

	*(input + 4) ^= *(rk + 4);
	*(input + 5) ^= *(rk + 5);
	*(input + 6) ^= *(rk + 6);
	*(input + 7) ^= *(rk + 7);
}


void S_Layer(byte* X) {
	//버퍼
	byte T[3] = { 0, };

	//연산시작
	X[2] = X[2] ^ (X[0] & X[1]);
	X[3] = X[3] ^ (X[4] & X[2]);
	X[0] = X[0] ^ X[3];

	X[1] = X[1] ^ X[4];
	X[4] = X[4] ^ (X[3] | X[2]);
	X[2] = X[2] ^ X[0];

	X[3] = X[3] ^ (X[2] & X[1]);
	X[5] = X[5] ^ (X[6] & X[7]);
	X[7] = X[7] ^ (X[5] | X[6]);
	X[6] = X[6] ^ (X[5] | X[7]);

	X[5] = ~X[5];
	X[0] = X[0] ^ X[6];
	X[4] = X[4] ^ X[5];
	X[3] = X[3] ^ X[7];

	T[0] = X[0];
	T[1] = X[4];
	T[2] = X[3];

	X[1] = X[1] ^ (T[0] & X[2]);
	T[0] = T[0] ^ X[1];

	X[1] = X[1] ^ (T[2] | T[1]);
	T[1] = T[1] ^ X[2];

	X[2] = X[2] ^ (X[1] | T[2]);
	T[2] = T[2] ^ (T[1] & T[0]);

	X[5] = X[5] ^ T[0];
	T[0] = X[6] ^ T[2];
	X[6] = X[7] ^ T[1];

	X[7] = X[0];
	X[0] = T[0];
	T[1] = X[4];
	X[4] = X[1];
	X[1] = T[1];
	T[2] = X[3];
	X[3] = X[2];
	X[2] = T[2];
}

void R_Layer(byte* X) {
	X[6] = ROL(X[6], 7);
	X[5] = ROL(X[5], 4);
	X[4] = ROL(X[4], 3);
	X[3] = ROL(X[3], 6);
	X[2] = ROL(X[2], 5);
	X[1] = ROL(X[1], 1);
	X[0] = ROL(X[0], 2);
}

void S_Inv_Layer(byte* X) {
	//버퍼
	byte T[3] = { 0, };

	//연산 시작
	T[0] = X[0];
	X[0] = X[7];
	X[7] = X[6];
	X[6] = T[0];
	T[0] = X[0];
	T[1] = X[1];
	T[2] = X[2];

	X[3] = X[3] ^ (X[4] | T[2]);
	X[4] = X[4] ^ (T[2] | T[1]);

	T[1] = T[1] ^ X[3];
	T[0] = T[0] ^ X[4];
	
	T[2] = T[2] ^ (T[1] & T[0]);
	X[4] = X[4] ^ (X[3] & X[0]);

	X[7] = X[7] ^ T[1];
	X[6] = X[6] ^ T[2];
	X[5] = X[5] ^ T[0];

	T[0] = X[4];
	X[4] = X[1];
	X[1] = T[0];
	T[0] = X[2];
	X[2] = X[3];
	X[3] = T[0];

	X[0] = X[0] ^ X[6];
	X[4] = X[4] ^ X[5];
	X[3] = X[3] ^ X[7];

	X[3] = X[3] ^ (X[2] & X[1]);
	X[2] = X[2] ^ X[0];
	X[4] = X[4] ^ (X[3] | X[2]);

	X[1] = X[1] ^ X[4];
	X[0] = X[0] ^ X[3];
	X[3] = X[3] ^ (X[4] & X[2]);
	X[2] = X[2] ^ (X[0] & X[1]);

	X[5] = ~X[5];
	X[6] = X[6] ^ (X[5] | X[7]);
	X[7] = X[7] ^ (X[5] | X[6]);
	X[5] = X[5] ^ (X[6] & X[7]);
}

void R_Inv_Layer(byte* X) {
	X[6] = ROR(X[6], 7);
	X[5] = ROR(X[5], 4);
	X[4] = ROR(X[4], 3);
	X[3] = ROR(X[3], 6);
	X[2] = ROR(X[2], 5);
	X[1] = ROR(X[1], 1);
	X[0] = ROR(X[0], 2);
}

//PIPO-128 기준 key-schedule
//128-bit 키로 14개의 64-bit 라운드키 생성
void pipo_ks(byte* mk, byte rk[14][8]) {
	lg K0 = ((lg)mk[8] << 56) | ((lg)mk[9] << 48) | ((lg)mk[10] << 40) | ((lg)mk[11] << 32) | ((lg)mk[12] << 24) | ((lg)mk[13] << 16) | ((lg)mk[14] << 8) | ((lg)mk[15]);
	lg K1 = ((lg)mk[0] << 56) | ((lg)mk[1] << 48) | ((lg)mk[2] << 40) | ((lg)mk[3] << 32) | ((lg)mk[4] << 24) | ((lg)mk[5] << 16) | ((lg)mk[6] << 8) | ((lg)mk[7]);

	lg K[2] = {0, };
	K[0] = K0;
	K[1] = K1;

	//uint64_t 자료형 rk값을 -> byte배열 rk[14][8]에 넣어주는 과정
	for (int r = 0; r < 14; r++) {
		lg value = K[r % 2] ^ r;//uint64_t 자료형 rk 값
		rk[r][0] = (value >> 56) & 0xff;
		rk[r][1] = (value >> 48) & 0xff;
		rk[r][2] = (value >> 40) & 0xff;
		rk[r][3] = (value >> 32) & 0xff;
		rk[r][4] = (value >> 24) & 0xff;
		rk[r][5] = (value >> 16) & 0xff;
		rk[r][6] = (value >> 8) & 0xff;
		rk[r][7] = (value >> 0) & 0xff;
	}
}

//pipo-128 암호화
void Pipo_Enc(byte* pt, byte* mk, byte* ct) {
	byte copy_pt[8] = { 0, };
	memcpy(copy_pt, pt, 8 * sizeof(byte));

	byte rk[14][8] = { 0, };
	pipo_ks(mk, rk);

	AddRoundKey(copy_pt, rk[0]);

	for (int i = 1; i <= 13; i++) {
		S_Layer(copy_pt);
		R_Layer(copy_pt);
		AddRoundKey(copy_pt, rk[i]);
	}

	//결과 도출
	memcpy(ct, copy_pt, 8 * sizeof(byte));

}

////pipo-128 복호화
void Pipo_Dec(byte* ct, byte* mk, byte* result) {
	byte copy_ct[8] = { 0, };
	memcpy(copy_ct, ct, 8 * sizeof(byte));

	byte rk[14][8] = { 0, };
	pipo_ks(mk, rk);

	for (int i = 13; i >= 1; i--) {
		AddRoundKey(copy_ct, rk[i]);
		R_Inv_Layer(copy_ct);
		S_Inv_Layer(copy_ct);
	}

	AddRoundKey(copy_ct, rk[0]);

	//결과 도출
	memcpy(result, copy_ct, 8 * sizeof(byte));

}

//결과 출력 함수
void printstate(byte* data) {
	for (int i = 0; i < 8; i++) {
		printf("%02x ", data[i]);
	}
	printf("\n");
}

int main() {
	//PIPO TTA 표준문서 기준 테스터 벡터
	byte pt[8] = { 0x09,0x85,0x52,0xf6,0x1e,0x27,0x00,0x26 };
	byte mk[16] = { 0x6d,0xc4,0x16,0xdd,0x77,0x94,0x28,0xd2,0x7e,0x1d,0x20,0xad,0x2e,0x15,0x22,0x97 };
	byte ct[8] = { 0, };
	byte dec_pt[8] = { 0, };

	//pipo-128 암호화
	Pipo_Enc(pt, mk, ct);
	printf("CT:");
	printstate(ct);

	//pipo-128 복호화
	Pipo_Dec(ct, mk, dec_pt);
	printf("Decrypted CT:");
	printstate(dec_pt);
}


