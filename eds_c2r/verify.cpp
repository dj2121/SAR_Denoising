#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <cstdlib>

using namespace std;


//START HELPER STRUCTS
//
typedef struct cNum
{
    float r;
    float i;
    
} cNum;
//
//END HELPER STRUCTS

//START HELPER FUNCTIONS
//

//Get nth line from a file
string getLine(string filename, int n){
    ifstream inFile;
    string data;

    inFile.open(filename);

    for(int i = 0; i < n; i++){
        getline(inFile, data);
    }

    getline(inFile, data); 
    inFile.close(); 
    return data;
} 


vector<float> vecFromFile(string filename, int size){
    vector<float> vec(size, 0);

    ifstream rf(filename, ios::out | ios::binary);
    if(!rf) {
        cout << "Cannot open file!" << endl;
        exit(0);
    }

    for(int i = 0; i < size; i++){
        rf.read((char *) &vec[i], sizeof(float));
    }
    rf.close();

    if(!rf.good()) {
        cout << "Error occurred at reading time!" << endl;
        exit(0);
    }

    return vec;
}


vector<float> getLinespace(float lb, float ub, int num){
    vector<float> out(num, 0);
    float step = (ub-lb)/(num-1);
    for(int i = 0; i < num; i++){
        out[i] = round(lb + (i*step));
    }
    return out;
}


//
//END HELPER FUNCTIONS




int main(){

    int nRows, nCols;

    string dir = "result/";
    string configFile = dir + "config.txt";
    
    nRows = stoi(getLine(configFile, 1));
    nCols = stoi(getLine(configFile, 4));

    int size = nRows * nCols;

    string file1 = dir + "C11.bin";
    string file2 = dir + "C12_real.bin";
    string file3 = dir + "C12_imag.bin";
    string file4 = dir + "C22.bin";

    vector<float> c11 = vecFromFile(file1, size);
    vector<float> c12r = vecFromFile(file2, size);
    vector<float> c12i = vecFromFile(file3, size);
    vector<float> c22 = vecFromFile(file4, size);

    dir = "result/python/";

    string file21 = dir + "C11.bin";
    string file22 = dir + "C12_real.bin";
    string file23 = dir + "C12_imag.bin";
    string file24 = dir + "C22.bin";

    vector<float> cc11 = vecFromFile(file21, size);
    vector<float> cc12r = vecFromFile(file22, size);
    vector<float> cc12i = vecFromFile(file23, size);
    vector<float> cc22 = vecFromFile(file24, size);

    float match = 0;
    int minR = 0, maxR = c11.size();

    for(int i = 0; i < c11.size(); i++){
        if(abs(c11[i] - cc11[i]) < 0.01){
            match += 1;
        }
    }

    cout << "Match % c11: " << (match * 100 / maxR) << endl;

    cout << endl << endl;

    match = 0;

    for(int i = 0; i < c12r.size(); i++){
        if(abs(c12r[i] - cc12r[i]) < 0.01){
            match += 1;
        }
    }

    cout << "Match % c12r: " << (match * 100 / maxR) << endl;

    cout << endl << endl;

    match = 0;

    for(int i = 0; i < c12i.size(); i++){
        if(abs(c12i[i] - cc12i[i]) < 0.01){
            match += 1;
        }
    }

    cout << "Match % c12i: " << (match * 100 / maxR) << endl;


    cout << endl << endl;

    match = 0;

    for(int i = 0; i < c22.size(); i++){
        if(abs(c22[i] - cc22[i]) < 0.01){
            match += 1;
        }
    }

    cout << "Match % c22: " << (match * 100 / maxR) << endl;


    cout << endl << endl;

    return 0;
}