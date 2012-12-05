#include <iostream>
#include <sstream>
#include "vector.h"

template <class T>
class Matrix {
protected:
	std::string	tag;
	T			*val;
	Vector<T>	**pvv;
	Vector<T>	**pvh;
	bool		ref;
	int			row;
	int			col;

private:
	void init(std::string name="") {
		tag = name;
		val = NULL;
		pvv = NULL;
		pvh = NULL;
		ref = false;
		row = 0;
		col = 0;
	}
	void create(const int r, const int c) {
		if(r <= 0 || c <= 0) throw;
		val = new T [r*c];
		ref = false;
		row = r;
		col = c;
		vectorize(val,1,c);
		memset(val, 0x00, sizeof(T)*r*c);
	}
	void reference(T* p, const int r, const int c, const int s, const int d) {
		if(p == NULL || r <= 0 || c <= 0) throw;
		ref = true;
		row = r;
		col = c;
		vectorize(p,s,d);
	}
	void reference(const Matrix<T> *p, const int ofr, int ofc, int r, int c) {
		if(p == NULL || ofr < 0 || ofc < 0 || r <= 0 || c <= 0) throw;
		if(p->row < (ofr+r) || p->col < (ofc+c)) throw
		ref = true;
		row = r;
		col = c;
		pvv = new Vector<T>* [c];
		pvh = new Vector<T>* [r];
		for(int i = 0; i < c; i++){
			pvv[i] = new Vector<T>(p->pvv[i+ofc], ofr, r);
		}
		for(int i = 0; i < r; i++){
			pvh[i] = new Vector<T>(p->pvh[i+ofr], ofc, c);
		}
	}
	void vectorize(T *p, const int s, const int d) {
		pvv = new Vector<T>* [col];
		pvh = new Vector<T>* [row];
		for(int i = 0; i < col; i++){
			pvv[i] = new Vector<T>(p + s*i, row, d);
		}
		for(int i = 0; i < row; i++){
			pvh[i] = new Vector<T>(p + d*i, col, s);
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
	}

public:
	Matrix(const int r, const int c, std::string name="") {
		init(name);
		create(r,c);
	}
	Matrix(T *p, const int r, const int c, std::string name="") {
		init(name);
		reference(p,r,c,1,c);
	}
	Matrix(T *p, const int r, const int c, const int s, const int d, std::string name="") {
		init(name);
		reference(p,r,c,s,d);
	}
	Matrix(const Matrix<T> *p, const int ofr, const int ofc, const int r, const int c) {
		if(p == NULL) throw;
		init(p->tag);
		reference(p, ofr, ofc, r, c);
	}
	Matrix(const Matrix<T> *p) {
		if(p == NULL) throw;
		init(p->tag);
		reference(p, 0, 0, p->row, p->col);
	}
	Matrix(const Matrix<T>& o) {
		init(o.tag);
		if(o.ref){
			reference(&o, 0, 0, o.row, o.col);
		}
		else{
			create(o.row, o.col);
			*this = o;
		}
	}
	virtual ~Matrix() {
		release();
	}
	static Matrix<T> ide(int num) {
		Matrix<T> d0(num,num);
		return d0.ide();
	}

/* implement */
	// util
	virtual Matrix<T>& SetName(std::string name) {
		tag = name;
		return (*this);
	}
	virtual int Row(void) {
		return row;
	}
	virtual int Col(void) {
		return col;
	}
	virtual Matrix<T>& Set(const T *p) {
		return Set(p,1,col);
	}
	virtual Matrix<T>& Set(const T *p, const int s, const int d) {
		if(p == NULL) throw;
		for(int i = 0; i < row; i++){
			for(int j = 0; j < col; j++){
				(*this)[i][j] = *(p + d*i + s*j);
			}
		}
		return *this;
	}
	virtual Matrix<T>& Dia(const T *p, const int s=1) {
		if(p == NULL) throw;
		for(int i = 0; i < row; i++){
			(*this)[i][i] = *(p + s*i);
		}
		return *this;
	}
	// uni
	virtual Vector<T>& operator [](const int i) {
		if(i < 0 || row < i) throw;
		return *(pvh[i]);
	}
	virtual const Vector<T>& operator [](const int i) const {
		if(i < 0 || row < i) throw;
		return *(pvh[i]);
	}
	virtual Vector<T>& operator ()(const int i) {
		if(i < 0 || col < i) throw;
		return *(pvv[i]);
	}
	virtual const Vector<T>& operator ()(const int i) const {
		if(i < 0 || col < i) throw;
		return *(pvv[i]);
	}
	virtual const Matrix<T>& operator =(const Matrix<T> &s0) {
		if(this->row != s0.row) throw;
		for(int i = 0; i < row; i++){
			(*this)[i] = s0[i];
		}
		return *this;
	}
	virtual const Matrix<T>& operator +=(const Matrix<T> &s0) {
		if(this->row != s0.row) throw;
		for(int i = 0; i < row; i++){
			(*this)[i] += s0[i];
		}
		return *this;
	}
	virtual const Matrix<T>& operator -=(const Matrix<T> &s0) {
		if(this->row != s0.row) throw;
		for(int i = 0; i < row; i++){
			(*this)[i] -= s0[i];
		}
		return *this;
	}
	virtual Matrix<T> ide() {
		if(row != col) throw;
		Matrix<T> d0(row, col);
		d0.tag += "'IDE";
		for(int i = 0; i < row; i++){
			d0[i][i] = (T)(1);
		}
		return d0;
	}
	virtual Matrix<T> tra() {
		Matrix<T> d0(this);
		Vector<T> **pp = d0.pvv;
		int i = d0.col;
		d0.tag += "'TRA";
		d0.pvv = d0.pvh;
		d0.pvh = pp;
		d0.col = d0.row;
		d0.row = i;
		return d0;
	}
	virtual Matrix<T> inv() {
		if(row != col) throw;
		Matrix<T> inv(row, col);
		Matrix<T> src(*this);
		inv.tag += "'INV";
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
			if(src[i][i] == (T)(0)){
				throw *this;
			}
			else{
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
	virtual Matrix<T> mul(const T v) {
		Matrix<T> d0(row, col);
		for(int i = 0; i < row; i++){
			d0[i] = (*this)[i].mul(v);
		}
		return d0;
	}
	virtual Matrix<T> div(const T v) {
		Matrix<T> d0(row, col);
		for(int i = 0; i < row; i++){
			d0[i] = (*this)[i].div(v);
		}
		return d0;
	}
	virtual Matrix<T> cut(int ofr, int ofc, int r, int c) {
		Matrix<T> d0(this, ofr, ofc, r, c);
		return d0;
	}
	// duo
	virtual Matrix<T> operator +(const Matrix<T> &s0) {
		if(this->row != s0.row) throw;
		Matrix<T> d0(row, col);
		d0.tag = "(" + this->tag + " + " + s0.tag + ")";
		for(int i = 0; i < row; i++){
			d0[i] = (*this)[i] + s0[i];
		}
		return d0;
	}
	virtual Matrix<T> operator -(const Matrix<T> &s0) {
		if(this->row != s0.row) throw;
		Matrix<T> d0(row, col);
		d0.tag = "(" + this->tag + " - " + s0.tag + ")";
		for(int i = 0; i < row; i++){
			d0[i] = (*this)[i] - s0[i];
		}
		return d0;
	}
	virtual Matrix<T> operator *(const Matrix<T> &s0) {
		if(this->col != s0.row) throw;
		Matrix<T> d0(this->row, s0.col);
		d0.tag = "(" + this->tag + " * " + s0.tag + ")";
		for(int i = 0; i < d0.row; i++){
			for(int j = 0; j < d0.col; j++){
				d0[i][j] = (*this)[i] * s0(j);
			}
		}
		return d0;
	}

/* debug */
	virtual Matrix<T>& print(void) {
		return print(std::cout);
	}
	virtual Matrix<T>& print(std::ostream &os) {
		os << "# Matrix: " << tag << std::endl;
		for(int i = 0; i < row; i++){
			(*this)[i].print(os);
		}
		return *this;
	}
};

