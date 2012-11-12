#include <iostream>
#include "vector.h"

template <class T>
class Matrix {
protected:
	char*		tag;
	T*			val;
	Vector<T>**	pvv;
	Vector<T>**	pvh;
	bool		ref;
	int			row;
	int			col;

private:
	void init(void) {
		tag = NULL;
		val = NULL;
		pvv = 0;
		pvh = 0;
		ref = false;
		row = 0;
		col = 0;
	}
	void create(const int r, const int c) {
		release();
		val = new T [r*c];
		ref = false;
		row = r;
		col = c;
		vectorize(1,c);
		memset(val, 0x00, sizeof(T)*r*c);
	}
	void reference(T* p, const int r, const int c, const int s, const int d) {
		release();
		val = p;
		ref = true;
		row = r;
		col = c;
		vectorize(s,d);
	}
	void vectorize(const int s, const int d) {
		pvv = new Vector<T>* [col];
		for(int i = 0; i < col; i++){
			pvv[i] = new Vector<T>(val + s*i, row, d);
		}
		pvh = new Vector<T>* [row];
		for(int i = 0; i < row; i++){
			pvh[i] = new Vector<T>(val + d*i, col, s);
		}
	}
	void release(void) {
		if(pvv){
			for(int i = 0; i < col; i++){
				delete pvv[i];
			}
			delete [] pvv;
		}
		if(pvh){
			for(int i = 0; i < row; i++){
				delete pvh[i];
			}
			delete [] pvh;
		}
		if(ref == false && val != NULL){
			delete [] val;
		}
		init();
	}

public:
	Matrix(const int r, const int c) {
		init();
		create(r,c);
	}
	Matrix(T* p, const int r, const int c) {
		init();
		reference(p,r,c,1,c);
	}
	Matrix(T* p, const int r, const int c, const int s, const int d) {
		init();
		reference(p,r,c,s,d);
	}
	Matrix(const Matrix* p) {
		init();
		val = p->val;
		pvv = new Vector<T>* [p->col];
		for(int i = 0; i < p->col; i++){
			pvv[i] = new Vector<T>(p->pvv[i]);
		}
		pvh = new Vector<T>* [p->row];
		for(int i = 0; i < p->row; i++){
			pvh[i] = new Vector<T>(p->pvh[i]);
		}
		ref = true;
		row = p->row;
		col = p->col;
	}
	Matrix(const Matrix& o) {
		init();
		create(o.row, o.col);
		*this = o;
	}
	virtual ~Matrix() {
		release();
	}

/* implement */
	// util
	virtual int Row(void) {
		return row;
	}
	virtual int Col(void) {
		return col;
	}
	virtual Matrix& Set(const T* p) {
		return Set(p,1,col);
	}
	virtual Matrix& Set(const T* p, const int s, const int d) {
		for(int i = 0; i < row; i++){
			for(int j = 0; j < col; j++){
				(*this)[i][j] = *(p + d*i + s*j);
			}
		}
		return *this;
	}
	virtual Matrix& Dia(const T* p, const int s=1) {
		for(int i = 0; i < row; i++){
			(*this)[i][i] = *(p + s*i);
		}
		return *this;
	}
	// uni
	virtual Vector<T>& operator [](const int i) {
		return *(pvh[i]);
	}
	virtual const Vector<T>& operator [](const int i) const {
		return *(pvh[i]);
	}
	virtual Vector<T>& operator ()(const int i) {
		return *(pvv[i]);
	}
	virtual const Vector<T>& operator ()(const int i) const {
		return *(pvv[i]);
	}
	virtual const Matrix& operator =(const Matrix &s0) {
		for(int i = 0; i < row; i++){
			(*this)[i] = s0[i];
		}
		return *this;
	}
	virtual const Matrix& operator +=(const Matrix &s0) {
		for(int i = 0; i < row; i++){
			(*this)[i] += s0[i];
		}
		return *this;
	}
	virtual const Matrix& operator -=(const Matrix &s0) {
		for(int i = 0; i < row; i++){
			(*this)[i] -= s0[i];
		}
		return *this;
	}
	virtual Matrix ide() {
		Matrix d0(row, col);
		if(row == col){
			for(int i = 0; i < row; i++){
				d0[i][i] = (T)(1);
			}
		}
		return d0;
	}
	virtual Matrix tra() {
		Matrix d0(this);
		Vector<T>** p = d0.pvv;
		int i = d0.col;
		d0.pvv = d0.pvh;
		d0.pvh = p;
		d0.col = d0.row;
		d0.row = i;
		return d0;
	}
	virtual Matrix inv() {
		Matrix inv(row, col);
		Matrix src(*this);
		inv = inv.ide();
		for(int i = 0; i < row; i++){
			if(src[i][i] == (T)(0)){
				for(int ii = i+1; ii < row; ii++){
					if(src[ii][i] != (T)(0)){
						src[i] += src[ii];
						inv[i] += inv[ii];
						break;
					}
				}
			}
			{
				T v = src[i][i];
				src[i] = src[i].div(v);
				inv[i] = inv[i].div(v);
			}
			for(int ii = 0; ii < row; ii++){
				if(ii != i){
					T v = src[ii][i];
					src[ii] -= src[i].mul(v);
					inv[ii] -= inv[i].mul(v);
				}
			}
		}
		return inv;
	}
	virtual Matrix mul(const T v) {
		Matrix d0(row, col);
		for(int i = 0; i < row; i++){
			d0[i] = (*this)[i].mul(v);
		}
		return d0;
	}
	virtual Matrix div(const T v) {
		Matrix d0(row, col);
		for(int i = 0; i < row; i++){
			d0[i] = (*this)[i].mul(v);
		}
		return d0;
	}
	// duo
	virtual Matrix operator +(const Matrix &s0) {
		Matrix d0(row, col);
		for(int i = 0; i < row; i++){
			d0[i] = (*this)[i] + s0[i];
		}
		return d0;
	}
	virtual Matrix operator -(const Matrix &s0) {
		Matrix d0(row, col);
		for(int i = 0; i < row; i++){
			d0[i] = (*this)[i] - s0[i];
		}
		return d0;
	}
	virtual Matrix operator *(const Matrix &s0) {
		Matrix d0(this->row, s0.col);
		for(int i = 0; i < d0.row; i++){
			for(int j = 0; j < d0.col; j++){
				d0[i][j] = (*this)[i] * s0(j);
			}
		}
		return d0;
	}

/* debug */
	virtual Matrix& print(void) {
		std::cout << "# Matrix: " << "\n";
		for(int i = 0; i < row; i++){
			(*this)[i].print();
		}
		return *this;
	}
};
