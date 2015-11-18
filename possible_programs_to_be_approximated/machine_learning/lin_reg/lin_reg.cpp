#include <armadillo>
#include <vector>
#include <math.h>
using namespace arma;
using namespace std;
struct normal{
    double scale;
    double translate;
};

arma::colvec vec2colvec(vector<double>& vec){
    int length = vec.size();
    arma::colvec A(length);
    for(int i=0; i<length; i++){
        A(i) = vec[i];
    }
    return A;
}

arma::mat vec2mat(vector<vector<double> >&vec){
    int col = vec.size();
    int row = vec[0].size();
    arma::mat A(row, col);
    for(int i = 0; i<col; i++){
        for(int j=0; j<row; j++){
            A(j, i) = vec[i][j];
        }
    }
    return A;
}

normal getScale(vector<double> &vec, double newmin, double newmax){
    double max, min;
    for(int i=0; i<vec.size(); i++){
        if(i == 0){
            max = vec[i];
            min = vec[i];
        }else{
            if(vec[i] > max){
                max = vec[i];
            }
            if(vec[i] < min){
                min = vec[i];
            }
        }
    }
    normal result;
    result.translate = (max - min) / 2 - max;
    result.scale = (max - min) / (newmax - newmin);
    return result;
}

arma::colvec BatchGradientDescent(vector<vector<double> >&vecX, vector<double>& vecY){
    int nfeatures = vecX.size() + 1;
    int nsamples = vecX[0].size();
    //add X0 = 1.
    vector<double> tempvec;
    for(int i=0; i<vecX[0].size(); i++){
        tempvec.push_back(1.0);
    }
    vecX.insert(vecX.begin(), tempvec);
    /*
    //try to normalize vecX
    //X0 should not be normalized.
    //first calculate the scale and translate
    vector<normal> nor;
    normal tpnor;
    tpnor.scale = 1.0;
    tpnor.translate = 0.0;
    nor.push_back(tpnor);
    for(int i = 1; i<nfeatures; i++){
        tpnor = getScale(vecX[i], -1.0, 1.0);
        nor.push_back(tpnor);
    }
    //now do scale and translate
    for(int i=1; i<nfeatures; i++){
        for(int j=0; j<nsamples; j++){
            vecX[i][j] += nor[i].translate;
            vecX[i][j] /= nor[i].scale;
        }
    }
//    cout<<nor[1].translate<<", "<<nor[1].scale<<endl;
    */
    //change vecX and vecY into matrix or vector.
    arma::colvec y = vec2colvec(vecY);
    arma::mat x = vec2mat(vecX);
    //set learning rate;
    double lrate = 0.0001;
    //build theta vector, and randomize initial values
    arma::colvec theta(nfeatures);
    for(int i=0; i<nfeatures; i++){
        theta(i) = 0.0;
    }
    arma::colvec thetatemp;
    int counter = 0;
    while(1){
        arma::rowvec thetaT = theta.t();
        arma::colvec descent(nfeatures);
        for(int j=0; j<nfeatures; j++){
            double sum = 0.0;
            for(int i=0; i<nsamples; i++){
                rowvec xit = x.row(i);
                colvec xi = xit.t();
                double h = as_scalar(thetaT * xi);
                double errortemp = y(i) - h;
                errortemp *= xi(j);
                sum += errortemp;
            }
            sum *= lrate;
            descent(j) = sum;
        }
        thetatemp = theta + descent;
//        cout<<"************** round "<<counter<<endl
//            <<theta<<endl;
        int converge = 0;
        for(int i=0; i<nfeatures; i++){
            if(fabs(theta(i) - thetatemp(i)) / theta(i) > 0.00001) break;
            else ++converge;
        }
        if(converge == nfeatures || ++counter > 100000){
            return thetatemp;
        }
        theta = thetatemp;
    }

}

arma::colvec StochasticGradientDescent(vector<vector<double> >&vecX, vector<double>& vecY){
    int nfeatures = vecX.size() + 1;
    int nsamples = vecX[0].size();
    //add X0 = 1.
    vector<double> tempvec;
    
    for(int i=0; i<vecX[0].size(); i++){
        tempvec.push_back(1.0);
    }
    vecX.insert(vecX.begin(), tempvec);
   
    
    /*
    //try to normalize vecX
    //X0 should not be normalized.
    //first calculate the scale and translate
    vector<normal> nor;
    normal tpnor;
    tpnor.scale = 1.0;
    tpnor.translate = 0.0;
    nor.push_back(tpnor);
    for(int i = 1; i<nfeatures; i++){
        tpnor = getScale(vecX[i], -1.0, 1.0);
        nor.push_back(tpnor);
    }
    //now do scale and translate
    for(int i=1; i<nfeatures; i++){
        for(int j=0; j<nsamples; j++){
            vecX[i][j] += nor[i].translate;
            vecX[i][j] /= nor[i].scale;
        }
    }
//    cout<<nor[1].translate<<", "<<nor[1].scale<<endl;
    */
    //change vecX and vecY into matrix or vector.
    arma::colvec y = vec2colvec(vecY);
    arma::mat x = vec2mat(vecX);
    cout<< y;
    cout<< x;
    //set learning rate;
    double lrate = 0.0001;
    //build theta vector, and randomize initial values
    arma::colvec theta(nfeatures);
    arma::colvec thetatemp(nfeatures);
    for(int i=0; i<nfeatures; i++){
        theta(i) = 0.0;
    }
    int counter = 0;
    while(1){
        thetatemp = theta;
        arma::colvec descent(nfeatures);
        for(int j=0; j<nfeatures; j++){
            int i = 0;
            for(i=0; i<nsamples; i++){
                double thetaj = theta(j);
                arma::rowvec thetaT = theta.t();
                rowvec xit = x.row(i);
                colvec xi = xit.t();
                double h = as_scalar(thetaT * xi);
                double errortemp = y(i) - h;
                errortemp *= xi(j);
                theta(j) += lrate * errortemp;
                if(fabs(theta(j) - thetaj) / theta(j) <= 0.00001) break;
            }
//            cout<<"j = "<<j<<", i = "<<i<<endl;
        }
//        cout<<"************** round "<<counter<<endl
//            <<theta<<endl;
        int converge = 0;
        for(int i=0; i<nfeatures; i++){
            if(fabs(theta(i) - thetatemp(i)) / theta(i) > 0.00001) break;
            else ++converge;
        }
        if(converge == nfeatures || ++counter > 100000){
            return theta;
        }
    }

}

arma::colvec NormalEquation(vector<vector<double> >&vecX, vector<double>& vecY){
    //add X0 = 1.
    vector<double> tempvec;
    for(int i=0; i<vecX[0].size(); i++){
        tempvec.push_back(1.0);
    }
    vecX.insert(vecX.begin(), tempvec);
    arma::colvec y = vec2colvec(vecY);
    arma::mat x = vec2mat(vecX);
    arma::mat xT = x.t();
    arma::mat temp;
    temp = xT * x;
    temp = pinv(temp);
    arma::colvec result = temp * xT * y;
    return result;
}


int main() {
  vector<double> xVec;
  vector<double> yVec;
  vector<double> zVec;
  arma::colvec myColVec;
  vector< vector<double> > featuresVec;
  
  xVec.push_back(1);
  xVec.push_back(4);
  xVec.push_back(5);
  xVec.push_back(3);
  xVec.push_back(9);
  xVec.push_back(18);
  xVec.push_back(2);
  
  yVec.push_back(23);
  yVec.push_back(43);
  yVec.push_back(12);
  yVec.push_back(11);
  yVec.push_back(42);
  yVec.push_back(1);
  yVec.push_back(10);
  
  zVec.push_back(187);
  zVec.push_back(356);
  zVec.push_back(111);
  zVec.push_back(97);
  zVec.push_back(363);
  zVec.push_back(62);
  zVec.push_back(86);
  
  featuresVec.push_back(xVec);
  featuresVec.push_back(yVec);
   
  myColVec = StochasticGradientDescent(featuresVec, zVec);
  //myColVec = BatchGradientDescent(featuresVec, zVec);
  
  cout<<myColVec; 
}
