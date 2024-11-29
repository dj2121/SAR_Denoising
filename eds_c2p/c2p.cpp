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

void vecToFile(vector<vector<cNum> > vec, int channel, int part , string filename){
    ofstream wf(filename, ios::out | ios::binary);
    if(!wf) {
        cout << "Cannot open file " << filename << " for writing" << endl;
        exit(0);
    }

    for(int i = 0; i < vec[channel].size(); i++){
        if(part == 0 || part == 2){
            wf.write((char *) &vec[channel][i].r, sizeof(float));
        }
        if(part == 1 || part == 2){
            wf.write((char *) &vec[channel][i].i, sizeof(float));
        }
    }

    wf.close();

    if(!wf.good()) {
        cout << "Error occurred at writing time for file: " << filename << endl;
        exit(0);
    }

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


vector<vector<cNum> > edsprocc(vector<vector<cNum> > cmat, int nRows, int nCols, int winsize){

    int lineLength = winsize;

    int size = nRows * nCols;

    vector<float> x(2, 0);
    vector<float> y(2, 0);
    x[0] = winsize;
    y[0] = winsize;

    vector<vector<cNum> > fcmat = cmat;

    vector<vector<vector<float> > > dirmask;  
    for(int i = 0; i < 18; i++){
        vector<vector<float> > temp1;
        for(int j = 0; j < 2*winsize+1; j++){
            vector<float> temp2;
            for(int k = 0; k < 2*winsize+1; k++){
                temp2.push_back(0);
            }
            temp1.push_back(temp2);
        }
        dirmask.push_back(temp1);
    }

    vector<float> rchnl(4, 0);


    //eds1 = abs(cmat)
    vector<vector<float> > eds1;
    vector<float> temp(size);
    for(int i = 0; i < size; i++){
        temp[i] = sqrt(pow(cmat[0][i].r, 2) + pow(cmat[0][i].i, 2));
    }
    eds1.push_back(temp);

    for(int i = 0; i < size; i++){
        temp[i] = sqrt(pow(cmat[1][i].r, 2) + pow(cmat[1][i].i, 2));
    }
    eds1.push_back(temp);

    for(int i = 0; i < size; i++){
        temp[i] = sqrt(pow(cmat[2][i].r, 2) + pow(cmat[2][i].i, 2));
    }
    eds1.push_back(temp);

    for(int i = 0; i < size; i++){
        temp[i] = sqrt(pow(cmat[3][i].r, 2) + pow(cmat[3][i].i, 2));
    }
    eds1.push_back(temp);


    //eds2 = np.angle(cmat)
    vector<vector<float> > eds2;
    for(int i = 0; i < size; i++){
        temp[i] = atan2(cmat[0][i].i, cmat[0][i].r);
    }
    eds2.push_back(temp);

    for(int i = 0; i < size; i++){
        temp[i] = atan2(cmat[1][i].i, cmat[1][i].r);
    }
    eds2.push_back(temp);

    for(int i = 0; i < size; i++){
        temp[i] = atan2(cmat[2][i].i, cmat[2][i].r);
    }
    eds2.push_back(temp);

    for(int i = 0; i < size; i++){
        temp[i] = atan2(cmat[3][i].i, cmat[3][i].r);
    }
    eds2.push_back(temp);

    //Initialization over

    int cnt = 0;

    for(int i = 0; i < 171; i = i + 10){
        vector<vector<float> > gker( 2 * winsize + 1 ,vector<float> (2 * winsize + 1, 0));
        x[1] = x[0] + lineLength * cos(i*3.14159265/180);
        y[1] = y[0] + lineLength * sin(i*3.14159265/180);
        
        vector<float> cx = getLinespace(x[0], x[1], 1000);
        vector<float> cy = getLinespace(y[0], y[1], 1000);

        for(int j = 0; j < cx.size(); j++){
            if(cx[j] < gker.size() && cy[j] < gker[0].size()){
                gker[(int)cx[j]][(int)cy[j]] = 1;
            }
        }

        x[1] = x[0] + lineLength * cos((i+180)*3.14159265/180);
        y[1] = y[0] + lineLength * sin((i+180)*3.14159265/180);

        cx = getLinespace(x[0], x[1], 1000);
        cy = getLinespace(y[0], y[1], 1000);

        for(int j = 0; j < cx.size(); j++){
            if(cx[j] < gker.size() && cy[j] < gker[0].size()){
                gker[(int)cx[j]][(int)cy[j]] = 1;
            }
        }

        for(int j = 0; j < gker.size(); j++){
            for(int k = 0; k < gker[0].size(); k++){
                dirmask[cnt][j][k] = gker[j][k];
            }
        }

        cnt++;

    }

    cout << "Directional masks prepared!" << endl;

    for(int i = 0; i < cmat.size(); i++){
        float imag = 0;
        for(int j = 0; j < cmat[i].size(); j++){
            imag += cmat[i][j].i;
        }
        if(imag == 0){
            rchnl[i] = 1;
        }
    }

    cout << "Rows to compute: " << nRows << endl;

    int tcnt = 0;
    int rowLim = nRows - winsize;
    int colLim = nCols - winsize;
    int ws = winsize + winsize + 1;
    vector<float> ttin((ws)*(ws), 0);

    int sumRchnl = 0;
    for(int i = 0; i < rchnl.size(); i++){
        sumRchnl += rchnl[i];
    }

    vector<float> sumDM;
    for(int i = 0; i < dirmask.size(); i++){
        float tempDM = 0;
        for(int j = 0; j < 2*winsize+1; j++){
            for(int k = 0; k < 2*winsize+1; k++){
                tempDM += dirmask[i][j][k];
            }
        }
        sumDM.push_back(tempDM);
    }

    for(int i = winsize; i < rowLim; i++){
        for(int j = winsize; j < colLim; j++){

            vector<vector<float> > dc(cmat.size() ,vector<float> (cnt, 0));
            vector<vector<float> > dcp(cmat.size() ,vector<float> (cnt, 0));
            vector<vector<float> > Dc(sumRchnl ,vector<float> (cnt, 0));

            vector<vector<float> > temp1;
            int ttindex = 0;
            for(int a = i-winsize; a < i+winsize+1; a++){
                for(int b = j-winsize; b < j+winsize+1; b++){
                    ttin[ttindex++] = eds1[0][(a*nCols) + b];
                }
            }
            temp1.push_back(ttin);
            ttindex = 0;
            for(int a = i-winsize; a < i+winsize+1; a++){
                for(int b = j-winsize; b < j+winsize+1; b++){
                    ttin[ttindex++] = eds1[1][(a*nCols) + b];
                }
            }
            temp1.push_back(ttin);
            ttindex = 0;
            for(int a = i-winsize; a < i+winsize+1; a++){
                for(int b = j-winsize; b < j+winsize+1; b++){
                    ttin[ttindex++] = eds1[2][(a*nCols) + b];
                }
            }
            temp1.push_back(ttin);
            ttindex = 0;
            for(int a = i-winsize; a < i+winsize+1; a++){
                for(int b = j-winsize; b < j+winsize+1; b++){
                    ttin[ttindex++] = eds1[3][(a*nCols) + b];
                }
            }
            temp1.push_back(ttin);

            

            vector<vector<float> > temp1p;
            ttindex = 0;
            for(int a = i-winsize; a < i+winsize+1; a++){
                for(int b = j-winsize; b < j+winsize+1; b++){
                    ttin[ttindex++] = eds2[0][(a*nCols) + b];
                }
            }
            temp1p.push_back(ttin);
            ttindex = 0;
            for(int a = i-winsize; a < i+winsize+1; a++){
                for(int b = j-winsize; b < j+winsize+1; b++){
                    ttin[ttindex++] = eds2[1][(a*nCols) + b];
                }
            }
            temp1p.push_back(ttin);
            ttindex = 0;
            for(int a = i-winsize; a < i+winsize+1; a++){
                for(int b = j-winsize; b < j+winsize+1; b++){
                    ttin[ttindex++] = eds2[2][(a*nCols) + b];
                }
            }
            temp1p.push_back(ttin);
            ttindex = 0;
            for(int a = i-winsize; a < i+winsize+1; a++){
                for(int b = j-winsize; b < j+winsize+1; b++){
                    ttin[ttindex++] = eds2[3][(a*nCols) + b];
                }
            }
            temp1p.push_back(ttin);

            for(int k = 0; k < cnt; k++){
                for(int l = 0; l < cmat.size(); l++){


                    float tdc = 0;
                    for(int a = 0; a < dirmask[k].size(); a++){
                        for(int b = 0; b < dirmask[k][a].size(); b++){
                            tdc += (dirmask[k][a][b] * temp1[l][a*ws+b])/sumDM[k];
                        }
                    }
                    dc[l][k] = tdc;

                    float tdcp = 0;
                    for(int a = 0; a < dirmask[k].size(); a++){
                        for(int b = 0; b < dirmask[k][a].size(); b++){
                            tdcp += (dirmask[k][a][b] * temp1p[l][a*ws+b])/sumDM[k];
                        }
                    }
                    dcp[l][k] = tdcp;

                }
            }

            for(int k = 0; k < cnt; k++){
                vector<float> edst;
                for(int l = 0; l < rchnl.size(); l++){
                    if(rchnl[l]==1){
                        edst.push_back(eds1[l][(i*nCols)+j]);
                    }
                }

                vector<float> tdc;
                for(int l = 0; l < rchnl.size(); l++){
                    if(rchnl[l]==1){
                        tdc.push_back(dc[l][k]);
                    }
                }

                for(int l = 0; l < Dc.size(); l++){
                    Dc[l][k] = abs(tdc[l] - edst[l]);
                }
            }

            float aloc = 0;
            float amin = 999999999;
            for(int k = 0; k < Dc.size(); k++){
                for(int l = 0; l < Dc[0].size(); l++){
                    if(Dc[k][l] < amin){
                        amin = Dc[k][l];
                        aloc = l;
                    }
                }
            }

            for(int k = 0; k < fcmat.size(); k++){
                fcmat[k][(i*nRows)+j].r = dc[k][aloc] * cos(dcp[k][aloc]);
                fcmat[k][(i*nRows)+j].i = dc[k][aloc] * sin(dcp[k][aloc]);
            }

        }

        if(i%99 == 0){
            tcnt++;
            cout << "Filtering Progress: " << i+tcnt << "/" << nRows << endl;
        }

    }

    return fcmat;

}




int main(){

    int nRows, nCols;

    string dir = "data/";
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

    vector<cNum> c12(size);
    for(int i = 0; i < size; i++){
        c12[i].r = c12r[i];
        c12[i].i = c12i[i];
    }

    vector<cNum> c21 = c12;

    vector<vector<cNum> > c2;

    vector<cNum> temp(size);
    for(int i = 0; i < size; i++){
        temp[i].r = c11[i];
        temp[i].i = 0;
    }
    c2.push_back(temp);

    for(int i = 0; i < size; i++){
        temp[i].r = c12[i].r;
        temp[i].i = c12[i].i;
    }
    c2.push_back(temp);

    for(int i = 0; i < size; i++){
        temp[i].r = c21[i].r;
        temp[i].i = c21[i].i;
    }
    c2.push_back(temp);

    for(int i = 0; i < size; i++){
        temp[i].r = c22[i];
        temp[i].i = 0;
    }
    c2.push_back(temp);

    c11.clear(); c12i.clear(); c12r.clear(); c22.clear(); c12.clear();

    vector<vector<cNum> > fcmat1 = edsprocc(c2, nRows, nCols, 1);
    vector<vector<cNum> > fcmat2 = edsprocc(c2, nRows, nCols, 3);
    vector<vector<cNum> > fcmat3 = edsprocc(fcmat1, nRows, nCols, 2);
    vector<vector<cNum> > fcmat4 = edsprocc(c2, nRows, nCols, 5);
    vector<vector<cNum> > fcmat5 = edsprocc(fcmat4, nRows, nCols, 2);
    vector<vector<cNum> > fcmat6 = edsprocc(fcmat2, nRows, nCols, 1);

    vector<vector<cNum> > fcmat = c2;

    for(int i = 0; i < fcmat.size(); i++){
        for(int j = 0; j < fcmat[0].size(); j++){
            float lr = 0, li = 0;
            lr += fcmat1[i][j].r;
            lr += fcmat2[i][j].r;
            lr += fcmat3[i][j].r;
            lr += fcmat4[i][j].r;
            lr += fcmat5[i][j].r;
            lr += fcmat6[i][j].r;

            li += fcmat1[i][j].i;
            li += fcmat2[i][j].i;
            li += fcmat3[i][j].i;
            li += fcmat4[i][j].i;
            li += fcmat5[i][j].i;
            li += fcmat6[i][j].i;

            fcmat[i][j].r = lr / 6.0;
            fcmat[i][j].i = li / 6.0;
        }
    }

    string path = "result/";

    vecToFile(fcmat, 0, 0, path + "C11.bin");
    vecToFile(fcmat, 3, 0, path + "C22.bin");
    vecToFile(fcmat, 1, 0, path + "C12_real.bin");
    vecToFile(fcmat, 1, 1, path + "C12_imag.bin");
    
    return 0;
}