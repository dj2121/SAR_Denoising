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

    vector<vector<vector<float> > > dirmask(18, vector<vector<float> >((2*winsize+1), vector<float>((2*winsize+1), 0) ) );
   
    vector<float> rchnl(4, 0);

    //eds1 = abs(cmat)
    vector<vector<float> > eds1;
    vector<float> temp(size);

    for(int j = 0; j < cmat.size(); j++){
        for(int i = 0; i < size; i++){
            temp[i] = sqrt(pow(cmat[j][i].r, 2) + pow(cmat[j][i].i, 2));
        }
        eds1.push_back(temp);
    }

    //eds2 = np.angle(cmat)
    vector<vector<float> > eds2;
    for(int j = 0; j < cmat.size(); j++){
        for(int i = 0; i < size; i++){
            temp[i] = atan2(cmat[j][i].i, cmat[j][i].r);
        }
        eds2.push_back(temp);
    }

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

    for(int i = 0; i < cmat.size(); i++){
        float imag = 0;
        for(int j = 0; j < cmat[i].size(); j++){
            imag += cmat[i][j].i;
        }
        if(imag == 0){
            rchnl[i] = 1;
        }
    }

    int tcnt = 0;
    int rowLim = nRows - winsize;
    int colLim = nCols - winsize;
    int ws = winsize + winsize + 1;
    vector<float> ttin((ws)*(ws), 0);

    int sumRchnl = 0;
    for(int i = 0; i < rchnl.size(); i++){
        sumRchnl += rchnl[i];
    }

    vector<vector<vector<cNum> > > cmat2;

    for(int i = 0; i < cmat.size(); i++){
        vector<vector<cNum> > ct1;
        vector<cNum> ct2;
        for(int j = 0; j < cmat[0].size(); j++){
            ct2.push_back(cmat[i][j]);
            if((j+1) % nCols == 0){
                ct1.push_back(ct2);
                ct2.clear();
            }
        }
        cmat2.push_back(ct1);
    }

    vector<vector<vector<cNum> > > cmat3(cmat2.size(), vector<vector<cNum> >(cmat2[0].size() + (2*winsize), vector<cNum>(cmat2[0][0].size()+ (2*winsize)) ) );

    for(int i = 0; i < cmat2.size(); i++){
        for(int j = winsize; j < cmat2[0].size() + winsize; j++){
            for(int k = winsize; k < cmat2[0][0].size() + winsize; k++){
                cmat3[i][j][k].r = cmat2[i][j-winsize][k-winsize].r;
                cmat3[i][j][k].i = cmat2[i][j-winsize][k-winsize].i;
            }
        }
    }

    vector<vector<vector<vector<cNum> > > > tempt(cnt, vector<vector<vector<cNum> > >(cmat2.size(), vector<vector<cNum> >(cmat2[0].size(), vector<cNum>(cmat2[0][0].size()) ) ) );

    int fws = (2*winsize) + 1;

    float res = 0;

    for(int b = 0; b < cnt; b++){
        for(int i = 0; i < cmat2.size(); i++){

            for(int k = winsize; k < cmat2[0].size() + winsize; k++){
                for(int l = winsize; l < cmat2[0][0].size() + winsize; l++){

                    res = 0;

                    for(int n = 0; n < fws; n++){
                        for(int m = 0; m < fws; m++){
                            res += cmat3[i][k+n-winsize][l+m-winsize].r * dirmask[b][n][m];
                        }
                    }

                    tempt[b][i][k-winsize][l-winsize].r = res;

                    res = 0;

                    for(int n = 0; n < fws; n++){
                        for(int m = 0; m < fws; m++){
                            res += cmat3[i][k+n-winsize][l+m-winsize].i * dirmask[b][n][m];
                        }
                    }

                    tempt[b][i][k-winsize][l-winsize].i = res;

                }
            }

        }
    }

    vector<int> sz(3, 0);
    sz[0] = cmat3.size();
    sz[1] = cmat3[0].size();
    sz[2] = cmat3[0][0].size();

    vector<int> rchnl2;

    for(int i = 0; i < rchnl.size(); i++){
        if(rchnl[i]==1){
            rchnl2.push_back(i);
        }
    }


    vector<vector<vector<cNum> > > tempt2(cnt, vector<vector<cNum> >(cmat2.size(), vector<cNum>(cmat2[0].size() * cmat2[0][0].size() ) ) );

    for(int i = 0; i < tempt.size(); i++){
        for(int l = 0; l < tempt[0].size(); l++){
            for(int j = 0; j < cmat2[0].size(); j++){
                for(int k = 0; k < cmat2[0][0].size(); k++){

                    tempt2[i][l][(j*nCols)+k].i = tempt[i][l][j][k].i;
                    tempt2[i][l][(j*nCols)+k].r = tempt[i][l][j][k].r;

                }
            }
        }
    }

    vector<vector<vector<cNum> > > temptr(cnt, vector<vector<cNum> >(rchnl2.size(), vector<cNum>(cmat2[0].size() * cmat2[0][0].size() ) ) );
    
    for(int i = 0; i < temptr.size(); i++){
        for(int l = 0; l < rchnl2.size(); l++){
            for(int j = 0; j < cmat2[0].size() * cmat2[0][0].size(); j++){

                temptr[i][l][j].i = tempt2[i][rchnl2[l]][j].i;
                temptr[i][l][j].r = tempt2[i][rchnl2[l]][j].r;

            }
        }
    }

    vector<vector<cNum> > cmatr(rchnl2.size(), vector<cNum>(cmat[0].size()));

    for(int l = 0; l < rchnl2.size(); l++){
        for(int j = 0; j < cmat2[0].size() * cmat2[0][0].size(); j++){
            cmatr[l][j].i = 0; 
            cmatr[l][j].r = sqrt(pow(cmat[rchnl2[l]][j].r, 2) + pow(cmat[rchnl2[l]][j].i, 2));
        }
    }


    for(int i = 0; i < temptr.size(); i++){
        for(int l = 0; l < temptr[0].size(); l++){
            for(int j = 0; j < temptr[0][0].size(); j++){

                temptr[i][l][j].i -= cmatr[l][j].i;
                temptr[i][l][j].r -= cmatr[l][j].r;

                temptr[i][l][j].r = sqrt(pow(temptr[i][l][j].r, 2) + pow(temptr[i][l][j].i, 2));
                temptr[i][l][j].i = 0;

            }
        }
    }


    vector<vector<float> > tR1(temptr.size(), vector<float>(cmat[0].size()));

    for(int i = 0; i < temptr.size(); i++){
        for(int j = 0; j < temptr[0][0].size(); j++){
            float min = 999999;
            for(int l = 0; l < temptr[0].size(); l++){
                if(temptr[i][l][j].r < min){
                    min = temptr[i][l][j].r;
                }
            }

            tR1[i][j] = min;
        }
    }

    
    vector<int> tR2(cmat[0].size());

    for(int i = 0; i < tR1[0].size(); i++){
        int axis = 0;
        float min = 999999;
        for(int j = 0; j < tR1.size(); j++){
            if(tR1[j][i] < min){

                min = tR1[j][i];
                axis = j;

            }
        }

        tR2[i] = axis;
    }


    for(int i = 0; i < fcmat[0].size(); i++){
        int axis = tR2[i];
        for(int j = 0; j < fcmat.size(); j++){
            fcmat[j][i].r = tempt2[axis][j][i].r;
            fcmat[j][i].i = tempt2[axis][j][i].i;
        }
    }

    cout << "Filtering Done." << endl;
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