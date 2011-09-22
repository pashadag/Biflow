#ifndef DEFS_H
#define DEFS_H


#include<cassert>
#include<cmath>
#include<string>
#include<iostream>
#include<sstream>
#include<fstream>
#include<cstdlib>
#include<vector>
#include<map>
#include<list>
#include<queue>
#include<cstdarg>
#include<algorithm>

using namespace std;

template<class T, class U>
ostream & operator << (ostream &out, const map<T,U> & m) {
	typename map<T,U>::const_iterator it;
	for (it = m.begin(); it != m.end(); it++) {
		out << it->first << " -> " << it->second << endl;
	}
	return out;
}

//print vector of vectors
template<class T>
ostream & operator << (ostream & out, const vector<vector<T > > & v) {
	out << v.size() << endl;
	if (v.size() != 0) {
		out << v[0];
		for (int i = 1; i < v.size(); i++) out << endl << v[i];
	}
	return out;
}

//read vector of vectors
template<class T>
istream & operator >> (istream & in, vector<vector<T > > & v) {
	int count;
	vector<T> row;
	in >> count;
	v.reserve(count);
	for (int i = 0; i < count; i++) {
		row.clear();
		in >> row;
		v.push_back(row);
	}

	return in;
}

//print vector of T
template<class T>
ostream & operator << (ostream & out, const vector<T> & v) {
	out << v.size() << '\t';
	if (v.size() != 0) {
		out << v[0];
		for (int i = 1; i < v.size(); i++) out << '\t' << v[i];
	}
	return out;
}
//read vector of T
template<class T>
istream & operator >> (istream & in, vector<T> & v) {
	int count;
	T val;
	in >> count;
	v.reserve(count);
	for (int i = 0; i < count; i++) {
		in >> val;
		v.push_back(val);
	}
	return in;
}




#endif



