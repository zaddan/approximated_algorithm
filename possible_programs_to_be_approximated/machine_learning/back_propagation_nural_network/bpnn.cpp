// Back Propagation Neural Networks
#include <iostream>
#include <armadillo>
#include <math.h>
using namespace arma;
using namespace std;

#define elif else if
#define MAX_ITER 10000

double lrate = 0.1;
double lambda = 0.0;

int numHiddenLayerNode;
int numOutputNodes;
int numHiddenLayers = 1;

colvec vec2colvec(vector<double>& vec){
    int length = vec.size();
    colvec A(length);
    for(int i=0; i<length; i++){
        A(i) = vec[i];
    }
    return A;
}

rowvec vec2rowvec(vector<double>& vec){
    colvec A = vec2colvec(vec);
    return A.t();
}

mat vec2mat(vector<vector<double> >&vec){
    int cols = vec.size();
    int rows = vec[0].size();
    mat A(rows, cols);
    for(int i = 0; i<rows; i++){
        for(int j=0; j<cols; j++){
            A(i, j) = vec[j][i];
        }
    }
    return A;
}

colvec log(colvec vec){
    for(int i = 0; i < vec.size(); i++){
        vec(i) = log(vec(i));
    }
    return vec;
}

rowvec log(rowvec vec){
    for(int i = 0; i < vec.size(); i++){
        vec(i) = log(vec(i));
    }
    return vec;
}

double sigmoid(double z){
    return 1.0 / (1.0 + exp(-z));
}

rowvec sigmoid(rowvec z){
    for(int i=0; i<z.size(); i++){
        z(i) = sigmoid(z(i));
    }
    return z;
}

colvec sigmoid(colvec z){
    rowvec temp = z.t();
    return (sigmoid(temp)).t();
}

double dsigmoid(double z){
    return z * (1.0 - z);
}

colvec dsigmoid(colvec a){
    colvec one = ones<colvec>(a.size());
    return a % (one - a);
}

rowvec dsigmoid(rowvec a){
    rowvec one = ones<rowvec>(a.size());
    return a % (one - a);
}

vector<colvec> getActivation(mat x, vector<mat>& weightsMatrix, int m){

    vector<colvec> a;
    colvec temp1(x.n_rows);
    a.push_back(temp1);
    for(int i=0; i<numHiddenLayers; i++){
        colvec temp(numHiddenLayerNode);
        a.push_back(temp);
    }
    
    colvec temp2(numOutputNodes);
    a.push_back(temp2);
    colvec one = ones<colvec>(1);
    for(int i = 0; i < a.size(); i++){
        if(i == 0) a[i] = x.col(m);
        else{
            colvec xtemp = a[i - 1];
            xtemp =  join_cols(one, xtemp);
            a[i] = weightsMatrix[i - 1] * xtemp;
            a[i] = sigmoid(a[i]);
        }
    }    
    return a;    
}

//h(xi) is just last vector of a
colvec gethm(vector<colvec> a){
    return a[a.size() - 1];
}

double getCostFunction(mat x, vector<mat>& weightsMatrix, mat y, double lambda){

    int nsamples = x.n_cols;
    double sum = 0.0;
    for(int m = 0; m < nsamples; m++){
        vector<colvec> a = getActivation(x, weightsMatrix, m);
        colvec hx = gethm(a);
        colvec err = hx - y.col(m);
        err = err % err;
        double temp = 0.0;
        for(int i=0; i<err.size(); i++){
            temp += err(i);
        }
        sum += temp / 2;
    }
    sum = sum / (double)nsamples;
    double temp = lambda / 2 / nsamples;
    double sum2 = 0.0;
    for(int i = 0; i < weightsMatrix.size() - 1; i++){
        for(int j = 0; j < weightsMatrix[i].n_cols; j++){
            for(int k = 0; k < weightsMatrix[i].n_rows; k++){
                sum2 += weightsMatrix[i](k, j) * weightsMatrix[i](k, j);
            }
        }
    }
    return sum + temp * sum2;
}

vector<mat> getdJ(mat x, mat y, vector<mat>& weightsMatrix, double lambda){
    //big delta is temp variables for calculating dJ
    //let every variables in bigDelta to be zero.
    vector<mat> bigDelta;
    for(int i=0; i<weightsMatrix.size(); i++){
        mat temp = zeros<mat>(weightsMatrix[i].n_rows, weightsMatrix[i].n_cols);
        bigDelta.push_back(temp);
    }   
    vector<mat> dJ;
    for(int i=0; i<weightsMatrix.size(); i++){
        mat temp = zeros<mat>(weightsMatrix[i].n_rows, weightsMatrix[i].n_cols);
        dJ.push_back(temp);
    }
    int nsamples = x.n_cols;
    //use backProp method
    for(int m = 0; m < nsamples; m++){
        vector<colvec> a = getActivation(x, weightsMatrix, m);
        vector<colvec> tempDelta;
        for(int i=0; i<a.size(); i++){
            colvec temp = zeros<colvec>(a[i].size());
            tempDelta.push_back(temp);
        }
        //no tempDelta[0]
        for(int l = tempDelta.size() - 1; l > 0; l --){
            if(l == tempDelta.size() - 1){
                tempDelta[l] = (a[l] - y.col(m)) % dsigmoid(a[l]);
            }else{
                mat mult = weightsMatrix[l].t() * tempDelta[l + 1];
                tempDelta[l] = mult.rows(1, mult.n_rows - 1) % dsigmoid(a[l]);
            }
        }
        for(int l = 0; l < bigDelta.size(); l++){
            colvec tp = ones<colvec>(1);
            tp =  join_cols(tp, a[l]);
            bigDelta[l] += tempDelta[l + 1] * tp.t();    
        }
        a.clear();
        tempDelta.clear();
    }
    for(int l = 0; l < bigDelta.size(); l++){
        dJ[l] = bigDelta[l] / (double)nsamples;
        dJ[l] = dJ[l] + lambda * weightsMatrix[l];
        for(int j = 0; j < dJ[l].n_rows; j++){
            dJ[l](j, 0) = dJ[l](j, 0) - lambda * weightsMatrix[l](j, 0);
        }            
    }
    return dJ;
}

colvec calculateY(colvec x, vector<mat> weightsMatrix){

    colvec result(x);
    colvec tp = ones<colvec>(1);
    for(int i=0; i<weightsMatrix.size(); i++){
        result = join_cols(tp, result);
        result = weightsMatrix[i] * result;
        result = sigmoid(result);
    }
    return result;
}

void bpnn(vector<vector<double> >&vecX, vector<vector<double> >&vecY, vector<vector<double> >& testX, vector<vector<double> >& testY){

    int nsamples = vecX.size();
    int nfeatures = vecX[0].size();
    //change vecX and vecY into matrix or vector.
    mat y = vec2mat(vecY);
    mat x = vec2mat(vecX);
    numHiddenLayerNode = nfeatures * 5;
    numOutputNodes = vecY[0].size();
    //build weights matrices and randomly initialize them.
    vector<mat> weightsMatrix;
    mat tempmat;
    double init_epsilon = 0.12;
    //input --> first hidden layer:
    tempmat = randu<mat>(numHiddenLayerNode, nfeatures + 1);
    weightsMatrix.push_back(tempmat);
    //hidden layer --> hidden layer :
    for(int i=0; i< numHiddenLayers - 1; i++){
        tempmat = randu<mat>(numHiddenLayerNode, numHiddenLayerNode + 1);
        weightsMatrix.push_back(tempmat);
    }
    //last hidden layer --> output layer:
    tempmat = randu<mat>(numOutputNodes, numHiddenLayerNode + 1);
    weightsMatrix.push_back(tempmat);
    for(int i=0; i<weightsMatrix.size(); i++){
        weightsMatrix[i] = weightsMatrix[i] * (2 * init_epsilon) - init_epsilon;
    }
    //till now, we finished building weights matrices.
    /*
    //Gradient Checking (remember to disable this part after you're sure the 
    //cost function and dJ function are correct)
    vector<mat> dJ = getdJ(x, y, weightsMatrix, lambda);
    cout<<"test!!!!"<<endl;
    double epsilon = 1e-4;
    for(int i=0; i<weightsMatrix.size(); i++){
        cout<<"################ Weight Layer "<<i<<endl;
        for(int j=0; j<weightsMatrix[i].n_rows; j++){
            for(int k=0; k<weightsMatrix[i].n_cols; k++){
                double memo = weightsMatrix[i](j, k);
                weightsMatrix[i](j, k) = memo + epsilon;
                double value1 = getCostFunction(x, weightsMatrix, y, lambda);
                weightsMatrix[i](j, k) = memo - epsilon;
                double value2 = getCostFunction(x, weightsMatrix, y, lambda);
                double tp = (value1 - value2) / (2 * epsilon);
                cout<<j<<", "<<k<<", "<<tp<<", "<<dJ[i](j, k)<<", "<<tp / dJ[i](j, k)<<endl;
                weightsMatrix[i](j, k) = memo;
            }
        }
    }
    */
    int converge = 0;
    double lastcost = 0.0;
    while(converge < MAX_ITER){
        vector<mat> dJ = getdJ(x, y, weightsMatrix, lambda);
        for(int j = 0; j < weightsMatrix.size(); j++){
            weightsMatrix[j] -= lrate * dJ[j];
        }
        double cost = getCostFunction(x, weightsMatrix, y, lambda);
        cout<<"learning step: "<<converge<<", Cost function value = "<<cost<<endl;
        if(fabs((cost - lastcost) ) <= 5e-6 && converge > 0) break;
        lastcost = cost;
        ++ converge;
    }
    cout<<"############result#############"<<endl;
    int correct = 0;
    for(int i=0; i<testX.size(); i++){
        colvec tpcol = vec2colvec(testX[i]);
        colvec result = calculateY(tpcol, weightsMatrix);
        //cout<<result(0)<<endl;
        if((result(0) >= 0.5 && testY[i][0] == 1) || (result(0) < 0.5 && testY[i][0] == 0))
            ++ correct;
    }
    cout<<"right: "<<correct<<", total: "<<testX.size()<<", accuracy: "<<double(correct) / (double)(testX.size())<<endl;
}

int main(int argc, char** argv)
{
    long start, end;
    //read training X from .txt file
    FILE *streamX, *streamY;
    streamX = fopen("trainX.txt", "r");
    int numofX = 30;
    vector<vector<double> > vecX;
    double tpdouble;
    int counter = 0;
    while(1){
        if(fscanf(streamX, "%lf", &tpdouble)==EOF) break;
        if(counter / numofX >= vecX.size()){
            vector<double> tpvec;
            vecX.push_back(tpvec);
        } 
        vecX[counter / numofX].push_back(tpdouble);
        ++ counter;
    }
    fclose(streamX);

    cout<<vecX.size()<<", "<<vecX[0].size()<<endl;

    //read training Y from .txt file
    int numofY = 1;
    streamY = fopen("trainY.txt", "r");
    vector<vector<double> > vecY;
    counter = 0;
    while(1){
        if(fscanf(streamY, "%lf", &tpdouble)==EOF) break;
        if(counter / numofY >= vecY.size()){
            vector<double> tpvec;
            vecY.push_back(tpvec);
        } 
        vecY[counter / numofY].push_back(tpdouble);
        ++ counter;
    }
    fclose(streamY);

    for(int i = 1; i<vecX.size(); i++){
        if(vecX[i].size() != vecX[i - 1].size()) return 0;
    }
    if(vecX.size() != vecY.size()) return 0;

    streamX = fopen("testX.txt", "r");
    vector<vector<double> > vecTX;
    counter = 0;
    while(1){
        if(fscanf(streamX, "%lf", &tpdouble)==EOF) break;
        if(counter / numofX >= vecTX.size()){
            vector<double> tpvec;
            vecTX.push_back(tpvec);
        } 
        vecTX[counter / numofX].push_back(tpdouble);
        ++ counter;
    }
    fclose(streamX);

    streamY = fopen("testY.txt", "r");
    vector<vector<double> > vecTY;
    counter = 0;
    while(1){
        if(fscanf(streamY, "%lf", &tpdouble)==EOF) break;
        if(counter / numofY >= vecTY.size()){
            vector<double> tpvec;
            vecTY.push_back(tpvec);
        } 
        vecTY[counter / numofY].push_back(tpdouble);
        ++ counter;
    }
    fclose(streamY);

    start = clock();
    bpnn(vecX, vecY, vecTX, vecTY);
    end = clock();
    cout<<"used time: "<<((double)(end - start)) / CLOCKS_PER_SEC<<endl;

    return 0;
}
