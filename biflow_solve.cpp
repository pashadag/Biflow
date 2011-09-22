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
#include "defs.h"

using namespace std;
ostringstream oMsg;
string sbuf;

vector<int> nodeMap; //this map is in one direction for 1...numNodes and backwards for numNodes+1...end
ifstream fIn;
ofstream fOut;
char ch;
int numNodes, numArcs, origNumNodes;
int srcNode, dstNode, newSrc, newDst, nodeIndex, balance;
string sLine, lowBound, cap, cost, lineType, problemType, key;
string filename, outFilename, inFilename;
bool sawProblemLine = false;
bool startedArcs = false;
int paramCount = 0;

string make_key(int x, int y) {
	ostringstream o;
	o << x << "," << y;
	return o.str();
}

void mapArc(int & src, int & dst) {
	if ((src > 0) && (nodeMap.at(src) != -1)) {
		src = nodeMap.at(src);
	} 
	if ((dst < 0) && (nodeMap.at(-1 * dst) != -1)) {
		dst = -1*nodeMap.at(-1 * dst);
	}
	return;
}

void usage(int argc, char * argv[]) {
	cout << "Usage: " << argv[0] << " -i <input_file> -o <output_file>" << endl; //{<optparam3>} 
	exit(1);
}


void read_input(int argc, char * argv[]) {

	while ((ch = getopt(argc, argv, "i:o:")) != -1) {
		switch (ch) {
			case 'i':
				inFilename = optarg;
				fIn.open(optarg, ifstream::in);
				if (fIn.fail()) {
					cerr << "Cannot open file " << optarg << endl;
					exit(1);
				}
				paramCount++;
				break;
			case 'o':
				outFilename = optarg;
				fOut.open(optarg);
				if (fOut.fail()) {
					cerr << "Cannot open file " << optarg << endl;
					exit(1);
				}
				paramCount++;
				break;
		}
	}

	if (paramCount != 2) usage(argc,argv);
	assert(fIn.is_open() && fOut.is_open());

	return;
}

int main(int argc, char * argv[]) {
	read_input(argc,argv);

	for (int i = 0; i < 63; i++) fOut << " "; //save the first 64 characters of the file to later put the "p" line here
	fOut << "\n";

	while (getline(fIn, sLine)) { //first pass over input to mark down the nodes that will be split in nodeMap
		istringstream stmLine(sLine);
		stmLine >> lineType;
		if (lineType == "p") {
			stmLine >> problemType >> numNodes >> numArcs;
			origNumNodes = numNodes;
			nodeMap.resize(numNodes + 1, -1); //we will not use the zero element
			sawProblemLine = true;
		} else if (lineType == "n") {
			assert(sawProblemLine);
			stmLine >> nodeIndex >> balance;
			fOut << "n\t" << nodeIndex << "\t" << balance << endl;
			assert(nodeIndex > 0);
			if (stmLine >> lowBound >> cap >> cost) {
				if (balance != 0) {
					cerr << "Abort: not allowed to have costs or bounds on a vertex with non-zero balance.";
					exit(1);
				}
				nodeMap.at(nodeIndex) = ++numNodes;
				nodeMap.push_back(nodeIndex);
				fOut << "n\t" << numNodes << "\t0" << endl;
				numArcs++;
			} 
		} else if (lineType == "a") {
			break;
		} else if (lineType == "c") {
			fOut << sLine << endl;
		}
	}

	fIn.seekg(ios_base::beg);
	while (getline(fIn, sLine)) { //second pass to generate the flow problem file with the split nodes
		istringstream stmLine(sLine);
		stmLine >> lineType;

		if (lineType == "n") {
			stmLine >> nodeIndex >> balance;
			if (stmLine >> lowBound >> cap >> cost) {
				fOut << "a\t" << nodeIndex << "\t" << nodeMap.at(nodeIndex) << "\t" << lowBound << "\t" << cap << "\t" << cost << endl;
			}
		} else if (lineType == "a") {
			startedArcs = true;
			stmLine >> srcNode >> dstNode >> lowBound >> cap >> cost;
			mapArc(srcNode, dstNode);
			fOut << "a\t" << srcNode << "\t" << dstNode << "\t" << lowBound << "\t" << cap << "\t" << cost << endl;
		} else if ((lineType == "c") && (startedArcs)) {
			fOut << sLine << endl;
		}
	}

	fOut.seekp(ios_base::beg);
	fOut << "p min\t" << numNodes << "\t"  << numArcs; //put this line at the beginning of the file.
	fOut.close();

	oMsg.str("");
	oMsg << "biflow_solve_novert " << outFilename << " > " << outFilename << ".b2m_output";
	if (system(oMsg.str().c_str()) != 0) {  // unfortunately, right now bidir2mon doesn't return the error code correctly
		cerr << "Abort: error while executing bidir2mon\n";
		exit(1);
	}


	float flow;
	map<string,float>::iterator it;
	map<string,float> hash;
	oMsg.str("");
	oMsg << outFilename << ".b2m_output";
	ifstream fIn2; 
	fIn2.open(oMsg.str().c_str());
	while (getline(fIn2, sLine)) { //pass over flow output to read in and store results into hash
		istringstream stmLine(sLine);
		stmLine >> lineType;
		if (lineType == "a") {
			stmLine >> srcNode >> dstNode >> lowBound >> cap >> cost >> flow;
			key = make_key(srcNode, dstNode);
			it = hash.find(key);
			if (it == hash.end()) {
				hash.insert(make_pair(key, flow));
			} else {
				it->second += flow;
			}
		}
	}

	//cerr << "Map Contents:\n" << hash;
	fIn2.close();
	ifstream fIn3;
	fIn3.open(inFilename.c_str());
	fOut.open(outFilename.c_str());
	while (getline(fIn3, sLine)) { //third pass over original problem file, this time appending with the results stored in hash.
		istringstream stmLine(sLine);
		stmLine >> lineType;
		if (lineType == "a") {
			stmLine >> srcNode >> dstNode >> lowBound >> cap >> cost >> flow;
			newSrc = srcNode; newDst = dstNode;
			mapArc(newSrc, newDst);
			key = make_key(newSrc, newDst);
			//cerr << "srcNode = " << srcNode << "\ndstNode = " << dstNode << "\nline = " << sLine << "\nkey = " << key << endl;
			it = hash.find(key);
			assert (it != hash.end());
			fOut << "a\t" << srcNode << "\t" << dstNode << "\t" << lowBound << "\t" << cap << "\t" << cost << "\t" << it->second << endl;
		} else if (lineType == "n") {
			stmLine >> nodeIndex >> balance;
			if (stmLine >> lowBound >> cap >> cost) {
				key = make_key(nodeIndex, nodeMap.at(nodeIndex));
				it = hash.find(key);
				assert (it != hash.end());
				flow = it->second;
				fOut << "n\t" << nodeIndex << "\t" << balance << "\t" << lowBound << "\t" << cap << "\t" << cost << "\t" << flow << endl;
			} else {
				fOut << "n\t" << nodeIndex << "\t" << balance << endl;
			}
		} else {
			fOut << sLine << endl;
		}
	}

	return 0;
}

