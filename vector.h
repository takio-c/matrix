#include <iostream>
#include <cstring>

template <class T>
class Vector {
protected:
	std::string	tag;
	T*			val;
	bool		ref;
	int			num;
	int			stp;

private:
	void init(std::string name="") {
		tag = name;
		val = NULL;
		ref = false;
		num = 0;
		stp = 0;
	}
	void create(const int n) {
		if(n <= 0) throw *this;
		val = new T [n];
		num = n;
		stp = 1;
		ref = false;
		std::memset(val, 0x00, sizeof(T)*n);
	}
	void reference(T* p, const int n, const int s=1) {
		if(p == NULL || n <= 0) throw;
		val = p;
		num = n;
		stp = s;
		ref = true;
	}
	void release(void) {
		if(ref == false && val != NULL){
			delete [] val;
		}
	}

public:
	Vector(const int n, std::string name="") {
		init(name);
		create(n);
	}
	Vector(T* p, const int n, const int s=1, std::string name="") {
		init(name);
		reference(p, n, s);
	}
	Vector(const Vector* p) {
		if(p == NULL) throw;
		init(p->tag);
		reference(p->val, p->num, p->stp);
	}
	Vector(const Vector& o) {
		init(o.tag);
		create(o.num);
		*this = o;
	}
	virtual ~Vector() {
		release();
	}

/* implement */
	// util
	virtual Vector& SetName(std::string name) {
		tag = name;
		return (*this);
	}
	virtual int Size(void) {
		return num;
	}
	virtual Vector& Set(const T* p, const int s=1) {
		if(p == NULL) throw;
		for(int i = 0; i < num; i++){
			(*this)[i] = *(p + s*i);
		}
		return (*this);
	}
	// uni
	virtual T& operator [](const int i) {
		if(i < 0 || num < i) throw;
		return *(val + stp*i);
	}
	virtual const T& operator [](const int i) const {
		if(i < 0 || num < i) throw;
		return *(val + stp*i);
	}
	virtual const Vector& operator =(const Vector &s0) {
		if(this->num != s0.num) throw;
		for(int i = 0; i < num; i++){
			(*this)[i] = s0[i];
		}
		return (*this);
	}
	virtual const Vector& operator +=(const Vector &s0) {
		if(this->num != s0.num) throw;
		for(int i = 0; i < num; i++){
			(*this)[i] += s0[i];
		}
		return (*this);
	}
	virtual const Vector& operator -=(const Vector &s0) {
		if(this->num != s0.num) throw;
		for(int i = 0; i < num; i++){
			(*this)[i] -= s0[i];
		}
		return (*this);
	}
	// duo
	virtual Vector operator +(const Vector &s0) {
		if(this->num != s0.num) throw;
		Vector<T> d0(num);
		d0.tag = "(" + this->tag + " + " + s0.tag;
		for(int i = 0; i < num; i++){
			d0[i] = (*this)[i] + s0[i];
		}
		return d0;
	}
	virtual Vector operator -(const Vector &s0) {
		if(this->num != s0.num) throw;
		Vector<T> d0(num);
		d0.tag = "(" + this->tag + " - " + s0.tag;
		for(int i = 0; i < num; i++){
			d0[i] = (*this)[i] - s0[i];
		}
		return d0;
	}
	virtual T operator *(const Vector &s0) {
		if(this->num != s0.num) throw;
		T v = 0;
		for(int i = 0; i < num; i++){
			v += (*this)[i] * s0[i];
		}
		return v;
	}
	virtual Vector mul(const T v) {
		Vector<T> d0(num);
		for(int i = 0; i < num; i++){
			d0[i] = (*this)[i] * v;
		}
		return d0;
	}
	virtual Vector div(const T v) {
		Vector<T> d0(num);
		for(int i = 0; i < num; i++){
			d0[i] = (*this)[i] / v;
		}
		return d0;
	}
	virtual Vector cut(int o, int n) {
		if(num < (o+n) || o < 0) throw;
		Vector<T> d0(val+o*stp, n, stp);
		return d0;
	}

/* debug */
	virtual Vector& print(void) {
		return print(std::cout);
	}
	virtual Vector& print(std::ostream &os) {
		os << "#\t";
		for(int i = 0; i < num; i++){
			os.width(10);
			os << (*this)[i] << "\t";
		}
		os << "\t# Vector: " << tag << std::endl;
		return *this;
	}
};

